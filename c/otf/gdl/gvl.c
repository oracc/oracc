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

#include "gvl.h"

static gvl_i *sl = NULL; /* sl is always the head of the list of signlist datasets, not necessarily the current one */
static gvl_i *curr_sl = NULL; /* the sl that should be used for look up */
static int tsv = 0;
static int gvl_trace = 0;

static const char *report = "http://oracc.museum.upenn.edu/ogsl/reportingnewvalues/";

const unsigned char *(*gvl_lookup)(unsigned const char *key);

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

static unsigned const char *
gvl_lookup_h(unsigned const char *key)
{
  return hash_find(curr_sl->u.h, (const unsigned char *)key);
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
  strcpy(tmpkey,(ccp)key);
  if (*field)
    {
      char *tk = tmpkey + strlen(tmpkey);
      *tk++ = ';';
      strcpy(tk,field);
    }
  return (ucp)tmpkey;
}

static unsigned char *
gvl_q_c10e(unsigned const char *g, unsigned const char **mess,
	   const char **v_oid, const char **q_oid)
{
  gvl_g *gp = NULL;
  unsigned const char *v, *q;
  unsigned char *tmp = malloc(strlen((ccp)g)+1), *end;
  int pnest = 0;

  strcpy((char*)tmp, (ccp)g);
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

  /* is the value OK */
  gp = gvl_validate(v);
  if (gp)
    {
      if (gp->mess && !strstr((ccp)gp->mess, "must be qualified"))
	{
	  *mess = gp->mess;
	  free(tmp);
	  return NULL;
	}
      else if (gp->oid)
	*v_oid = gp->oid;
      else
	*v_oid = ""; /* this happens if a value must be qualified--which it is if we are in this routine */
    }
  else
    {
      *mess = (uccp)"unknown validation failure";
      free(tmp);
    }

  /* is the sign OK or correctable? */
  gp = gvl_validate(q);
  if (gp)
    {
      if (gp->sign)
	{
	  unsigned char *tmp2 = malloc(strlen((ccp)v) + strlen((ccp)gp->sign) + 3);
	  sprintf((char*)tmp2, "%s(%s)", v, gp->sign);
	  *q_oid = gp->oid;
	  free(tmp);
	  return tmp2;
	}
      else
	{
	  *mess = gp->mess;
	  free(tmp);
	  return NULL;
	}
    }
  else
    {
      *mess = (uccp)"unknown validation failure";
      free(tmp);
    }
  return NULL;
}

gvl_g *
gvl_validate(unsigned const char *g)
{
  gvl_g *gp = NULL;

  if (g)
    {
      unsigned const char *g_orig = g;
      if (gvl_trace)
	fprintf(stderr, "gvl_validate: called with g=%s\n", g);
      if (!(gp = hash_find(sl->h,g)))
	{
	  unsigned const char *l = NULL;
	  if (gvl_trace)
	    fprintf(stderr, "gvl_validate: %s not found in seen-hash\n", g);

	  gp = mb_new(sl->m);
	  gp->text = npool_copy(g, sl->p);
	  hash_add(sl->h, gp->text, gp);
	  
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
		  static unsigned const char *mess = NULL;
		  static const char *v_oid = NULL, *q_oid = NULL;
		  unsigned char *q_c10e = gvl_q_c10e(g_orig, &mess, &v_oid, &q_oid);
		  if (q_c10e)
		    {
		      g = gvl_tmp_key(q_c10e,"qv");
		      if ((l = gvl_lookup(g)))
			{
			  gp->oid = (ccp)l;
			  gp->sign = gvl_lookup(gvl_tmp_key(l,""));
			  gp->mess = gvl_vmess("qualified value %s should be %s", g_orig, q_c10e);
			}
		      else
			{
			  if (!strcmp(v_oid, q_oid))
			    {
			      gp->oid = q_oid;
			      gp->sign = gvl_lookup(gvl_tmp_key((uccp)q_oid,""));
			      gp->mess = gvl_vmess("unnecessary qualifier on value: %s", g_orig);
			    }
			  else if (strcmp((ccp)g_orig, (ccp)q_c10e))
			    gp->mess = gvl_vmess("unknown qualified value: %s (also tried %s)", g_orig, q_c10e);
			  else
			    {
			      if (v_oid && q_oid)
				{
				  unsigned const char *h = gvl_lookup(gvl_tmp_key(gvl_v_base(v_oidXXX),"h"));
				  unsigned const char *p = NULL;
				  if ((p = strstr(h, q_oid)))
				    {
				      p = strchr(p,'/');
				      ++p;
				      p = h_decode(p);
				      gp->mess = gvl_vmess("
				    }
				}
			      else
				gp->mess = gvl_vmess("unknown qualified value: %s", g_orig);
			    }
			}
		      free(q_c10e);
		    }
		  else
		    {
		      gp->mess = gvl_vmess("error in qualified value %s: %s", g_orig, mess);
		    }
		}
	    }
	  else if ((l = gvl_lookup(g)))
	    {
	      /* best case: g is a known sign or value */
	      gp->oid = (ccp)l;
	      gp->sign = gvl_lookup(gvl_tmp_key(l,""));
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
