/* Grapheme Validation Library */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <wchar.h>
#include <wctype.h>

#include <npool.h>
#include <hash.h>
#include <ctype128.h>
#include <atf.h>
#include <gdl.h>
#include <memblock.h>
#include <sexify.h>

#include "gvl.h"

static gvl_i *sl = NULL; /* sl is always the head of the list of signlist datasets, not necessarily the current one */
static gvl_i *curr_sl = NULL; /* the sl that should be used for look up */
static int tsv = 0;
static int gvl_trace = 0;

int gvl_strict = 0;

static const char *report = "http://oracc.museum.upenn.edu/ogsl/reportingnewvalues/";

const unsigned char *(*gvl_lookup)(unsigned const char *key);

unsigned char *gvl_v_from_h(const unsigned char *b, const unsigned char *qsub);
unsigned char *gvl_val_base(const unsigned char *v);

static gvl_i *gvl_i_init_d(const char *name, Dbi_index *dbi);
static gvl_i *gvl_i_init_h(const char *name, Hash_table *h);
static void   gvl_i_term(const char *name);

#define ccp const char *
#define ucp unsigned char *
#define uccp unsigned const char *

static unsigned char *
snames_of(unsigned const char *oids)
{
  List *l = list_create(LIST_SINGLE);
  unsigned char *xoids = (ucp)strdup((ccp)oids), *xoid, *x, *ret;
  x = xoids;
  while (*x)
    {
      xoid = x;
      while (*x && ' ' != *x)
	++x;
      if (*x)
	*x++ = '\0';
      list_add(l,(void*)gvl_lookup(xoid));
    }
  ret = list_concat(l);
  list_free(l,NULL);
  free(xoids);
  return ret;
}

static unsigned char *
gvl_vmess(char *s, ...)
{
  unsigned char *ret = NULL;
  if (s)
    {
      va_list ap, ap2;
      char *e;
      int need;
      
      va_start(ap, s);
      va_copy(ap2, ap);
      need = vsnprintf(NULL, 0, s, ap);
      e = malloc(need+1);
      vsprintf(e, s, ap2);
      va_end(ap2);
      va_end(ap);
      ret = npool_copy((unsigned char *)e, curr_sl->p);
      free(e);
    }
  return ret;
}

#define gvl_BAD ""
#define gvl_v "v"
#define gvl_s "s"
#define gvl_n "n"
#define gvl_p "p"
#define gvl_c "cs"
#define gvl_q "qv"

static int
gnr(const char *g)
{
  return ((*g == 'n' || *g == 'N') && (g[1]=='(' || g[1]=='+')) ? '1' : 0;
}

static const char *
gvl_type(unsigned const char *g)
{
  if ((*g > 127 || isalpha(*g) || *g == '\'') && !gnr((const char *)g))
    {
      if (strchr((ccp)g,'('))
	{
	  while (*g)
	    if ('!' == *g)
	      return gvl_v;
	    else if ('(' == *g)
	      return gvl_q;
	    else
	      ++g;
	  abort(); /* can't happen */
	}
      else
	{
	  if (u_isupper(g))
	    {
	      g += mbtowc(NULL,(const char *)g,6);
	      while (*g)
		if (u_islower(g))
		  return gvl_v;
		else if (isdigit(*g) || '@' == *g || '~' == *g)
		  break;
		else
		  g += mbtowc(NULL,(const char *)g,6);
	      return gvl_s;
	    }
	  else
	    return gvl_v;
	}
    }
  else if (isdigit(*g) || *g == 'n' || *g == 'N')
    return gvl_n;
  else if (*g == '|')
    {
      register unsigned const char *e = g;
      while (*++e != '|')
	;
      if (*e == '(')
	return gvl_q;
      else
	return gvl_c;
    }
  else if (*g == '*' || *g == ':' || *g == '/')
    return gvl_p;
  else
    return gvl_BAD;
}

