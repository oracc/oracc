#include <stdlib.h>
#include <stdio.h>
#include <ctype128.h>
#include <string.h>
#include <psd_base.h>
#include <dbi.h>
#include <list.h>
#include "se.h"
#include "types.h"
#include "selib.h"
#include "property.h"
#include "alias.h"
#include "lang.h"
#include "vid.h"

extern struct vid_data *vp;
extern Hash_table *pretrim;

char *strdup(const char *);
char *strndup(const char *,size_t);

extern void debug_results(const char *label, struct Datum *dp);

#ifndef HAVE_STRNDUP
#include "strndup.c"
#endif

const char *se_curr_project = "cdli";
const char *curr_index = "txt";
const char *curr_filter = NULL;

const char *cbd_lang;

int gdf_flag = 0;

struct ix { char *name; Dbi_index*dip; };
static List *indexes = NULL;
Dbi_index *curr_dip = NULL;
Dbi_index *signmap = NULL;

extern void se_pcre_init(const char *project, const char *index);
extern List *se_pcre(const char *rx);

extern struct Datum binop24(enum se_toks bool, struct near *scope, struct Datum d1, struct Datum d2);
extern struct Datum binop(enum se_toks bool, struct near *scope, struct Datum d1, struct Datum d2);
extern int fieldcode(const char *proj,const char *index,const char *fldname);
static void close_indexes(void);
static struct Datum eval_expr_seq(struct token *toks, int lmask, struct token **lastp);
static struct Datum expr(struct token *e, int sign_name);
static struct Datum rexp(struct token *e);
static Dbi_index *open_index(const char *proj,const char *index);
static struct Datum merge_data (List *lp);
static void datum_del (void *vp);
static void filter_fields(struct Datum *dp, int fcode);
static void filter_langs(struct Datum *dp, int lmask);
static void filter_logo(struct Datum *dp);
static void filter_props(struct Datum *dp, int pmask);

static int loc8_cmp (struct location8 *l1, struct location8 *l2);
static int loc16_cmp (struct location16 *l1, struct location16 *l2);
static int loc24_cmp (struct location24 *l1, struct location24 *l2);

static int filter_decay;

void
set_project(struct token *t)
{
#if 0
  if (strchr(t->data,'/') && !strstr(t->data,"/projects"))
    {
      static char buf[128];
      char *bp = buf, *tp = t->data;
      se_curr_project = buf;
      while (*tp != '/')
	*bp++ = *tp++;
      strcpy(bp,"/projects/");
      bp += strlen(bp);
      ++tp;
      while (*tp)
	*bp++ = *tp++;
      *bp = '\0';
    }
  else
#endif
    se_curr_project = t->data;
}
void
set_index(struct token *t)
{
  const char *slash = NULL;
  extern int l2;
  extern const char *return_index;

  curr_index = t->data;

  if (!return_index)
    return_index = curr_index;

  if ((slash = strchr(curr_index,'/')))
    cbd_lang = slash + 1;
  
  if (!(curr_dip = open_index(se_curr_project,curr_index)))
    fprintf(stderr,"se: open failed on %s\n", curr_index);
  if (l2)
    se_vids_init(curr_index);
}
static int
set_filter(struct token *t)
{
  int code = fieldcode(se_curr_project, curr_index, t->data);
  progress("se: %s/%s/%s: code %d\n",
	   se_curr_project, curr_index, t->data, code);
  return code;
}
static int
set_lang(struct token *t)
{
  struct lm_tab *tabp;
  int ret = 0;
  char langbuf[10],*l;
  const char *tp;

  for (tp = ((struct lang_context*)(t->data))->fulltag; *tp; ++tp)
    {
      l = langbuf;
      while (*tp && *tp != '|')
	{
	  if ('-' == *tp)
	    {
	      *l = '\0';
	      while (*tp && *tp != '|')
		++tp;
	    }
	  else
	    {
	      if (l - langbuf > 9)
		{
		  *l = '\0';
		  fprintf(stderr,"se: lang %s too long and not found\n",langbuf);
		  return 0;
		}
	      *l++ = *tp++;
	    }
	}
      *l = '\0';
      tabp = langmask(langbuf,l-langbuf);
      if (tabp)
	{
	  progress("se: lang %s is code %08x\n",langbuf, tabp->lm);
	  ret |= tabp->lm;
	}
      else
	{
	  fprintf(stderr,"se: lang %s not found\n",langbuf);
	  return 0;      
	}
    }
  return ret;
}

