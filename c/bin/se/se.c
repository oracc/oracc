#include <stdio.h>
#include <stdlib.h>
#include <psd_base.h>
#include <messages.h>
#include <options.h>
#include <list.h>
#include <hash.h>
#include <loadfile.h>
#include <dbi.h>
#include <locale.h>
#include "types.h"
#include "se.h"
#include "selib.h"
#include "atflocale.h"
#include "charsets.h"
#include "lang.h"
#include "vid.h"

int any_index = 0;
int do_uniq = 1;
int l2 = 1;
int s2 = 1;
int show_count = 0;
int show_tokens = 0;
int textresult = 0;
int verbose = 0;
const char *xmldir = NULL;
const char *outfile = NULL;
const char *project = NULL;
const char *return_index = NULL;
FILE*out_f = NULL, *f_log;
enum result_granularity res_gran = g_not_set, best_res_gran = g_not_set;
static struct Datum result;
enum vid_proj vid_display_proj;
char *pretrim_file = NULL;
unsigned char **pretrim_lines, *pretrim_content;
List *pretrim_args = NULL;
Hash_table *pretrim = NULL;
FILE *f_mangletab = NULL;

extern const char *se_curr_project, *curr_index, *cbd_lang;

const char *curr_project, *textid;

extern struct expr_rules *ret_type_rules;
extern struct Datum evaluate(struct token *toks, int fcode, int lmask,
			     struct token **lastused);

static const char **anytoks(const char *project, const char *index, const char **toks);
static void put_results(struct Datum *res);
static void run_search(struct token*toks);
static void uniq (struct Datum *dp);

#include "xfuncs.c"

struct vid_data *vp;

extern int gdf_flag;

void
se_vids_init(const char *index)
{
  const char *vids = se_file (se_curr_project, (l2&&!gdf_flag) ? "cat" : index, "vid.dat");
  if (!xaccess(vids, R_OK, 0))
    vp = vid_load_data(vids);
  else
    vp = NULL;
}

void
debug_location8(struct location8 *l8p, struct location8 *r8p)
{
  fprintf(f_log, "%c%06d.%d.%d\n", 
	  id_prefix(l8p->text_id), XidVal(l8p->text_id),
	  l8p->unit_id, l8p->word_id);
  
  fputs(" :: ", f_log);
  
  if (r8p)
    {
      fprintf(f_log, "%c%06d.%d.%d\n", 
	      id_prefix(r8p->text_id), XidVal(r8p->text_id),
	      r8p->unit_id, r8p->word_id);
    }
}

static int nl_16 = 1;

void
debug_location16(struct location16 *l16p, struct location16 *r16p)
{
  fprintf(f_log, "%c%06d.%d.%d b=%d; sc=%d; ec=%d", 
	  id_prefix(l16p->text_id), XidVal(l16p->text_id),
	  l16p->unit_id, l16p->word_id,
	  l16p->branch_id, mask_sc(l16p->start_column), logo_mask(l16p->end_column)
	  );

  if (prop_sc(l16p->start_column))
    selemx_print_fields(f_log, prop_sc(l16p->start_column));

  fputs(" :: ", f_log);
	
  if (r16p)
    {
      fprintf(f_log, "%c%06d.%d.%d b=%d; sc=%d; ec=%d", 
	      id_prefix(r16p->text_id), XidVal(r16p->text_id),
	      r16p->unit_id, r16p->word_id,
	      r16p->branch_id, mask_sc(r16p->start_column), logo_mask(r16p->end_column)
	      );
	    
      if (prop_sc(r16p->start_column))
	selemx_print_fields(f_log, prop_sc(r16p->start_column));
    }

  if (nl_16)
    fputc('\n', f_log);
}

void
debug_location24(struct location24 *l24p, struct location24 *r24p)
{
  nl_16 = 0;
  debug_location16((struct location16 *)l24p, (struct location16 *)r24p);
  if (l24p->sentence_id)
    fprintf(f_log, 
	    " s=%d;c=%d;p=%d;l=%d",
	    l24p->sentence_id,
	    l24p->clause_id,
	    l24p->phrase_id,
	    l24p->lemma_id);
  if (r24p && r24p->sentence_id)
    fprintf(f_log, 
	    " :: s=%d;c=%d;p=%d;l=%d",
	    r24p->sentence_id,
	    r24p->clause_id,
	    r24p->phrase_id,
	    r24p->lemma_id);

  fputc('\n', f_log);
  nl_16 = 1;
}