unsigned const char *
gvl_cuneify(unsigned const char *g)
{
  return gvl_cuneify_gv(gvl_validate(g));
}

unsigned const char *
gvl_cuneify_gv(gvl_g*gg)
{
  if (gg)
    {
      if (!gg->utf8)
	gg->utf8 = gvl_lookup(gvl_tmp_key((uccp)gg->oid,"uchar"));
      return gg->utf8;
    }
  else
    return NULL;
}

unsigned const char *
gvl_ucode(gvl_g*gg)
{
  if (gg)
    {
      if (!gg->uhex)
	gg->uhex = gvl_lookup(gvl_tmp_key((uccp)gg->oid,"ucode"));
      return gg->uhex;
    }
  else
    return NULL;
}

unsigned const char *
gvl_get_id(unsigned const char *g)
{
  gvl_g*gg = gvl_validate(g);
  if (gg)
    return (uccp)gg->oid;
  else
    return NULL;
}

unsigned const char *
gvl_get_sname(unsigned const char *g)
{
  gvl_g*gg = gvl_validate(g);
  if (gg)
    return gg->sign;
  else
    return NULL;
}

int
gvl_is_sname(unsigned const char *g)
{
  gvl_g*gg = gvl_validate(g);
  if (gg)
    return gg->type && *gg->type == 's';
  else
    return 0;
}

int
gvl_looks_like_sname(unsigned const char *g)
{
  return sl_has_sign_indicator(g);
}

int
gvl_is_value(unsigned const char *g)
{
  return gvl_validate(g) != NULL;
}

static unsigned const char *
gvl_lookup_d(unsigned const char *key)
{
  return sl_lookup_d(curr_sl->u.d,key);
}

#if 0
/* NOTE: THIS ONLY WORKS WITH HASH-BASED VERSION OF GVL */
static unsigned const char *
gvl_key_of(unsigned const char *v)
{
  return hash_exists(curr_sl->u.h, v);
}
#endif

static unsigned const char *
gvl_lookup_h(unsigned const char *key)
{
  return key ? hash_find(curr_sl->u.h, (const unsigned char *)key) : NULL;
}

gvl_i*
gvl_setup(const char *project, const char *name, int arg_tsv)
{
  gvl_i *ret = NULL;
  tsv = arg_tsv;
  if (tsv)
    {
      Hash_table *h = NULL;

      use_unicode = 1;
      gvl_lookup = gvl_lookup_h;
      
      if ((h = sl_init_h(project, name)))
	ret = gvl_i_init_h(name, h);
      else
	fprintf(stderr, "gvl: failed to open TSV %s/%s\n", (char *)project, (char*)name);
    }
  else
    {
      Dbi_index *dbi = NULL;

      gvl_lookup = gvl_lookup_d;

      if ((dbi = sl_init_d(project, name)))
	ret = gvl_i_init_d(name, dbi);
      else
	fprintf(stderr, "gvl: failed to open DBI %s/%s\n", (char *)project, (char*)name);
    }

  ret->tsv = arg_tsv;
  sl_init_si();

  return ret;
}
      
void
gvl_wrapup(const char *name)
{
  gvl_i_term(name);
}

/* Don't error here; caller must decide if not finding the gvl is an error or not */
gvl_i *
gvl_i_find(const char *name)
{
  gvl_i *tmp = sl;
  while (tmp)
    {
      if (!strcmp(tmp->n, name))
	return tmp;
      else
	tmp = tmp->next;
    }
  return NULL;
}

gvl_i *
gvl_i_init_(const char *name)
{
  gvl_i *p = NULL;

  if (!name)
    return NULL;
  
  if (!(p = calloc(1, sizeof(struct gvl_i))))
    return NULL;

  p->n = name;
  
  if (!sl)
    {
      sl = p;
      sl->prev = sl->next = NULL;
    }
  else
    {
      gvl_i *tmp = NULL;
      if ((tmp = gvl_i_find(name)))
	{
	  free(p);
	  curr_sl = tmp;
	  return tmp;
	}

      p->prev = NULL;
      p->next = sl;
      sl->prev = p;
      sl = p;
    }
  curr_sl = sl;
  sl->h = hash_create(1024);
  sl->m = mb_init(sizeof(gvl_g), 1024);
  sl->p = npool_init();

  if (!sl->h || !sl->p)
    return NULL;

  return sl;
}