static int
set_prop(struct token *t)
{
  struct prop_tab *tabp;
  int ret = 0;
  char buf[10],*l;
  const char *tp;
  const char *curr_prop = t->data;
  
  if (!strcmp(curr_prop,"s"))
    return SIGN_NAME_PROP;

  for (tp = curr_prop; *tp; ++tp)
    {
      l = buf;
      while (*tp && *tp != '|')
	{
	  if (l - buf > 9)
	    {
	      *l = '\0';
	      fprintf(stderr,"se: prop %s too long and not found\n",buf);
	      return 0;
	    }
	  *l++ = *tp++;
	}
      *l = '\0';
      tabp = propmask(buf,l-buf);
      if (tabp)
	{
	  progress("se: prop %s is code %08x in group %08x\n", 
		   buf, tabp->prop, tabp->group);
	  ret |= tabp->prop;
	  ret |= tabp->group;
	}
      else
	{
	  fprintf(stderr,"se: prop %s not found\n",buf);
	  return 0;
	}
    }
  return ret;
}

struct Datum
evaluate(struct token *toks, int fcode, int lmask, struct token **lastused)
{
  struct Datum ret;
  struct Datum arg2;
  static struct token *lastp;
  enum se_toks pending_boolean = se_notused;
  struct near *pending_range = NULL;
  int field_code = fcode;
  int lang_mask = lmask;

  ret.count = 0;

  filter_decay = 0;
  while (toks->type != se_top)
    {
      switch (toks->type)
	{
	case se_project:
	  set_project(toks++);
	  break;
	case se_index:
	  set_index(toks++);
	  lang_mask = lm_any;
	  break;
	case se_filter1:
	  filter_decay = 1;
	case se_filter:
#if 0
	  if (curr_dip->h.data_size == sizeof(struct location8))
#else
	    if (!strcmp((char*)curr_dip->h.name,"cat")
		||!strcmp((char*)curr_dip->h.name,"cbd"))
#endif
	    field_code = set_filter(toks++);
	  else
	    field_code = set_prop(toks++);
	  break;
	case se_lang:
	  lang_mask = set_lang(toks++);
	  break;
	case se_not:
	case se_and:
	case se_or:
	  pending_boolean = toks->type;
	  pending_range = (struct near*)toks->data;
	  ++toks;
	  break;
	case se_logo:
	case se_sign:
	case se_expr:
	case se_rexp:
	  arg2 = eval_expr_seq(toks,lang_mask,&lastp);
	  if (field_code >= 0)
	    {
#if 0
	      if (curr_dip->h.data_size == sizeof(struct location8))
#else
	      if (!strcmp((char*)curr_dip->h.name,"cat")
		  ||!strcmp((char*)curr_dip->h.name,"cbd"))
#endif
		filter_fields(&arg2, field_code);
	      else
		filter_props(&arg2,field_code);
	      if (filter_decay)
		{
		  filter_decay = 0;
		  field_code = 0;
		}
	    }
	  toks = lastp;
	  if (pending_boolean)
	    {
	      if (ret.data_size == sizeof(struct location24)
		  && arg2.data_size == sizeof(struct location24))
		ret = binop24(pending_boolean, pending_range, ret, arg2);
	      else
		ret = binop(pending_boolean, pending_range, ret, arg2);
	      ret.expr_id = -1;
	    }
	  else
	    ret = arg2;
	  break;
	case se_groupo:
	  arg2 = evaluate(++toks,field_code,lang_mask,&lastp);
	  if (verbose)
	    fprintf(stderr,"se eval: group returned %d results\n", 
		    arg2.count);

	  toks = lastp+1;
	  if (pending_boolean)
	    {
	      if (ret.data_size == sizeof(struct location24)
		  && arg2.data_size == sizeof(struct location24))
		ret = binop24(pending_boolean, pending_range, ret, arg2);
	      else
		ret = binop(pending_boolean, pending_range, ret, arg2);
	      if (verbose)
		fprintf(stderr,"se eval: binop returned %d results\n", 
			ret.count);
	    }
	  else
	    ret = arg2;
	  break;
	case se_groupc:
	  *lastused = toks;
	  return ret;
	  break;
	case se_near:
	  pending_range = (struct near*)toks->data;
	  pending_boolean = toks->type;
	  ++toks;
	  break;
	case se_sans:
	case se_space:
	case se_hyphen:
	case se_tilde:
	  fprintf(stderr,"se: impossible state in evaluator\n");
	  exit(-1);
	  break;
	case se_notused:
	case se_xspace:
	case se_top:
	  break;
	default:
	  fprintf(stderr,"se: unhandled token type\n");
	  goto ret;
	  break;
	}
    }
 ret:
  close_indexes();
  return ret;
}