void
debug_message(const char *msg)
{
  fprintf(f_log,"=====begin: %s====\n", msg);
}

void
debug_results(const char *label, struct Datum *dp)
{
  int i;
  
  fprintf(f_log,"=====begin: %s [key=%s]====\n", label, dp->key);

  if (dp->data_size == sizeof(struct location8))
    for (i = 0; i < dp->count; ++i)
      debug_location8(dp->l.l8p[i], dp->r.l8p ? dp->r.l8p[i] : NULL);
  else if (dp->data_size == sizeof(struct location16))
    for (i = 0; i < dp->count; ++i)
      debug_location16(dp->l.l16p[i], dp->r.l16p ? dp->r.l16p[i] : NULL);
  else
    for (i = 0; i < dp->count; ++i)
      debug_location24(dp->l.l24p[i], dp->r.l24p ? dp->r.l24p[i] : NULL);

  fprintf(f_log,"=====end: %s [key=%s]=====\n", label, dp->key);
  fflush(f_log);
}

#define more_in_unit(curr,next) \
  (!s2									\
   && i+1 < dp->count							\
   && id_prefix((curr)->text_id) == id_prefix((next)->text_id)		\
   && XidVal((curr)->text_id) == XidVal((next)->text_id)		\
   && (curr)->unit_id == (next)->unit_id)

#define more_in_unit24(curr,next) \
  (!s2									\
   && i+1 < dp->count							\
   && id_prefix((curr)->text_id) == id_prefix((next)->text_id)		\
   && XidVal((curr)->text_id) == XidVal((next)->text_id)		\
   && ((struct location24*)curr)->sentence_id == ((struct location24*)next)->sentence_id)

void
show_results(struct Datum *dp)
{
  int i;
  switch (res_gran)
    {
    case g_not_set:
    case g_text:
    case g_record:
      for (i = 0; i < dp->count; ++i)
	{
	  struct location8 *l8p = dp->l.l8p[i];
	  if ('x' == id_prefix(l8p->text_id))
	    fprintf(out_f, "%s.x%06d\n", 
		    cbd_lang ? cbd_lang : se_curr_project, XidVal(l8p->text_id));
	  else
	    fprintf(out_f, "%c%06d\n", 
		    id_prefix(l8p->text_id), XidVal(l8p->text_id));
	}
      break;
    case g_field:
      for (i = 0; i < dp->count; ++i)
	{
	  struct location8 *l8p = dp->l.l8p[i];
	  fprintf(out_f, "%c%06d.%d\n",
		 id_prefix(l8p->text_id), XidVal(l8p->text_id),
		 l8p->unit_id);
	}
      break;
    case g_word:
    case g_grapheme:
      for (i = 0; i < dp->count; ++i)
	{
	  int last_id_printed = 0;
	  struct location8 *l8p = dp->l.l8p[i];
	  fprintf(out_f, "%c%06d.%d.%d", 
		 id_prefix(l8p->text_id), XidVal(l8p->text_id),
		 l8p->unit_id,
		 l8p->word_id);
	  while (more_in_unit(l8p, dp->l.l8p[i+1]))
	    {
#if 0
	      if (l8p->word_id != dp->l.l8p[i+1]->word_id)
#else
		if (dp->l.l8p[i+1]->word_id != last_id_printed)
#endif
		{
		  fprintf(out_f, ",%d",dp->l.l8p[++i]->word_id);
		  l8p = dp->l.l8p[i];
		  last_id_printed = l8p->word_id;
		}
	      else
		++i;
	    }
	  fputc('\n',out_f);
	}
      break;
    }
}

static const char *
set_tra_suffix(Two_bytes_s branch_id)
{
#if 0
  int trcode = get_trcode(branch_id);
  return trcodes[trcode];
#endif
  if (l2)
    return "_project-en";
  else
    return "";
}