gvl_i *
gvl_i_init_d(const char *name, Dbi_index *dbi)
{
  gvl_i *p = NULL;
  if (dbi)
    {
      p = gvl_i_init_(name);
      if (!p->u.d)
	p->u.d = dbi;
    }
  return p;
}

gvl_i *
gvl_i_init_h(const char *name, Hash_table *h)
{
  gvl_i *p = NULL;
  if (h)
    {
      p = gvl_i_init_(name);
      if (!p->u.h)
	p->u.h = h;
    }
  return p;  
}

void
gvl_i_term(const char *name)
{
  gvl_i *tmp = sl;
  int found = 0;
  while (tmp)
    {
      if (!strcmp(tmp->n, name))
	{
	  gvl_i *next = tmp->next;
	  if (tmp->tsv)
	    hash_free(tmp->u.h, NULL);
	  else
	    sl_term_d(tmp->u.d);
	  hash_free(tmp->h, NULL);
	  npool_term(tmp->p);
	  free(tmp);
	  sl = next;
	  ++found;
	  goto ret;
	}
      else
	tmp = tmp->next;
    }
 ret:
  if (!found)
    fprintf(stderr, "gvl_i named '%s' not found during gvl_i_term\n", name);
}

unsigned char *
gvl_tmp_key(unsigned const char *key, const char *field)
{
  static char tmpkey[128];
  if (key)
    {
      strcpy(tmpkey,(ccp)key);
      if (*field)
	{
	  char *tk = tmpkey + strlen(tmpkey);
	  *tk++ = ';';
	  strcpy(tk,field);
	}
      return (ucp)tmpkey;
    }
  else
    return NULL;
}

#define QFIX (q_fixed ? (ccp)q_fixed : "")

static int
gvl_v_isupper(unsigned const char *v)
{
  static size_t s, i;
  wchar_t *wv = utf2wcs(v,&s);
  for (i = 0; i < s; ++i)
    if (iswalpha(wv[i]))
      break;
  return iswupper(wv[i]);
}