static struct Datum
eval_expr_seq(struct token *toks, int lang_mask, struct token **lastp)
{
  struct Datum left, right, current;
  enum se_toks pending_boolean = se_notused;
  struct near *pending_range = NULL;
  const char *str1;
  int sign_name = 0;
  int role_logo = 0;

  while (toks->type != se_top)
    {
      switch (toks->type)
	{
	case se_sans:
	case se_near:
	case se_space:
	case se_hyphen:
	case se_tilde:
	  pending_range = (struct near*)toks->data;
	  pending_boolean = toks->type;
	  ++toks;
	  break;
	case se_expr:
	case se_rexp:
	  if (toks->type == se_expr)
	    {
	      current = expr(toks, role_logo ? 2 : sign_name);	      
	      sign_name = 0;
	    }
	  else
	    current = rexp(toks);

	  current.expr_id = toks->expr_id;

	  /* several levels of filtering here: grapheme property
	     if appropriate; language; field type */
	  if (role_logo)
	    {
	      if (curr_dip->h.data_size == sizeof(struct location16))
		{
		  filter_logo(&current);
		}
	      else
		fprintf(stderr,"se: 'l:' (logogram) not valid for key");
	    }
	  if (lang_mask)
	    filter_langs(&current, lang_mask);
	  if (pending_boolean != se_notused)
	    {
	      right = current;
	      if (!strcmp(str1,toks->data))
		pending_range->redup = 1;
	      if (left.data_size == sizeof(struct location24)
		  && right.data_size == sizeof(struct location24))
		left = binop24(pending_boolean, pending_range, left, right);
	      else
		left = binop(pending_boolean, pending_range, left, right);
	      if (verbose)
		fprintf(stderr,"se eval: binop returned %d results\n", 
			left.count);

	      if (doing_debug)
		{
		  debug_results("post-binop", &left);
		}
	    }
	  else
	    {
	      left = current;
	      str1 = toks->data;
	    }
	  ++toks;
	  break;
	case se_logo:
	  role_logo = 1;
	  ++toks;
	  break;
	case se_sign:
	  sign_name = 1;
	  ++toks;
	  break;
	default:	  
	  goto ret;
	  break;
	}
    }
 ret:
  *lastp = toks;
  left.expr_id = -1;
  return left;
}

