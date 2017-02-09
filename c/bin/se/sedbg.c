/* sedbg: give all sorts of access to SF5 index and support files
 *
 * Primarily intended for debugging when bringing SE up on a new system.
 * Could also be used for regression testing.  Probably the best program
 * to bring up first off when porting because it can be used to exercise
 * a bunch of things in a fairly controlled environment.
 * 
 * Not really intended for the average user ...
 *
 */

#include <psd_base.h>
#include <options.h>
#include <dbi.h>
#include <list.h>
#include <hash.h>
#include <messages.h>
#include "property.h"
#include "vid.h"

#ifndef strdup
char *strdup(const char *);
#endif

#include "types.h"
#include "selib.h"
#include "se.h"
struct vid_data *vp;
const char *return_index = NULL;
extern struct expr_rules rulestab[];
struct expr_rules *ret_type_rules = rulestab;

static int gdf_flag = 0;

enum What_to_do
{
  WTD_FIRSTREC, 
  WTD_GRAPHEMES, 
  WTD_INDEX, 
  WTD_LABELS, 
  WTD_MAP, 
  WTD_SHOW, 
  WTD_DUMP_DISPLAY
};

int l2 = 1;

enum What_to_do what_to_do = WTD_INDEX;
Boolean brief = FALSE, freq = FALSE;
int xml_output = FALSE;
Boolean sort_flag = FALSE;
char *fname, *gname, *lname, *rname, *label_set;
char *domain;
FILE *f_fp, *g_fp, *l_fp, *r_fp;
Hash_table *arg_hash = NULL;
char *ibase = NULL;
List *arglist;
Dbi_index *dip;
const char *iname, *indexname, *project, *curr_project, *curr_index;

static void do_index (List *arglist);
static void setup_args (char *const*argv);

#include "xfuncs.c"

#if 0
void
se_vids_init(const char *index)
{
  const char *vids = se_file (project, l2 ? "cat" : index, "vid.dat");
  vp = vid_load_data(vids);
}
#endif

int
main (int argc, char *const *argv)
{
  options (argc, argv, "02bi:p:");
  setup_args (argv);
  if (!project || !indexname)
    usage();
  se_vids_init(indexname);
  switch (what_to_do)
    {
    case WTD_INDEX:
      do_index (arglist);
      break;
    default:
      fatal ();
      break;
    }
  return 0;
}

static void
setup_args (char *const*argv)
{
  arglist = list_create (LIST_SINGLE);
  if (argv[optind] == NULL)
    {
      ibase = NULL;
      return;
    }
  else
    {
      while (argv[optind] != NULL)
	list_add (arglist, argv[optind++]);
    }
}

static const char *
lmstr(enum langmask lm)
{
  switch (lm)
    {
    case 0:
      return "none";
    case lm_misc:
      return "misc";
    case lm_sux:
      return "sux";
    case lm_sux_es:
      return "sux-es";
    case lm_akk:
      return "akk";
    case lm_qpc:
      return "qpc";
    case lm_qpe:
      return "qpe";
    case lm_any:
      return "any";
    default:
      return "???";
    }
}

static const char *
property_string(Two_bytes p)
{
  static char buf[128];
  int bit;
  *buf = '\0';
  for (bit = 1; bit < (1<<16); bit <<= 1)
    {
      if (p&bit)
	{
	  const char *s = NULL;
	  if (strlen(buf))
	    strcat(buf,"|");
	  switch (bit)
	    {
	    case PROP_WB:
	      s = "wb";
	      break;
	    case PROP_WE:
	      s = "we";
	      break;
	    case PROP_LB:
	      s = "lb";
	      break;
	    case PROP_LE:
	      s = "le";
	      break;
	    default:
	      break;
	    }
	  if (s)
	    strcat(buf,s);
	}
    }
  return buf;
}

static void
p8(struct location8 *l8p)
{
  Four_bytes t = l8p->text_id;

  if ('x' == id_prefix(t))
    {
      char *slash = strrchr(indexname,'/');
      fprintf(stdout, "%s.x%06d",
	      slash+1,XidVal(l8p->text_id));
    }
  else
    {
#if 0
  if ('v' == id_prefix(t))
    {
#endif
      fprintf(stdout,
	      "         t=%s%%%s;u=%d;w=%d",
	      vid_get_id(vp,idVal(t),vid_proj_xmd), /* IS THIS RIGHT? */
	      lmVal(t) ? lmstr(lmVal(t)) : "none",
	      l8p->unit_id, 
	      l8p->word_id
	      );
#if 0
    }
  else
    {
      fprintf(stdout,
	      "         t=%c%06d;%%%s;u=%d;w=%d",
	      id_prefix(t),
	      XidVal(t),
	      lmVal(t) ? lmstr(lmVal(t)) : "none",
	      l8p->unit_id, 
	      l8p->word_id
	      );
    }
#endif
    }
}