static int
gvl_try_h(gvl_g *gp, gvl_g *vp, gvl_g *qp, unsigned char *q_fixed, unsigned char **mess)
{
  int qv_bad = 1;
  unsigned char *b = gvl_val_base(vp->text);
  unsigned const char *h = gvl_lookup(gvl_tmp_key(b,"h"));
  if (h)
    {
      unsigned const char *p = NULL;
      if ((p = (uccp)strstr((ccp)h, qp->oid)))
	{
	  unsigned char *p2 = NULL, *p_end = (ucp)strchr((char*)p,' '), *p_slash = NULL, *free1 = NULL, *free2 = NULL;
	  if (p_end)
	    {
	      p2 = free2 = malloc((p_end-p) + 1);
	      strncpy((char*)p2,(char*)p,p_end-p);
	      p2[p_end-p] = '\0';
	    }
	  else
	    {
	      p2 = malloc(strlen((char*)p) + 1);
	      strcpy((char*)p2,(char*)p);
	    }
	  if ((p_slash = (ucp)strchr((ccp)p,'/')))
	    {
	      p = free1 = gvl_v_from_h((uccp)b, (uccp)p_slash+1);
	      if (!p)
		{
		  fprintf(stderr, "gvl: internal error in data: gvl_from_h failed on %s\n", p_slash);
		  p = (ucp)"(null)";
		}
	    }
	  else
	    p = b;

	  /* ? build a p(qp->sign) here and set gp->text to it ? */

	  /* If the gp->text value is uppercase, make the result value
	     uppercase; then if value == qp->sign, elide the value and
	     just print the qp-sign with no parens */
	  if (gvl_v_isupper(gp->text) && !strcmp((ccp)(p=g_uc(p)), (ccp)qp->sign))
	    *mess = gvl_vmess("%s: should be %s%s", gp->text, qp->sign, QFIX);
	  else if (strcmp((ccp)vp->text,(ccp)p) || (ccp)q_fixed)
	    *mess = gvl_vmess("%s: should be %s(%s)%s", gp->text, p, qp->sign, QFIX);

	  qv_bad = 0;
	  /*ret = 1;*/ /* ok because deterministically resolved */
	  if (free1)
	    free(free1);
	  if (free2)
	    free(free2);
	}
    }
  return qv_bad;
}
static int
gvl_q_c10e(gvl_g *gp, unsigned char **mess)
{
  gvl_g *vp = NULL, *qp = NULL;
  unsigned const char *v, *q;
  unsigned char *tmp = malloc(strlen((ccp)gp->text)+1), *end = NULL, *q_fixed = NULL;
  int pnest = 0, v_bad = 0, q_bad = 0, ret = 0;

  /* get pointers to the value (v) and qualifier (q) parts */
  strcpy((char*)tmp, (ccp)gp->text);
  end = tmp+strlen((ccp)tmp);
  --end;
  *end = '\0';
  while (end > tmp && ('(' != end[-1] || pnest))
    {
      --end;
      if (')' == *end)
	++pnest;
      else if ('(' == *end)
	--pnest;
    }
  if (end == tmp)
    {
      *mess = gvl_vmess("[vq] %s: syntax error in value-qualifier", gp->text);
      return 0;
    }
  
  q = end--;
  *end = '\0';
  v = tmp;
#if 0
  if (v_val)
    *v_val = gvl_key_of(v); /* get the stable form of v which is the key in the sl hash */
#endif
      
  /* check the value */
  vp = gvl_validate(v);
  if (vp)
    {
      if (vp->mess && !strstr((ccp)vp->mess, "must be qualified"))
	v_bad = 1;
    }
      
  /* check the sign */
  qp = gvl_validate(q);
  if (qp)
    {
      if (!qp->sign)
	q_bad = 1;
      else if (strcmp((ccp)q, (ccp)qp->sign))
	{
	  q_fixed = malloc(strlen((ccp)q) + strlen((ccp)qp->sign) + strlen(" [ <= ] "));
	  (void)sprintf((char*)q_fixed," [%s <= %s]", qp->sign, q);
	}
    }
      
  /* Now if we have bad value and qualifier it's too hard to guess */
  if (v_bad && q_bad)
    *mess = gvl_vmess("%s: both value and qualifier unknown", gp->text);
  else if (v_bad)
    {
      /* If the v is unknown, check if the base is known for q under a different index, else report known v for q */
      int qv_bad = gvl_try_h(gp, vp, qp, q_fixed, mess);
      if (qv_bad)
	{
	  unsigned const char *tmp2 = gvl_lookup(gvl_tmp_key((uccp)qp->oid, "values"));
	  if (tmp2)
	    *mess = gvl_vmess("[vq] %s: %s unknown. Known for %s: %s%s", gp->text, vp->text, qp->sign, tmp2, QFIX);
	  else
	    *mess = gvl_vmess("[vq] %s: %s unknown. No known values for %s%s", gp->text, vp->text, qp->sign, QFIX);
	}
    }
  else if (q_bad)
    {
      /* If the q is unknown, report known q for v */
      unsigned const char *tmp2 = gvl_lookup(gvl_tmp_key(vp->text, "q"));
      if (tmp2)
	*mess = gvl_vmess("[vq] %s: %s unknown: known for %s: %s%s", gp->text, q, vp->text, tmp2, QFIX);
      else
	*mess = gvl_vmess("[vq] %s: %s unknown: %s is %s%s", gp->text, q, vp->text, vp->sign, QFIX);
    }
  else
    {
      unsigned char *tmp2 = malloc(strlen((ccp)vp->text) + strlen((ccp)qp->sign) + 3);
      sprintf((char*)tmp2, "%s(%s)", vp->text, qp->sign);
	  
      /* tmp2 is now a vq with valid v and q components */
      if (gvl_lookup(gvl_tmp_key(tmp2,"qv")))
	{
	  /* vq is known combo -- we have canonicalized the g that was passed as arg1 */
	  gp->oid = qp->oid;
	  gp->sign = gvl_lookup(gvl_tmp_key((uccp)qp->oid,""));
	  /* add gp->text to g hash as key of tmp2 ? */
	  if (gvl_strict)
	    *mess = gvl_vmess("[vq] %s: should be %s%s", gp->text, tmp2, QFIX);
	  ret = 1;
	}
      else if ('s' == *vp->type || 'c' == *vp->type)
	{
	  /* This is a qualified uppercase value like TA@g(LAK654a) */
	  if (strcmp(vp->oid, qp->oid))
	    {
	      unsigned const char *parents = gvl_lookup(gvl_tmp_key((uccp)qp->oid,"parents"));
	      if (parents)
		{
		  if (!strstr((ccp)parents, vp->oid))
		    {
		      unsigned char *snames = snames_of(parents);
		      *mess = gvl_vmess("[vq] %s: bad qualifier: %s is a form of %s", gp->text, qp->sign, snames);
		      free(snames);
		    }
		}
	      else
		*mess = gvl_vmess("[vq] %s: value and qualifier are different signs (%s vs %s)",
				  gp->text, vp->sign, qp->sign);
	    }
	}
      else
	{
	  /* is vq a v that doesn't need qualifying? */
	  if (vp->oid && qp->oid && !strcmp(vp->oid, qp->oid))
	    {
	      gp->oid = qp->oid;
	      gp->sign = gvl_lookup(gvl_tmp_key((uccp)qp->oid,""));
	      if (gvl_strict)
		*mess = gvl_vmess("[vq] %s: unnecessary qualifier on value%s", gp->text, QFIX);
	      ret = 1; /* this is still OK--we have resolved the issue deterministically */
	    }
	  else
	    {
	      /* if the qv is unknown see if the value has the wrong index in the q context */
	      int qv_bad = gvl_try_h(gp, vp, qp, q_fixed, mess);
	      if (qv_bad)
		{
		  /* we know the q doesn't have a value which is correct except for the index;
		     report known q for v */
		  if ('v' == *vp->type)
		    {
		      unsigned const char *q_for_v = gvl_lookup(gvl_tmp_key(v, "q"));
		      if (q_for_v)
			*mess = gvl_vmess("[vq] %s: unknown. Known for %s: %s%s", gp->text, vp->text, q_for_v, QFIX);
		      else
			*mess = gvl_vmess("[vq] %s: %s is %s%s", gp->text, vp->text, vp->sign, QFIX);
		    }
#if 1
		  else
		    {
		      fprintf(stderr, "gvl: [vq] unhandled case for input %s\n", gp->text);
		    }
#else
		  else
		    {
		      unsigned const char *parents = gvl_lookup(gvl_tmp_key((uccp)qp->oid,"parents"));
		      if (parents)
			{
			  unsigned char *snames = snames_of(parents);
			  *mess = gvl_vmess("[vq] %s: bad qualifier: %s is a form of %s", gp->text, qp->sign, snames);
			  free(snames);
			}
		    }
#endif
		}
	      /* Dont' free(b) because it belongs to wcs2utf */ 
	    }
	}
      free(tmp2);
    }
      
  if (!vp && !qp)
    *mess = gvl_vmess("[vq] unknown validation failure%s", QFIX);
      
  /* keep tmp until end because v and q give us the original text of
     the vq--the sign may have been canonicalized by gvl_validate */
  free(tmp);
  
  return ret;
}