static void
pretrim_data(struct Datum *dp)
{
  int i, dest;

  for (i = dest = 0; i < dp->count; ++i)
    {
      struct location8 *dpp = NULL;
      char buf[128];

      if (dp->data_size == sizeof(struct location8))
	dpp = &((struct location8*)dp->ldata)[i];
      else if (dp->data_size == sizeof(struct location16))
	dpp = (struct location8*)&((struct location16*)dp->ldata)[i];
      else
	dpp = (struct location8*)&((struct location24*)dp->ldata)[i];

      strcpy(buf, vid_get_id(vp,idVal(dpp->text_id),vid_proj_xmd)); /* IS THIS RIGHT? */

      if (!hash_find(pretrim, (unsigned char *)buf))
	{
	  sprintf(buf+strlen(buf), ".%d", dpp->unit_id);
	  if (!hash_find(pretrim, (unsigned char *)buf))
	    continue;
	}

      /* This one is included in the pretrim */
      if (i > dest)
	{
	  if (dp->data_size == sizeof(struct location8))
	    ((struct location8*)dp->ldata)[dest] = ((struct location8*)dp->ldata)[i];
	  else if (dp->data_size == sizeof(struct location16))
	    ((struct location16*)dp->ldata)[dest] = ((struct location16*)dp->ldata)[i];
	  else
	    ((struct location24*)dp->ldata)[dest] = ((struct location24*)dp->ldata)[i];
	}
      ++dest;
    }

  dp->count = dest;
}

static struct Datum
expr(struct token *e, int sign_name)
{
  struct Datum d;
  const char *grapheme = NULL;

  if (sign_name == 2)
    {
      free((char*)e->mangled);
      grapheme = strdup((const char *)keymangler(e->data, 
						 KM_ATF2UTF|KM_GRAPHEME,
						 NULL, 0, NULL));
      e->mangled = (unsigned char *)grapheme;
    }
  else
    grapheme = e->mangled ? e->mangled : e->data;

  d.type = curr_dip->h.ht_user;
  d.expr_id = e->expr_id;
  d.key = (const Uchar*)grapheme;

  if (sign_name)
    {
      if (signmap)
	{
	  dbi_find(signmap,(unsigned char *)grapheme);
	  if (signmap->nfound)
	    {
	      grapheme = dbi_detach_data(signmap,NULL);
	      progress("se eval: grapheme mapped to name %s\n", grapheme);
	    }
	}
    }
  else
    {
      const char *orig_g = grapheme;
      grapheme = alias_get_alias((char*)grapheme);
      if (verbose)
	fprintf(stderr,"grapheme %s = %s after aliasing\n",orig_g,grapheme);
    }
  dbi_find (curr_dip, (const unsigned char *)grapheme);
  if (!curr_dip->nfound)
    {
      progress("se eval: found 0 results for %s\n", grapheme);
      d.ldata = d.rdata = NULL;
      d.l.l8p = d.r.l8p = NULL;
      d.count = 0;
    }
  else
    {
      int i;
      d.data_size = curr_dip->h.data_size;
      d.ldata = dbi_detach_data(curr_dip, &d.count);
      if (pretrim)
	pretrim_data(&d);
      d.l.l8p = malloc(d.count * sizeof(void*));
      if (d.data_size == sizeof(struct location8))
	for (i = 0; i < d.count; ++i)
	  d.l.l8p[i] = &((struct location8*)d.ldata)[i];
      else if (d.data_size == sizeof(struct location16))
	for (i = 0; i < d.count; ++i)
	  d.l.l16p[i] = &((struct location16*)d.ldata)[i];
      else
	for (i = 0; i < d.count; ++i)
	  d.l.l24p[i] = &((struct location24*)d.ldata)[i];

      progress("se eval: found %d result%s for %s\n", 
	       d.count, d.count == 1 ? "" : "s", grapheme); 
      d.r.l8p = NULL;
      d.rdata = NULL;
      if (doing_debug)
	{
	  static char label[128];
	  sprintf(label, "data for key %s", grapheme);
	  debug_results(label, &d);
	}
    }
  d.stripped = 0;
  return d;
}