static void
pretrim_setup(void)
{
  unsigned char *p;
  static int true = 1;
  pretrim = hash_create(1000);
  if (pretrim_args)
    for (p = list_first(pretrim_args); p; p = list_next(pretrim_args))
      hash_add(pretrim, p, &true);
  else
    {
      int i;
      pretrim_lines = loadfile_lines3((unsigned char *)pretrim_file, NULL, &pretrim_content);
      for (i = 0; pretrim_lines[i]; ++i)
	hash_add(pretrim, pretrim_lines[i], &true);
    }
}

static void
vid_show_results(struct Datum *dp)
{
  int i;
  switch (res_gran)
    {
    case g_not_set:
    case g_text:
    case g_record:
      for (i = 0; i < dp->count; ++i)
	{
	  struct location8 *l8p = dp->l.l8p[i];
	  fprintf(out_f, "%s\n", 
		  vid_get_id(vp,idVal(l8p->text_id), vid_display_proj));
	}
      break;
    case g_field:
      for (i = 0; i < dp->count; ++i)
	{
	  struct location8 *l8p = dp->l.l8p[i];
	  fprintf(out_f, "%s.%d\n",
		  vid_get_id(vp,idVal(l8p->text_id), vid_display_proj),
		  l8p->unit_id);
	}
      break;
    case g_word:
    case g_grapheme:
      if (dp->data_size == sizeof(struct location24))
	{
	  for (i = 0; i < dp->count; ++i)
	    {
	      struct location8 *l8p = dp->l.l8p[i];
	      const char *tra = "";
	      
	      if (return_index && !strcmp(return_index,"tra"))
		tra = set_tra_suffix(dp->l.l16p[i]->branch_id);
	      
	      fprintf(out_f, "%s%s.%d.%d", 
		      vid_get_id(vp,idVal(l8p->text_id), vid_display_proj), 
		      tra,
		      l8p->unit_id,
		      l8p->word_id);
	      while (more_in_unit24(l8p, dp->l.l8p[i+1]))
		{
		  if (l8p->word_id != dp->l.l8p[i+1]->word_id)
		    {
		      /* FIXME: both the fprinted l8p[i]
			 here were l8p[++i] which must be
			 wrong--but with new wmapper this 
			 code is going to go away anyway */			
		      ++i;
		      fprintf(out_f, ",%d.%d",
			      dp->l.l8p[i]->unit_id,
			      dp->l.l8p[i]->word_id);
		      l8p = dp->l.l8p[i];
		    }
		  else
		    ++i;
		}
	      fputc('\n',out_f);
	    }
	}
      else
	{
	  for (i = 0; i < dp->count; ++i)
	    {
	      struct location8 *l8p = dp->l.l8p[i];
	      const char *tra = "";
	      
	      if (return_index && !strcmp(return_index,"tra"))
		tra = set_tra_suffix(dp->l.l16p[i]->branch_id);
	      
	      fprintf(out_f, "%s%s.%d.%d", 
		      vid_get_id(vp,idVal(l8p->text_id), vid_display_proj), 
		      tra,
		      l8p->unit_id,
		      l8p->word_id);
	      while (more_in_unit(l8p, dp->l.l8p[i+1]))
		{
		  if (l8p->word_id != dp->l.l8p[i+1]->word_id)
		    {
		      fprintf(out_f, ",%d",dp->l.l8p[++i]->word_id);
		      l8p = dp->l.l8p[i];
		    }
		  else
		    ++i;
		}
	      fputc('\n',out_f);
	    }
	}
      break;
    }
}