#define GVL_x "ₓ"

unsigned char *
gvl_val_base(const unsigned char *v)
{
  if (v)
    {
      unsigned char *b = NULL, *sub = NULL, *ret;
      
      b = malloc(strlen((ccp)v)+1);
      strcpy((char*)b, (ccp)v);
      if (strlen((ccp)v) > 4)
	{
	  sub = b + strlen((ccp)b);
	  while (1)
	    {
	      if ('\0' == *sub && sub - 3 > b && sub[-3] == 0xe2 && sub[-2] == 0x82)
		{
		  if ((sub[-1] >= 0x80 && sub[-1] <= 0x89) || sub[-1] == 0x93)
		    {
		      sub -= 3;
		      *sub = '\0';
		    }
		}
	      else
		break;
	    }
	}
      ret = g_lc(b);
      free(b);
      return ret;
    }
  return NULL;
}

const char *
gvl_sub_of(int i)
{
  switch (i)
    {
    case 1:
      return "₁";
    case 2:
      return "₂";
    case 3:
      return "₃";
    case 4:
      return "₄";
    case 5:
      return "₅";
    case 6:
      return "₆";
    case 7:
      return "₇";
    case 8:
      return "₈";
    case 9:
      return "₉";
    case 0:
      return "₀";
    }
  return NULL;
}
  