static struct Datum
rexp(struct token *e)
{
  struct Datum d;
  List *graph_list;
  Hash_table *graphs_seen;
  Uchar *graph;
  List *graph_data;

  d.type = curr_dip->h.ht_user;
  d.expr_id = e->expr_id;
  d.key = e->mangled ? e->mangled : e->data;

  se_pcre_init(se_curr_project,curr_index);
  graph_list = se_pcre(e->mangled ? e->mangled : e->data);
  if (!graph_list)
    {
      d.ldata = d.rdata = NULL;
      d.stripped = d.count = 0;
      d.l.l8p = d.r.l8p = NULL;
      return d;
    }
  graphs_seen = hash_create (0);
  graph_data = list_create (LIST_SINGLE);
  graph = list_first (graph_list); /* flush first 2 entries */
  graph = list_next (graph_list);
  for (graph = list_next (graph_list);
       NULL != graph;
       graph = list_next (graph_list))
    {
      const char *alias = alias_get_alias ((char*)graph);
      static int i = 1;

      if (!hash_find (graphs_seen, (unsigned char *)alias))
	{
	  hash_add (graphs_seen, (unsigned char *)strdup(alias), &i);
	  dbi_find (curr_dip, (unsigned char *)alias);
	  if (curr_dip->nfound)
	    {
	      struct Datum *dp = malloc(sizeof (struct Datum));

	      dp->type = curr_dip->h.ht_user;
	      dp->data_size = curr_dip->h.data_size;
	      dp->ldata = dbi_detach_data(curr_dip, &dp->count);
	      dp->l.l8p = malloc(dp->count * sizeof(struct location8*));
	      if (dp->data_size == sizeof(struct location8))
		for (i = 0; i < dp->count; ++i)
		  dp->l.l8p[i] = &((struct location8*)dp->ldata)[i];
	      else
		for (i = 0; i < dp->count; ++i)
		  dp->l.l16p[i] = &((struct location16*)dp->ldata)[i];
	      progress("se eval: found %d result%s for %s\n", 
		       dp->count, dp->count == 1 ? "" : "s", alias); 
	      dp->rdata = NULL;
	      dp->r.l8p = NULL;
	      dp->stripped = 0;
	      list_add (graph_data, dp);
	    }
	}
    }
  hash_free (graphs_seen, NULL);
  progress("se eval: found %d key%s matching rx %s\n", 
	   list_len(graph_data), list_len(graph_data) == 1 ? "" : "s", 
	   e->mangled ? e->mangled : e->data);
  if (list_len(graph_data))
    d = merge_data(graph_data);
  else
    {
      d.ldata = d.rdata = NULL;
      d.stripped = d.count = 0;
      d.l.l8p = d.r.l8p = NULL;
    }
  progress("se eval: found %d result%s for rx %s\n", 
	   d.count, d.count == 1 ? "" : "s", 
	   e->mangled ? e->mangled : e->data);
  return d;
}

static Dbi_index *
open_index(const char *proj,const char *index)
{
  struct ix *ixp;
  const char *iname;
  const char *index_dir, *index_name;
  static char iname_buf[1024];
  if ((index_name = strchr(index,'/')))
    {
      if (strncmp(index, "gdf", 3))
	{
	  /* it's a language */
	  index_dir = strdup(index);
	  index_name = strndup(index,index_name-index);
	}
      else
	{
	  gdf_flag = 1;
	  index_dir = index;
	  index_name = "cat";
	}
    }
  else
    index_dir = index_name = index;
  
  iname = se_dir(proj,index_dir);
  strcpy(iname_buf,iname);
  if (NULL == indexes)
    {
      indexes = list_create(LIST_SINGLE);
    }
  else
    {
      for (ixp = list_first(indexes); ixp; ixp = list_next(indexes))
	if (!strcmp(iname,ixp->name))
	  break;
      if (ixp)
	{
	  alias_use(ixp->dip->aliases);
	  signmap = ixp->dip->signmap;
	  return ixp->dip;
	}
    }
  ixp = malloc(sizeof(struct ix));
  ixp->name = strdup(index_name);
  ixp->dip = dbi_open(index_name,iname_buf);
  if (ixp->dip)
    {
      progress("se eval: opened index %s\n",iname);
      list_add(indexes,ixp);
      ixp->dip->aliases = alias_init(proj,index);
      if (!access(se_file(proj, index, "signmap.dbi"), R_OK))
	signmap = ixp->dip->signmap = dbi_open("signmap",iname_buf);
      else
	signmap  = ixp->dip->signmap = NULL;
    }
  else
    error(NULL,"se eval: failed to open index %s\n",iname);
  return ixp->dip;
}