static void
p16(struct location16 *l16p)
{
  p8((struct location8*)l16p);
  fprintf(stdout,
	  ";br=%d;sc=%d;ec=%d;props=%s",
	  l16p->branch_id, 
	  mask_sc(l16p->start_column),
	  logo_mask(l16p->end_column),
	  property_string(l16p->properties));
  if (is_logo(l16p->end_column))
    fprintf(stdout, ";logo=1");
  if (prop_sc(l16p->start_column))
    {
      selemx_print_fields(stdout, prop_sc(l16p->start_column));
      fflush(stdout);
    }
}

static void
do_index (List *arglist)
{
  Uchar *key;
  char idxbuf[128], *slash;

  if ((slash = strchr(indexname,'/')))
    {
      strncpy(idxbuf,indexname,slash-indexname);
      idxbuf[slash-indexname] = '\0';
    }
  else
    strcpy(idxbuf,indexname);

  if (list_len (arglist))
    {
      Uchar *k;
#if 0
      alias_init ("");
#endif
      arg_hash = hash_create (0);
      for (k = list_first(arglist); k != NULL; k = list_next(arglist))
	{
#if 0
	  k = alias_get_alias(k);
#endif
	  hash_add (arg_hash, k, strdup((const char *)k));
	}
#if 0
      alias_term ();
#endif
    }

  iname = se_dir(project,indexname);
  dip = dbi_open (idxbuf,iname);

  ret_type_rules = &rulestab[dip->h.ht_user];

  if (l2)
    se_vids_init(indexname);

  while (NULL != (key = dbi_each (dip)))
    {
      const char *res;
      if (arg_hash != NULL 
	  && (NULL == (res = hash_find (arg_hash, key))))
	  /*	      || strcmp(res,(const char *)key))) */
	continue;
      if (freq)
	  fprintf (stdout, "%s\t%lu\n", key, (unsigned long)dip->nfound);
      else
	{
	  fprintf(stdout, "key %s has %lu locations\n", key, (unsigned long)dip->nfound);
	  if (!brief)
	    {
	      if (dip->h.data_size == sizeof(struct location8)
		  /* || 'x' == ret_type_rules->pos_id_prefix */)
		{
		  struct location8 *buf = dip->data;
		  size_t i;
		  for (i = 0; i < dip->nfound; ++i)
		    {
		      p8(&buf[i]);
		      fputc('\n', stdout);
		    }
		}
	      else if (dip->h.data_size == sizeof(struct location16))
		{
		  struct location16 *buf = dip->data;
		  size_t i;
		  for (i = 0; i < dip->nfound; ++i)
		    {
		      if ('x' == ret_type_rules->pos_id_prefix)
			p8((struct location8*)&buf[i]);
		      else
			p16((struct location16*)&buf[i]);
		      fputc('\n', stdout);
		    }
		}
	      else
		{
		  struct location24 *buf = dip->data;
		  size_t i;
		  for (i = 0; i < dip->nfound; ++i)
		    {
		      p16((struct location16*)&buf[i]);
		      fprintf(stdout, 
			      "; s=%d;c=%d;p=%d;l=%d",
			      buf[i].sentence_id,
			      buf[i].clause_id,
			      buf[i].phrase_id,
			      buf[i].lemma_id);
		      fputc('\n', stdout);
		    }
		}
	    }
	}
    }
  dbi_close (dip);
}

/**************************************************************************
 * Routines required for the PSD library
 */
const char *prog = "sedbg";
int major_version = 1, minor_version = 0;
const char *usage_string = "[-b] [-i <index>] [-p <project>] [<keys>]";

void
help ()
{
  printf("  -0  use zero-buffering for stdout\n");
  printf("  -b  brief output\n");
  printf("  -f  frequency output\n");
  printf("  -i  set index\n");
  printf("  -p  set project\n");
  printf("\n");
  printf("If <keys> are given the output is restricted to those items.\n");
  printf("\n");
}

int
opts (int c, char *arg)
{
  switch (c)
    {
    case '0':
      setbuf (stdout, 0);
      break;
    case '2':
      l2 = 1;
      break;
    case 'b':
      brief = TRUE;
      break;
    case 'f':
      freq = TRUE;
      break;
    case 'i':
      return_index = indexname = curr_index = arg;
      break;
    case 'p':
      project = curr_project = arg;
      break;
    default:
      return 1;
      break;
    }
  return 0;
}

void
se_vids_init(const char *index)
{
  const char *vids = se_file (curr_project, (l2&&!gdf_flag) ? "cat" : index, "vid.dat");
  if (!xaccess(vids, R_OK, 0))
    vp = vid_load_data(vids);
  else
    vp = NULL;
}