unsigned char *
gvl_v_from_h(const unsigned char *b, const unsigned char *qsub)
{
  if (b && qsub)
    {
      int qsub_i = atoi((ccp)qsub);
      if (qsub_i >= 0)
	{
	  unsigned char *ret = malloc(strlen((ccp)b)+7);
	  int tens = qsub_i / 10;
	  int unit = qsub_i % 10;
	  const char *tensp = NULL, *unitp = gvl_sub_of(unit);
	  strcpy((char *)ret, (const char *)b);
	  if (qsub_i)
	    {
	      if (tens && ((tensp = gvl_sub_of(tens))))
		strcat((char *)ret,tensp);
	      if (unitp)
		strcat((char *)ret,unitp);
	    }
	  else
	    strcat((char*)ret,"ₓ");
	  return ret;
	}
    }
  return NULL;
}

int
gvl_ignore(unsigned const char *g)
{
#if 1
  return 0;
#else
  while (*g)
    if (*g < 128 && (/*isdigit(*g) || */'/' == *g || '*' == *g))
      ++g;
    else
      break;
  return '\0' == *g || '(' == *g; /* ignore *(u) and friends */
#endif
}

gvl_g *
gvl_validate(unsigned const char *g)
{
  gvl_g *gp = NULL;
  /* unsigned const char *orig_g = g; */

  if (!use_unicode && !isdigit(g[0]) && '|' != g[0] && ':' != g[0] && '*' != g[0])
    g = g2utf(g);

  if (g && !gvl_ignore(g))
    {
      /*unsigned const char *g_orig = g;*/
      if (gvl_trace)
	fprintf(stderr, "gvl_validate: called with g=%s\n", g);
      if (!(gp = hash_find(sl->h,g)))
	{
	  unsigned const char *l = NULL;
	  unsigned char *a = NULL;
	  static int c10e_err = 0;

	  c10e_err = 0;
	  a = g_c10e(g, &c10e_err);
	  if (a)
	    {
	      if (strcmp((ccp)a,(ccp)g))
		{
		  g = npool_copy(g,sl->p);
		  if (gvl_trace)
		    fprintf(stderr, "gvl_validate: %s mapped to %s\n", g, a);
		  gvl_g *gp2 = hash_find(sl->h,a);
		  if (gp2)
		    {
		      gp2->accn = g;
		      hash_add(sl->h, g, gp2);
		      /* note: don't free a directly as it belongs to wcs2utf */
		      return gp2;
		    }
		}
	      else
		{
		  a = NULL;
		}
	    }

	  if (gvl_trace)
	    fprintf(stderr, "gvl_validate: %s not found in seen-hash\n", g);

	  gp = mb_new(sl->m);
	  if (a)
	    {
	      gp->text = npool_copy(a,sl->p);
	      gp->accn = npool_copy(g,sl->p);
	      hash_add(sl->h, gp->text, gp);
	      hash_add(sl->h, gp->accn, gp);
	      g = gp->text;
	      a = NULL;
	    }
	  else
	    {
	      gp->text = npool_copy(g, sl->p);
	      hash_add(sl->h, gp->text, gp);
	    }
	  
	  gp->type = gvl_type(g);
	  
	  if (*gp->type == 'q')
	    {
	      g = gvl_tmp_key(g,"qv");
	      if ((l = gvl_lookup(g)))
		{
		  gp->oid = (ccp)l;
		  gp->sign = gvl_lookup(gvl_tmp_key(l,""));
		}
	      else
		{
		  static unsigned char *mess = NULL;
		  (void)gvl_q_c10e(gp, &mess);
		  if (mess)
		    gp->mess = gvl_vmess("%s", mess);
		  mess = NULL;
		}
	    }
	  else if ((l = gvl_lookup(g)))
	    {
	      /* best case: g is a known sign or value */
	      gp->oid = (ccp)l;
	      gp->sign = gvl_lookup(gvl_tmp_key(l,""));
	    }
	  else if (*gp->type == 'n')
	    {
	      char *paren = strchr((ccp)g, '(');
	      if (paren)
		{
		  if ((l = gvl_lookup(g)))
		    {
		      gp->oid = (ccp)l;
		      gp->sign = gvl_lookup(gvl_tmp_key(l,""));
		    }
		  else
		    {
		      const unsigned char *lg = utf_lcase(g);
		      if ((l = gvl_lookup(lg)))
			{
			  gp->oid = (ccp)l;
			  gp->sign = gvl_lookup(gvl_tmp_key(l,""));
#if 0
			  /* allow 1(U@c) as well as 1(u@c) without a warning--is this right? */
			  if (gvl_strict)
			    gp->mess = gvl_vmess("numeric pseudo-signname %s should be %s", g, gp->sign);
#endif
			}
		      else
			{
			  gp->mess = gvl_vmess("unknown numeric sign %s", g, gp->sign);
			}
		    }
		}
	      else
		{
		  int n = 0;
		  if ((n = atoi((ccp)g)))
		    {
		      unsigned char *sx = sexify(n, "disz");
		      hash_add(sl->h, npool_copy(sx,sl->p), gp);
		      if ((l = gvl_lookup(sx)))
			{
			  gp->oid = (ccp)l;
			  gp->sign = gvl_lookup(gvl_tmp_key(l,""));
			}
		      else
			{
#if 0
			  /* TBD: if this has spaces; split and possibly use a list in gp */
			  gp->mess = gvl_vmess("unknown sexified number sign %s [< %s]", sx, g);
#endif
			}
		    }
		  else if (strchr((char*)g, '/'))
		    {
		      char *qnum = malloc(strlen((char*)g) + strlen("(diš)") + 1);
		      (void)sprintf((char*)qnum, "%s(diš)", g);
		      hash_add(sl->h, npool_copy((uccp)qnum,sl->p), gp);
		      if ((l = gvl_lookup((uccp)qnum)))
			{
			  gp->oid = (ccp)l;
			  gp->sign = gvl_lookup(gvl_tmp_key(l,""));
			}
		      else
			gp->mess = gvl_vmess("unknown sexified fraction %s [< %s]", qnum, g);
		      free(qnum);
		    }
		  else
		    gp->mess = gvl_vmess("unable to sexify non-numeric %s", g);
		}
	    }
	  else if (*gp->type == 'p')
	    {
	      if ((l = gvl_lookup(g)))
		{
		  gp->oid = (ccp)l;
		  gp->sign = gvl_lookup(gvl_tmp_key(l,""));
		}
	      else
		{
		  char *paren = strchr((ccp)g, '(');
		  if (paren)
		    {
		      unsigned char *q = malloc(strlen((ccp)paren));
		      strcpy((char*)q,paren+1);
		      if ((paren = strrchr((ccp)q,')')))
			{
			  *paren = '\0';
			  if ((l = gvl_lookup(q)))
			    {
			      gp->oid = (ccp)l;
			      gp->sign = gvl_lookup(gvl_tmp_key(l,""));
			    }
			  else
			    gp->mess = gvl_vmess("%s: punctuation with unknown sign qualifier", g);
			}
		      else
			gp->mess = gvl_vmess("%s: qualifier without closing ')'", g);
		    }
		}
	    }
	  else if (sl_has_sign_indicator(g))
	    {
	      const unsigned char *lg = utf_lcase(g);
	      if ((l = gvl_lookup(lg)))
		{
		  gp->oid = (ccp)l;
		  gp->sign = gvl_lookup(gvl_tmp_key(l,""));
		  if (gvl_strict)
		    gp->mess = gvl_vmess("pseudo-signname %s should be %s", g, gp->sign);
		}
	      else if ((l = gvl_lookup(gvl_tmp_key(lg,"q"))))
		{
		  gp->sign = l;
		  if (gvl_strict)
		    gp->mess = gvl_vmess("pseudo-signname %s must be qualifed by one of %s",g,l);
		}
	      else
		{
		  if ('|' == *g)
		    {
		      unsigned char *c10e = c10e_compound(g);
		      if (c10e)
			{
			  if ((l=gvl_lookup(c10e)))
			    {
			      gp->oid = (ccp)l;
			      gp->sign = gvl_lookup(gvl_tmp_key(l,""));
			      if (gvl_strict)
				gp->mess = gvl_vmess("compound %s should be %s", g, c10e);
			    }
			  else
			    {
			      unsigned char *c10e_no_p = sl_strip_pp(c10e);
			      if (c10e_no_p && (l=gvl_lookup(c10e_no_p)))
				{
				  gp->oid = (ccp)l;
				  gp->sign = gvl_lookup(gvl_tmp_key(l,""));
				  if (gvl_strict)
				    gp->mess = gvl_vmess("compound %s should be %s", g, c10e_no_p);
				}
			      else
				{
				  if (strcmp((ccp)g, (ccp)c10e) && c10e_no_p && strcmp((ccp)c10e, (ccp)c10e_no_p))
				    gp->mess = gvl_vmess("unknown compound: %s (also tried %s/%s)", g, c10e, c10e_no_p);
				  else if (strcmp((ccp)g, (ccp)c10e))
				    gp->mess = gvl_vmess("unknown compound: %s (also tried %s)", g, c10e);
				  else if (c10e_no_p && strcmp((ccp)g, (ccp)c10e_no_p))
				    gp->mess = gvl_vmess("unknown compound: %s (also tried %s)", g, c10e_no_p);
				  else
				    gp->mess = gvl_vmess("unknown compound: %s", g);
				}
			      free(c10e_no_p);
			    }
			}
		    }
		  else if ((l = gvl_lookup(gvl_tmp_key(g,"l"))))
		    {
		      gp->type = "l";
		      gp->oid = (ccp)l;
		      gp->sign = gvl_lookup(gvl_tmp_key(l,""));
		    }
		  else
		    gp->mess = gvl_vmess("unknown sign name: %s", g);
		}
	    }
	  else
	    {
	      const unsigned char *gq = gvl_lookup(gvl_tmp_key(g,"q"));
	      if (gq)
		gp->mess = gvl_vmess("value %s must be qualified with one of %s", g, gq);
	      else
		gp->mess = gvl_vmess("unknown value: %s. To request adding it please visit:\n\t%s", g, report);
	    }
	}
      else
	{
	  if (gvl_trace)
	    fprintf(stderr, "gvl_validate: %s was found in hash\n", g);
	}
    }
  
  return gp;
}