static void
ifree(void *vp)
{
  struct ix *ixp = vp;
  dbi_close(ixp->dip);
  free(ixp->name);
  free(ixp);
}

static void
close_indexes()
{
  list_free(indexes,ifree);
}

#define get_count(vp) ((struct Datum*)vp)->count
static struct Datum
merge_data (List *lp)
{
  List_node *base, *short1, *short2, *end;
  struct Datum ret;

  if (NULL == lp)
    {
      ret.count = 0;
      return ret;
    }

  /* turn the list into a circular one if it isn't already */
  if (lp->type != LIST_CIRCULAR)
    {
      lp->last->next = lp->first;
      lp->first->prev = lp->last;
      lp->type = LIST_CIRCULAR;
    }
  base = lp->first;
  short1 = base;
 
  while (base != lp->last)
    {
      /* trap boundary case of list two elements long */
      if (base->next->next == base)
	short2 = base->next;
      else
	{
          for (end = base->next; end != base; end = end->next)
            if (get_count(short1->data) > get_count(end->data))
	      short1 = end;
          short2 = short1->next;
          for (end = short2->next; end != short1; end = end->next)
            if (get_count(short2->data) > get_count(end->data))
	       short2 = end;
	}
      if (short1 == short2)
        fatal ();

      list_add (lp, merge_data_pair (short1->data, short2->data));
      list_delete (lp, short1, datum_del);
      list_delete (lp, short2, datum_del);
      short1 = base = lp->first;
    }
  ret = *(struct Datum*)(list_detach (lp, lp->first));
  list_free (lp, NULL);
  return ret;
}
#undef get_count