int
main(int argc, char * const*argv)
{
  struct token *toks = NULL;
  int ntoks = 0;

  f_log = stderr;
  exit_on_error = TRUE;
  setlocale(LC_ALL,LOCALE);
  options(argc, argv, "28acdg:j:o:p:P:stuvx:");
  if (!out_f)
    out_f = stdout;

  if (doing_debug)
    f_log = fopen("se.dbg","w");

  /*atf2utf8_init();*/
  charsets_init();
  langtag_init();
  tokinit();

  if (pretrim_file || pretrim_args)
    pretrim_setup();

  if (show_tokens)
    {
      if (xmldir)
	toks = tokenize(xmldir_toks(xmldir),&ntoks);
      else
	toks = tokenize((const char **)(argv+optind),&ntoks);
      showtoks(toks,ntoks);
    }
  else
    {
      if (any_index)
	{
	  const char *index[] = { "!cat" , "!txt" , "!tra" , "!lem", NULL, "!esp" , NULL };
	  struct Datum results[4];
	  int i, best_findset = -1;
	  char *hashproj = NULL;
	  FILE *anyout = stdout;
	  const char **toklist = NULL;

	  if (!project)
	    {
	      fprintf(stderr, "se: must give -j PROJECT option when using -a\n");
	      exit(1);
	    }

	  if (xmldir)
	    {
	      char *anyfile = malloc(strlen(xmldir)+5);
	      sprintf(anyfile,"%s/any",xmldir);
	      anyout = xfopen(anyfile,"w");
	    }

	  hashproj = malloc(strlen(project) + 2);
	  sprintf(hashproj, "#%s",project);

	  for (i = 0; index[i]; ++i)
	    {
	      if (!strcmp(index[i], "!lem"))
		{
		  if (best_findset > 0)
		    continue;
		}

	      toklist = anytoks(hashproj, index[i],
				xmldir ? xmldir_toks(xmldir) : (const char **)(argv+optind));
	      toks = tokenize(toklist,&ntoks);
	      /*showtoks(toks,ntoks);*/
	      run_search(toks);
	      results[i] = result;
	      if (result.count
		  && (best_findset < 0 
		      || (best_findset >= 0 && results[best_findset].count < result.count)))
		{
		  best_findset = i;
		  best_res_gran = res_gran;
		}
	    }
	  if (best_findset >= 0)
	    {
	      fprintf(anyout, "%s %lu\n", index[best_findset], 
		      (unsigned long)results[best_findset].count);
	      for (i = 0; index[i]; ++i)
		{
		  if (i == best_findset)
		    continue;
		  if (results[i].count)
		    fprintf(anyout, "%s %lu\n", index[i], (unsigned long)results[i].count);
		}
	      res_gran = best_res_gran;
	      return_index = &index[best_findset][1];
	      put_results(&results[best_findset]);
	    }
	  else
	    
	  fclose(anyout);
	}
      else
	{
	  toks = tokenize(xmldir ? xmldir_toks(xmldir) : (const char **)(argv+optind),&ntoks);
	  run_search(toks);
	  put_results(&result);
	}
    }

  if (pretrim)
    {
      hash_free(pretrim, NULL);
      pretrim = NULL;
      free(pretrim_lines);
      free(pretrim_content);
      pretrim_content = NULL;
      pretrim_lines = NULL;
      if (pretrim_args)
	{
	  list_free(pretrim_args, NULL);
	  pretrim_args = NULL;
	}
    }
  langtag_term();

  return 0;
}

static const char **
anytoks(const char *project, const char *index, const char **toks)
{
  const char **ret_toks = NULL;
  int ntoks = 0;
  while (toks[ntoks])
    ++ntoks;
  ret_toks = malloc((ntoks+3) * sizeof(const char *));
  ret_toks[0] = project;
  ret_toks[1] = index;
  for (ntoks = 0; toks[ntoks]; ++ntoks)
    ret_toks[ntoks+2] = toks[ntoks];
  ret_toks[ntoks+2] = NULL;
  return (const char **)ret_toks;
}

static void
run_search(struct token*toks)
{
  binop_init();
  binop24_init();
  result = evaluate(toks, -1, lm_any, NULL);
  progress("se: result.count == %lu\n", (unsigned long)result.count);

  if (do_uniq && (res_gran == g_word || res_gran == g_grapheme))
    do_uniq = 0;

  if (do_uniq && result.count > 1)
    {
      uniq(&result);
      progress("se: post-uniq result.count == %lu\n", (unsigned long)result.count);
    }
}

