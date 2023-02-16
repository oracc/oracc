/* Grapheme Validation Library */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

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
  else if (*g == '*' || *g == ':')
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
  return has_sign_indicator(g);
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
  while ('(' != end[-1] || pnest)
    {
      --end;
      if (')' == *end)
	++pnest;
      else if ('(' == *end)
	--pnest;
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

#define QFIX (q_fixed ? (ccp)q_fixed : "")
  
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
    *mess = (ucp)"both value and qualifier are unknown--I can't do anything with that";
  else if (v_bad)
    {
      /* If the v is unknown, report known v for q */
      unsigned const char *tmp2 = gvl_lookup(gvl_tmp_key((uccp)qp->oid, "values"));
      if (tmp2)
	*mess = gvl_vmess("unknown value %s: known values of %s are: %s%s", vp->text, qp->sign, tmp2, QFIX);
      else
	*mess = gvl_vmess("unknown value %s: there are no known values for %s%s", vp->text, qp->sign, QFIX);
    }
  else if (q_bad)
    {
      /* If the q is unknown, report known q for v */
      unsigned const char *tmp2 = gvl_lookup(gvl_tmp_key(vp->text, "q"));
      if (tmp2)
	*mess = gvl_vmess("unknown sign %s: known qualifiers of %s are: %s%s", qp->sign, vp->text, tmp2, QFIX);
      else
	*mess = gvl_vmess("unknown sign %s: %s only known as value of %s%s", qp->sign, vp->text, vp->sign, QFIX);
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
	    *mess = gvl_vmess("qualified value %s should be %s%s", gp->text, tmp2, QFIX);
	  ret = 1;
	}
      else
	{
	  /* is vq a v that doesn't need qualifying? */
	  if (!strcmp(vp->oid, qp->oid))
	    {
	      gp->oid = qp->oid;
	      gp->sign = gvl_lookup(gvl_tmp_key((uccp)qp->oid,""));
	      if (gvl_strict)
		*mess = gvl_vmess("unnecessary qualifier on value: %s%s", gp->text, QFIX);
	      ret = 1; /* this is still OK--we have resolved the issue deterministically */
	    }
	  else
	    {
	      /* if the qv is unknown see if the value has the wrong index in the q context */
	      int qv_bad = 1;
	      unsigned char *b = gvl_val_base(vp->text);
	      unsigned const char *h = gvl_lookup(gvl_tmp_key(b,"h"));
	      if (h)
		{
		  unsigned const char *p = NULL;
		  if ((p = (uccp)strstr((ccp)h, qp->oid)))
		    {
		      if ((p = (uccp)strchr((ccp)p,'/')))
			{
			  ++p;
			  if ((p = gvl_v_from_h((uccp)b, (uccp)p)))
			    {
			      /* build a p(qp->sign) here and set gp->text to it ? */
			      *mess = gvl_vmess("qualified value %s should be %s(%s)%s", gp->text, p, qp->sign, QFIX);
			      free((void*)p);
			      qv_bad = 0;
			      ret = 1; /* ok because deterministically resolved */
			    }
			}
		    }
		}
	      if (qv_bad)
		{
		  /* we know the q doesn't have a value which is correct except for the index;
		     report known q for v */
		  unsigned const char *q_for_v = gvl_lookup(gvl_tmp_key(v, "q"));
		  if (q_for_v)
		    *mess = gvl_vmess("%s can't be qualified by %s--maybe %s ?%s", vp->text, qp->sign, q_for_v, QFIX);
		  else
		    *mess = gvl_vmess("%s only known as value of %s%s", vp->text, vp->sign, QFIX);
		}
	      free(b);
	    }
	}
      free(tmp2);
    }

  if (!vp && !qp)
    *mess = gvl_vmess("unknown validation failure%s", QFIX);

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
      unsigned char *b = NULL, *sub = NULL;
      
      b = malloc(strlen((ccp)v));
      strcpy((char*)b, (ccp)v);
      if (strlen((ccp)v) > 4)
	{
	  sub = b + strlen((ccp)b);
	  while (1)
	    {
	      sub -= 3;
	      if (*sub == 0xe2 && sub[1] == 0x82)
		{
		  switch (sub[2])
		    {
		    case 0x80:
		    case 0x81:
		    case 0x82:
		    case 0x83:
		    case 0x84:
		    case 0x85:
		    case 0x86:
		    case 0x87:
		    case 0x88:
		    case 0x89:
		    case 0x93:
		      *sub = '\0';
		      if (sub - 3 > b)
			{
			  sub -= 3;
			  continue;
			}
		      break;
		    }
		}
	      break;
	    }
	}
      return b;
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
      return "ₓ";
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
	  if (tens)
	    tensp = gvl_sub_of(tens);
	  strcpy((char *)ret, (const char *)b);
	  if (tensp)
	    strcat((char *)ret,tensp);
	  if (unitp)
	    strcat((char *)ret,unitp);
	  return ret;
	}
    }
  return NULL;
}

int
gvl_ignore(unsigned const char *g)
{
  while (*g)
    if (*g < 128 && (/*isdigit(*g) || */'/' == *g || '*' == *g))
      ++g;
    else
      break;
  return '\0' == *g || '(' == *g; /* ignore *(u) and friends */
}

gvl_g *
gvl_validate(unsigned const char *g)
{
  gvl_g *gp = NULL;

  if (g && !gvl_ignore(g))
    {
      unsigned const char *g_orig = g;
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
		  if (!gvl_q_c10e(gp, &mess))
		    gp->mess = gvl_vmess("vq error in %s: %s", g_orig, mess);
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
		      /* do nothing; let the validation happen when n
			 is parsed into n and r components */
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
			  /* TBD: if this has spaces split and possibly use a list in gp */
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
	  else
	    {
	      int g_is_sn = has_sign_indicator(g);
	      if (g_is_sn)
		{
		  const unsigned char *lg = utf_lcase(g);
		  if ((l = gvl_lookup(lg)))
		    {
		      gp->oid = (ccp)l;
		      gp->sign = gvl_lookup(gvl_tmp_key(l,""));
		      if (gvl_strict)
			gp->mess = gvl_vmess("pseudo-signname %s should be %s", g, gp->sign);
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
			      else if (strcmp((ccp)g, (ccp)c10e))
				gp->mess = gvl_vmess("unknown compound: %s (also tried %s)", g, c10e);
			      else
				gp->mess = gvl_vmess("unknown compound: %s", g);
			    }
			}
		      else if ((l = gvl_lookup(gvl_tmp_key(g,"l"))))
			{
			  gp->type = "l";
			  gp->oid = (ccp)l;
			  gp->sign = gvl_lookup(gvl_tmp_key(l,""));
			}
		      else
			{
			  gp->mess = gvl_vmess("unknown sign name: %s", g);
			}
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
	}
      else
	{
	  if (gvl_trace)
	    fprintf(stderr, "gvl_validate: %s was found in hash\n", g);
	}
    }
  
  return gp;
}