struct Datum *
merge_data_pair (struct Datum *d1, struct Datum *d2)
{
  struct Datum *new;
  size_t d1_index, d2_index, new_index;
  int (*cmpfnc)(void*,void*);

  if (d1->data_size != d2->data_size || d1->data_size == sizeof(struct location8))
    cmpfnc = (int(*)(void*,void*))loc8_cmp;
  else if (d1->data_size == sizeof(struct location16))
    cmpfnc = (int(*)(void*,void*))loc16_cmp;
  else if (d1->data_size == sizeof(struct location24))
    cmpfnc = (int(*)(void*,void*))loc24_cmp;

  /* create a new Datum big enough to hold all the results */
  new = malloc(sizeof(struct Datum));
  new->type = d1->type;
  new->data_size = d1->data_size;
  new->count = d1->count + d2->count;
  new->l.l8p = malloc (new->count * sizeof(struct location8*));
  if (d1->rdata || d2->rdata)
    new->r.l8p = malloc (new->count * sizeof(struct location8*));
  else
    new->r.l8p = NULL;
  new->ldata = new->rdata = NULL; /* we access the original [lr]data thru the new ptrs */

  /* go through d1 and d2 adding the lowest one each time to
     new; if d1 == d2, just add one */
  for (d1_index = d2_index = new_index = 0;
       d1_index < d1->count && d2_index < d2->count;
       )
    {
      int res = cmpfnc(d1->l.l8p[d1_index], d2->l.l8p[d2_index]);
      if (res < 0)  /* d1 is first */
	{
	  if (d1->rdata)
	    new->r.l8p[new_index] = d1->r.l8p[d1_index];
	  new->l.l8p[new_index++] = d1->l.l8p[d1_index++];
	}
      else if (res > 0) /* d2 is first */
	{
	  if (d2->rdata)
	    new->r.l8p[new_index] = d2->r.l8p[d2_index];
	  new->l.l8p[new_index++] = d2->l.l8p[d2_index++];
	}
      else  /* d1 == d2 */
	{
	  new->l.l8p[new_index] = d1->l.l8p[d1_index];
	  if (d1->rdata && d2->rdata)
	    {
	      res = cmpfnc(&d1->r.l8p[d1_index], &d2->r.l8p[d2_index]);
	      if (res < 0) /* d2 is higher */
		new->r.l8p[new_index] = d2->r.l8p[d2_index];
	      else
		new->r.l8p[new_index] = d1->r.l8p[d1_index];
	    }
	  else
	    {
	      if (d1->rdata)
		new->r.l8p[new_index] = d1->r.l8p[d1_index];
	      else if (d2->rdata)
		new->r.l8p[new_index] = d2->r.l8p[d2_index];
	    }
	  ++new_index;
	  ++d1_index;
	  ++d2_index;
	}
    }
  while (d1_index < d1->count)
    {
      if (d1->rdata)
	new->r.l8p[new_index] = d1->r.l8p[d1_index];
      new->l.l8p[new_index++] = d1->l.l8p[d1_index++];
    }
  while (d2_index < d2->count)
    {
      if (d2->rdata)
	new->r.l8p[new_index] = d2->r.l8p[d2_index];
      new->l.l8p[new_index++] = d2->l.l8p[d2_index++];
    }
  new->count = new_index;
  return new;
}

static int
loc8_cmp (struct location8 *l1, struct location8 *l2)
{
  if (PQVal(l1->text_id) != PQVal(l2->text_id))
    {
      if (PQVal(l2->text_id)) /* l2 is a Q, so l1 is smaller */
	return -1;
      else
	return 1;
    }
  if (idVal(l1->text_id) - idVal(l2->text_id))
    return idVal(l1->text_id) - idVal(l2->text_id);
  if (l1->unit_id - l2->unit_id)
    return l1->unit_id - l2->unit_id;
  if (l1->word_id - l2->word_id)
    return l1->word_id - l2->word_id;
  return 0;
}

static int
loc16_cmp (struct location16 *l1, struct location16 *l2)
{
  if (PQVal(l1->text_id) != PQVal(l2->text_id))
    {
      if (PQVal(l2->text_id)) /* l2 is a Q, so l1 is smaller */
	return -1;
      else
	return 1;
    }
  if (idVal(l1->text_id) - idVal(l2->text_id))
    return idVal(l1->text_id) - idVal(l2->text_id);
  if (l1->unit_id - l2->unit_id)
    return l1->unit_id - l2->unit_id;
  if (l1->branch_id - l2->branch_id)
    return l1->branch_id - l2->branch_id;
  if (l1->word_id - l2->word_id)
    return l1->word_id - l2->word_id;
  if (l1->start_column - l2->start_column)
    return l1->start_column - l2->start_column;
  if (logo_mask(l1->end_column) - logo_mask(l2->end_column))
    return logo_mask(l1->end_column) - logo_mask(l2->end_column);
  return 0;
}