static void
put_results(struct Datum *res)
{
  if (show_count)
    printf("%lu\n",(unsigned long)res->count);
  else if (res->count)
    {
      if (xmldir)
	xmldir_results(xmldir,res->count);
      if ('v' == id_prefix(res->l.l8p[0]->text_id))
	{
	  if (!strcmp(return_index, "cat"))
	    vid_display_proj = vid_proj_xmd;
	  else
	    vid_display_proj = vid_proj_xtf;
	  vid_show_results(res);
	}
      else
	show_results(res);
    }
  else if (xmldir)
    xmldir_results(xmldir,res->count);
}

static void
uniq (struct Datum *dp)
{
  size_t end = 0, last = dp->count - 1, insert_point;

  while (end < last 
	 && (dp->l.l8p[end]->text_id != dp->l.l8p[end+1]->text_id
	     && (textresult
		 || dp->l.l8p[end]->unit_id != dp->l.l8p[end+1]->unit_id)))
    ++end;

  if (end < last) /* there are duplicates */
    {
      /* insert point is at the first of a series of duplicates:
	 it is the last duplicate that gets copied to insert_point
	 when the skip duplicates loop breaks */	 
      insert_point = end;
      while (1)
	{
	  size_t lastend = end;
	  /* skip duplicates: [NO LONGER] guaranteed to be true at least once */
	  while (end < last && 
		 (dp->l.l8p[end]->text_id == dp->l.l8p[end+1]->text_id
		  && (textresult 
		      || dp->l.l8p[end]->unit_id == dp->l.l8p[end+1]->unit_id)))
	    ++end;

	  if (end == lastend)
	    {
	      dp->l.l8p[insert_point] = dp->l.l8p[end];
	      if (NULL != dp->r.l8p)
		dp->r.l8p[insert_point] = dp->r.l8p[end];
	      ++insert_point;
	      ++end;
	    }
	  else
	    {
	      /* move non-duplicates backwards */
	      while (end < last 
		     && (dp->l.l8p[end]->text_id != dp->l.l8p[end+1]->text_id
			 && (textresult
			     || dp->l.l8p[end]->unit_id != dp->l.l8p[end+1]->unit_id)))
		{
		  dp->l.l8p[insert_point] = dp->l.l8p[end];
		  if (NULL != dp->r.l8p)
		    dp->r.l8p[insert_point] = dp->r.l8p[end];
		  ++insert_point;
		  ++end;
		}
	    }  
	  
	  if (end == last) /* no more duplicates possible */
	    {
	      dp->l.l8p[insert_point] = dp->l.l8p[end];
	      if (NULL != dp->r.l8p)
		dp->r.l8p[insert_point] = dp->r.l8p[end];
	      ++insert_point;
	      break;
	    }
	}
      dp->count = insert_point;
    }
}

int
opts(int argc, char *arg)
{
  switch (argc)
    {
    case '2':
      l2 = 1;
      break;
    case '8':
      use_unicode = 1;
      break;
    case 'a':
      any_index = 1;
      break;
    case 'c':
      show_count = 1;
      break;
    case 'd':
      doing_debug = 1;
      break;
    case 'j':
      project = arg;
      break;
    case 'o':
      outfile = arg;
      out_f = xfopen(outfile,"w");     
      break;
    case 'p':
      pretrim_file = arg; /* etcsri:Q000834 or etcsri:Q000834.2 */
      break;
    case 'P':
      if (!pretrim_args)
	pretrim_args = list_create(LIST_SINGLE);
      list_add(pretrim_args, arg);
      break;
    case 's':
      s2 = 1;
      break;
    case 't':
      show_tokens = 1;
      break;
    case 'u':
      do_uniq = 0;
      break;
    case 'v':
      verbose = 1;
      break;
    case 'x':
      xmldir = arg;
      break;
    default:
      return 1;
    }
  return 0;
}

const char *prog = "se";
int major_version = 1, minor_version = 0;
const char *usage_string = " #<PROJ> !<IDX> %<LNG> [search terms]";
void
help ()
{
  printf("  -c = show count of results\n");
  printf("  -d = output debugging information\n");
  printf("  -t = show tokens and exit\n");
  printf("  -u = do not uniq results\n");
  printf("  -v = verbose operation\n");
}

const char *file; /* for libatf */
int lnum; /* for libatf */