static int
loc24_cmp (struct location24 *l1, struct location24 *l2)
{
  if (PQVal(l1->text_id) != PQVal(l2->text_id))
    {
      if (PQVal(l2->text_id)) /* l2 is a Q, so l1 is smaller */
	return -1;
      else
	return 1;
    }
  if (idVal(l1->text_id) - idVal(l2->text_id))
    return idVal(l1->text_id) - idVal(l2->text_id);
  if (l1->unit_id - l2->unit_id)
    return l1->unit_id - l2->unit_id;
  if (l1->branch_id - l2->branch_id)
    return l1->branch_id - l2->branch_id;
  if (l1->word_id - l2->word_id)
    return l1->word_id - l2->word_id;
  if (l1->start_column - l2->start_column)
    return l1->start_column - l2->start_column;
  if (logo_mask(l1->end_column) - logo_mask(l2->end_column))
    return logo_mask(l1->end_column) - logo_mask(l2->end_column);

  if ((int)l1->sentence_id - (int)l2->sentence_id)
    return (int)l1->sentence_id - (int)l2->sentence_id;

  if ((int)l1->clause_id - (int)l2->clause_id)
    return (int)l1->clause_id - (int)l2->clause_id;

  if ((int)l1->phrase_id - (int)l2->phrase_id)
    return (int)l1->phrase_id - (int)l2->phrase_id;

  if ((int)l1->lemma_id - (int)l2->lemma_id)
    return (int)l1->lemma_id - (int)l2->lemma_id;

  return 0;
}

void
datum_del (void *vp)
{
  struct Datum*dp = vp;
  if (dp->l.l8p)
    free(dp->l.l8p);
  if (dp->r.l8p)
    free(dp->r.l8p);
#if 0
  if (dp->ldata)
    free(dp->ldata);
  if (dp->rdata)
    free(dp->rdata);
#endif
  free (dp);
}

static void
filter_fields(struct Datum *dp, int fcode)
{
  struct location8 **lp, **lp_ins;
  for (lp = lp_ins = dp->l.l8p; lp - dp->l.l8p < dp->count; ++lp)
    {
      if (*lp && (*lp)->unit_id == fcode)
	{
	  if (lp > lp_ins)
	    *lp_ins = *lp;
	  ++lp_ins;
	}
    }
  dp->count = lp_ins - dp->l.l8p;
  progress("se eval: %d results after filter_fields\n", dp->count);
}

static void
filter_langs(struct Datum *dp, int lmask)
{
  struct location8 **lp, **lp_ins;
  for (lp = lp_ins = dp->l.l8p; lp - dp->l.l8p < dp->count; ++lp)
    {
      int lm = lmVal((*lp)->text_id);
      if (*lp && ((!lm && lmask==lm_any) || (lm&lmask)))
	{
	  if (lp > lp_ins)
	    *lp_ins = *lp;
	  ++lp_ins;
	}
    }
  dp->count = lp_ins - dp->l.l8p;
  progress("se eval: %d results after filter_langs\n", dp->count);
}

static void
filter_logo(struct Datum *dp)
{
  struct location16 **lp, **lp_ins;
  for (lp = lp_ins = dp->l.l16p; lp - dp->l.l16p < dp->count; ++lp)
    {
      if (*lp && is_logo((*lp)->end_column))
	{
	  if (lp > lp_ins)
	    *lp_ins = *lp;
	  ++lp_ins;
	}
    }
  dp->count = lp_ins - dp->l.l16p;
  progress("se eval: %d results after filter_langs\n", dp->count);
}

static void
filter_props(struct Datum *dp, int fcode)
{
  struct location16 **lp, **lp_ins;
  for (lp = lp_ins = dp->l.l16p; lp - dp->l.l16p < dp->count; ++lp)
    {
      if (*lp)
	{
	  if (((fcode&pg_properties) && ((*lp)->properties&(pg_mask(fcode))))
	      || ((fcode&pg_start_column) && ((*lp)->start_column&(pg_mask(fcode)))))
	    {
	      if (lp > lp_ins)
		*lp_ins = *lp;
	      ++lp_ins;
	    }
	}
    }
  dp->count = lp_ins - dp->l.l16p;
  progress("se eval: %d results after filter_props\n", dp->count);
}
