#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype128.h>
#include <wctype.h>
#include <psdtypes.h>
#include <assert.h>
#include <hash.h>
#include <cdf.h>
#include "npool.h"
#include "tree.h"
#include "warning.h"
#include "atf.h"
#include "sexify.h"
#include "charsets.h"
#include "gdl.h"
#include "gsl.h"

static int rg_verbose = 0;
extern int math_mode, saa_mode;
extern int cuneify_fuzzy_allographs;
extern int gdl_bootstrap;
int do_signnames = 0;
int backslash_is_formvar = 1;
static struct npool *graphemes_pool;
struct node *pending_disamb = NULL;

#define pool_copy(x) npool_copy((x),graphemes_pool)

#define insertp_is_delim() (insertp > startp && (insertp[-1] == '.' || insertp[-1] == '-'))

#include "xvalue.c"

/* #define TEST */

#define mbincr(p) (p += mbtowc(NULL,(const char *)p,6))

#define NEW_ERROR_RECOVERY

/* This module is not allowed to use the non-g-prefixed forms of the
 tree building routines; we define them to trigger errors to keep us
 honest */
#define elem     #error
#define textNode #error
#define textElem #error
#define cdata    #error

int cbd_rules = 0;
int compound_warnings = 0;
int mixed_case_ok = 0;
int qualifier_warnings = 0;
int rendering_word_form = 0;
int use_legacy = 0;
int use_unicode = 0;
extern int in_split_word;

/* if non-zero the gparse was called from cparse, qualified, etc. */
static int inner_parse = 0, inner_qual = 0;

static int is_bad_cg[256];
static const char *bad_cg_chars = "[]<>{}#!?*";

static int is_grapheme_base[256];
static const char *grapheme_base_chars
  = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789',";

static int is_compound_base[256];
static const char *compound_base_chars
  = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789'";

#define C(x) #x,

static const char * const op_names[] = { OPS };
static const char op_chars[] = ".+xx&%@:";
static struct node op_nodes[o_top];
static struct node *ops_by_char[128];

static struct attr *brokenattr;
static struct attr *hashattr;
static struct attr *queryattr;
static struct attr *bangattr;
static struct attr *collatedattr;
static struct attr *supplattr;
static struct attr *excisattr;
static struct attr *implattr;
static struct attr *okattr;

static int in_square = 0;
static int in_presence = 0;
const char *curr_presence = NULL;
char cued_opener[128];

extern FILE*f_graphemes;

#define MODS_MAX 128
static enum e_type gtags[type_top];

#if 0
extern const char *isvalue(register const char *str,register unsigned int len);
#endif

static struct node *build_singleton(unsigned const char *base, enum t_type type, 
				    int nmods, struct mods *mods);
static int gmods(register unsigned char *g, struct mods *modsbuf);
static struct grapheme *compound(register unsigned char *g);
static int cparse(struct node *parent, unsigned char *g, const char end,
		  unsigned char **endptr);
static struct grapheme *numerical(register unsigned char *g);
static void process_mods(struct node*e,int nmods, struct mods *mods);
static struct grapheme *punct(register unsigned char *g);
static struct grapheme *qualified(register unsigned char *g);
static struct grapheme *singleton(register unsigned char *g, enum t_type type);
static struct grapheme *icmt_grapheme(const unsigned char *icmt);
static const unsigned char *signify(const unsigned char *utf);

void
g_reinit()
{
  in_square = 0;
}

static const unsigned char *
g2utf(const unsigned char *g)
{
  unsigned const char *g_dig = NULL, *g_end = NULL;
  const unsigned char *utf8g = NULL;

  /* mods and alts are not converted */
  g_end = g;
  while (*g_end && *g_end != '~' && *g_end != '\\'
	 && (*g_end != '@' || g_end[1] > 0x80 || isupper(g_end[1])))
    ++g_end;

  if (*g_end)
    g_dig = g_end;
  else
    g_dig = g_end = g+strlen((const char *)g);

  /* subdigs are converted separately */
  while (g_dig > g && (isdigit(g_dig[-1]) 
		       || ('x' == g_dig[-1] && (g_dig-1 > g))))
    --g_dig;
  utf8g = natf2utf((const char*)g,(const char *)g_dig,0,file,lnum);

   if (utf8g && *g_dig)
    {
      if (!is_signlist(utf8g))
	utf8g = subdig(g_dig,g_end);
      else
	utf8g = cpydig(g_dig,g_end);
    }

  return pool_copy(utf8g);
}

static int
sub23(const unsigned char *s)
{
  unsigned const char *last = s+strlen(cc(s))-1;
  if (*last == 0x82 || *last == 0x83)
    {
      if (last[-1] == 0x82 && last[-2] == 0xe2)
	{
	  if (strlen(cc(s)) < 7)
	    return 1;
	  if (last[-3] < 0x80 || last[-3] > 0x89 || last[-4] != 0x82 || last[-5] != 0xe2)
	    return 1;
	}
    }
  return 0;
}  

static struct node *
build_singleton(unsigned const char *base, enum t_type type, 
		int nmods, struct mods *mods)
{
  struct node *e;
  
  e = gelem(gtags[type],NULL,lnum,GRAPHEME);
  if (sub23(base))
    gsetAttr(e,a_g_accented,numacc(base));
  if (!nmods)
    appendChild(e,gtextNode(base));
  else
    {
      struct node *b = gtextElem(e_g_b,NULL,lnum,GRAPHEME,base);
      appendChild(e,b);
      process_mods(e,nmods,mods);
    }
  return e;
}

static void
process_mods(struct node*e,int nmods, struct mods *mods)
{
  int i;
  for (i = 0; i < nmods; ++i)
    {
      if (mods[i].type == g_disamb)
	{
	  /* \abs as disambiguator must be grapheme-final and
	     word-final so it's safe to make those assumptions */
	  struct node *m = gelem(e_g_x,NULL,lnum,GRAPHEME);
	  struct node *t = gtextNode(ucc(mods[i].data));
	  appendAttr(m,attr(a_g_type,ucc("disamb")));
	  appendChild(m,t);
	  /* If the g_disamb is the only mod, remove the g:b
	     wrapper from the grapheme */
	  if (nmods == 1)
	    {
	      struct node *gb = removeLastChild(e);
	      appendChild(e,removeLastChild(gb));
	    }
	  pending_disamb = m;
	}
      else
	{
	  struct node *m = gelem(gtags[mods[i].type],NULL,lnum,GRAPHEME);
	  struct node *t = gtextNode(ucc(mods[i].data));
	  appendChild(m,t);
	  appendChild(e,m);
	}
    }
}

static int
gmods(register unsigned char *g, struct mods *modsbuf)
{
  struct mods *mp = modsbuf;
  char *datap = NULL;

  while (*g)
    {
      if (mp - modsbuf >= MODS_MAX)
	{
	  fprintf(stderr,"too many modifiers\n");
	  exit(2);
	}
      switch (*g)
	{
	case '@':
	  mp->type = g_m;
	  *g++ = '\0';
	  datap = mp->data;
	  if (isdigit(*g))
	    {
	      while (isdigit(*g))
		{
		  if (datap - mp->data == MAX_MOD_DATA)
		    {
		      warning("@ modifier too long");
		      return -1;
		    }
		  else
		    *datap++ = *g++;
		}
	    }
	  else
	    {
	      if (islower(*g))
		*datap++ = *g++;
	      else
		{
		  vwarning("%s: bad character in modifier",g);
		  return -1;
		}
	    }
	  *datap = '\0';
	  ++mp;
	  break;
	case '~':
	  *g++ = '\0';
	  mp->type = g_a;
	  datap = mp->data;
	  if ('-' == *g || '+' == *g)
	    {
	      *datap++ = *g++;
	    }
	  else
	    {
	      while (*g < 128 && isalnum(*g) && 'x' != *g)
		{
		  /* FIXME: make this open-ended */
		  if (datap - mp->data == MODS_MAX)
		    {
		      vwarning("allograph too long (max %d characters)",MODS_MAX);
		      return 1;
		    }
		  *datap++ = *g++;
		}
	    }
	  if (datap == mp->data)
	    {
	      warning("empty allograph");
	      return -1;
	    }
	  *datap = '\0';
	  ++mp;
	  break;
	case '\\':
	  datap = mp->data;
	  if (backslash_is_formvar)
	    {
	      mp->type =  g_f;
	      *g++ = '\0';
	    }
	  else
	    {
	      mp->type = g_disamb;
	      *g++ = '\0';
	      *datap++ = '\\';
	    }
	  while (*g < 128 && (isalnum(*g) || '\\' == *g))
	    {
	      /* FIXME: make this open-ended */
	      if (datap - mp->data == MODS_MAX)
		{
		  vwarning("formvar too long (max %d characters)",MODS_MAX);
		  return 1;
		}
	      *datap++ = *g++;
	    }
	  if (datap == mp->data)
	    {
	      warning("empty formvar");
	      return -1;
	    }
	  *datap = '\0';
	  ++mp;
	  break;
	default:
	  {
	    unsigned char buf[5];
	    unsigned char *b = buf;
	    unsigned char *g2 = g;
	    while (*g2 && b - buf < 3)
	      *b++ = *g2++;
	    *b = '\0';
	    if (is_flag[*buf])
	      vwarning("misplaced flag at %s", buf);
	    else
	      vwarning("bad character in grapheme at %s", buf);
	    return -1;
	  }
	}
    }
  return mp-modsbuf;
}

/* FIXME: no memory management done for this yet */
static unsigned char *
gclean(unsigned char *g)
{
  unsigned char *b = malloc(strlen((char *)g) + 1);
  
  while (*g)
    if (!is_bad_cg[*g])
      *b++ = *g++;
    else
      ++g;
  return b;
}

static int
is_mixed(const unsigned char *g)
{
  if (*g > 127)
    {
      size_t len = 0;
      wchar_t c1 = 0;
      c1 = utf1char(g,&len);
      if (c1 && len > 0)
	{
	  if (iswupper(c1))
	    {
	      g += len;
	      c1 = utf1char(g,&len);
	      if (c1)
		return iswlower(c1);
	    }
	}
    }
  else
    {
      if (u_isupper(g))
	{
	  if ((g[0] == 'S' && (g[1] == 'Z' || g[1] == ','))
	      || (g[0] == 'S' && g[1] == ','))
	    g += 2;
	  else
	    mbincr(g);
	  return u_islower(g);
	}
    }
  return 0;
}

static char *
sname_to_check(const unsigned char *sn)
{
  if (use_unicode)
    return (char*)utf_lcase(sn);
  else
    {
      const unsigned char *utf8g = g2utf(sn);
      if (utf8g)
	return (char *)utf_lcase(utf8g);
      else
	return (char *)utf_lcase(sn);
    }
}

/* FIXME: this is very weak--we should handle subx.sub1 etc. */
static int 
is_xvalue(unsigned const char *s)
{
#define SUB_X_STR  "ₓ"
  if (strlen((char*)s) > (strlen(SUB_X_STR)+1))
    return !memcmp(s+strlen((char*)s)-strlen(SUB_X_STR),SUB_X_STR,strlen(SUB_X_STR));
  else
    return 0;
}

static int
cuneifiable(struct lang_context *lang)
{
  if (lang && lang->core 
      && BIT_ISSET(lang->core->features,LF_SAC)
      && (!lang->script || strcmp(lang->script, "949"))) /* This is not quite good enough; need to check for non-SAC */
    return 1;
  else
    return 0;
}

static const unsigned char *
unheth(const unsigned char *g)
{
  wchar_t *w = NULL;
  size_t len = 0;
  w = utf2wcs(g, &len);
  if (w)
    {
      wchar_t *wp = w;
      int found_heth = 0;
      while (*wp)
	{
	  if (*wp == U_heth)
	    {
	      *wp++ = 'h';
	      ++found_heth;
	    }
	  else if (*wp == U_HETH)
	    {
	      *wp++ = 'H';
	      ++found_heth;
	    }
	  else
	    ++wp;
	}
      if (found_heth)
	return pool_copy(wcs2utf(w,len));
    }
  return NULL;
}

struct grapheme*
gparse(register unsigned char *g, enum t_type type)
{
  struct grapheme *gp = NULL;
  int bad_grapheme = 0;
  unsigned char *orig = pool_copy(g);
  
  if (type == type_top)
    type = gtype(g);

  switch (type)
    {
    case g_v:
      {
	const unsigned char *gcheck = g;
	
	if (cbd_rules && strchr(cc(g),'*'))
	  {
	    static unsigned char tmpbuf[128], *ptmp = tmpbuf;
	    while (*gcheck)
	      if (*gcheck == '*')
		++gcheck;
	      else
		*ptmp++ = *gcheck++;
	    *ptmp = '\0';
	    gcheck = tmpbuf;
	  }
	if (use_unicode || use_legacy)
	  {
	    const unsigned char *a2n = accnum(gcheck);
	    if (a2n)
	      gcheck = g = (unsigned char *)strdup((const char *)a2n);
	  }
	if (!use_unicode)
	  gcheck = g2utf(gcheck);
	if (is_xvalue(gcheck))
	  {
	    if (!inner_qual)
	      vwarning("%s: x-values must be qualified with sign name", gcheck);
	    gp = singleton(g,type);
	  }
	else if (curr_lang->signlist 
		 && '#' == *curr_lang->signlist && !psl_is_value(gcheck))
	  {
	    if (!xstrcmp(g,"vacat"))
	      {
		notice("'vacat' must be given as an inline comment: ($vacat$)");
		return icmt_grapheme(g);
	      }
	    else
	      {
		int ok = 0;
		const unsigned char *noheth = NULL, *nodots = NULL;
		if (use_legacy)
		  {
		    noheth = unheth(g);
		    if (noheth && psl_is_value(noheth))
		      ok = 1;
		  }
		if (!ok)
		  {
		    /* Check if the sign has cdot or degree, as occurs
		       in Sumerian bases; by doing this as a fault trap
		       we avoid adding overhead to cuneification */
		    if ((nodots = atf_strip_base_chars(noheth ? noheth : g)))
		      {
			if (strcmp((char*)(noheth ? noheth : g),(char*)nodots))
			  {
			    if (psl_is_value(nodots))
			      ok = 1;
			    else
			      nodots = NULL;
			  }
			else
			  nodots = NULL;
		      }
		  }
		if (!ok && (!mixed_case_ok || !is_mixed(g)))
		  {
		    if (!ok)
		      {
			const char *report = "http://oracc.museum.upenn.edu/ogsl/reportingnewvalues/";
			vwarning("%s: unknown grapheme. To request adding it please visit:\n\t%s",g,report);
#ifdef NEW_ERROR_RECOVERY
			exit_status = 1;
			--status;
			gp = singleton(g,g_v);
			if (gp)
			  gp->gflags |= GFLAGS_BAD;
#endif
		      }
		  }
		else
		  {
		    if (noheth || nodots)
		      {
			gp = singleton((unsigned char *)strdup((char*)(noheth ? noheth : nodots)),
				       g_v);
			gp->raw = (unsigned char *)strdup((char*)g);
			if (noheth)
			  gp->gflags |= GFLAGS_HETH;
			else
			  gp->gflags |= GFLAGS_DOTS;
		      }
		    else
		      {
			gp = singleton(g,g_v);
		      }
		  }
	      }
	  }
	else if (curr_lang->values
		 && !hash_find(curr_lang->values,g))
	  {
	    if ((*g != 'x' || g[1])
		&& ((*g != 'n' && *g != 'N') || (g[1] && g[1] != '(')))
	      {
		vwarning("%s: grapheme value not in %s",
			 g,curr_lang->signlist);
		exit_status = 1;
		--status;
	      }
	    gp = singleton(g,g_v);
	  }
	else
	  {
	    gp = singleton(g,type);
	  }
      }
      break;
    case g_n:
      gp = numerical(g);
      break;
    case g_s:
      if (is_signlist(g))
	{
	  gp = singleton(g,type); /* FIXME?: should we preserve the info that
				     this is a signlist sign name */
	}
      else if (curr_lang->snames)
	{
	  if (is_xvalue(g) && !inner_qual)
	    vwarning("%s: x-values must be qualified with sign name", g);

	  if (!hash_find(curr_lang->snames,g))
	    {
	      const unsigned char *utf8g = g2utf(g), *lc;
	      if (utf8g)
		{
		  lc = utf_lcase(utf8g);
		  if (lc)
		    lc = utf2atf(lc);
		  else
		    lc = g;
		}
	      else
		lc = g;
	      if (hash_find(curr_lang->values,lc))
		{
		  /* we're OK, this is an upcased 
		     project value used as a name */
		}
	      else if (psl_get_sname(lc))
		{
		  vwarning("%s: sign is in OGSL but not in %s",
			   g, curr_lang->signlist);
		}
	      else
		{
		  vwarning("%s: grapheme name not in %s",
			   g,curr_lang->signlist);
		  --status;
		  exit_status = 1;
		}
	    }
	  gp = singleton(g,type);
	}
      else
	{
	  unsigned char *gcheck = g, *g_end,*g_utf;
	  const unsigned char *noheth = NULL;
	  int len = 0;

	  if (is_xvalue(g) && !inner_qual)
	    vwarning("%s: x-values must be qualified with sign name", g);

	  if (use_unicode)
	    {
	      gcheck = accnum(gcheck);
	      if (gcheck)
		g = (unsigned char *)strdup((const char *)gcheck); /* very messy */
	      else
		gcheck = g;
	    }
	  else
	    gcheck = (unsigned char *)g2utf(gcheck);
	  g_utf = gcheck;
	  gcheck = (unsigned char *)sname_to_check(gcheck);
	  if (gcheck)
	    {
	      len = strlen((const char *)gcheck);
	      g_end = &gcheck[len];
	    }
	  else
	    {
	      gcheck = g_utf;
	      g_end = g_utf+strlen((char*)g_utf);
	    }
	  while (is_flag[((unsigned char *)g_end)[-1]])
	    --g_end;
	  *g_end = '\0';
	  if (curr_lang->signlist
	      && '#' == *curr_lang->signlist
	      && !gdl_bootstrap
	      && !psl_is_sname(g_utf)
	      && !psl_is_value(gcheck)
	      && gcheck[len-1] != 'x')
	    {
	      int ok = 0;
	      if (use_legacy)
		{
		  noheth = unheth(g);
		  if (noheth)
		    {
		      if (psl_is_sname(noheth))
			ok = 1;
		      else
			{
			  const unsigned char *lc;
			  noheth = pool_copy(noheth);
			  lc = utf_lcase(noheth);
			  ok = (lc && psl_is_value(lc));
			}
		    }
		}
	      if (!ok)
		{
		  vwarning("%s: unknown sign-name grapheme",g);
		  exit_status = 1;
		  --status;
		  bad_grapheme = 1;
		}
	    }
	  if (noheth)
	    {
	      gp = singleton((unsigned char *)noheth,g_s);
	      gp->raw = (unsigned char *)strdup((char*)g);
	      gp->gflags |= GFLAGS_HETH;
	    }
	  else
	    {
	      gp = singleton(g,g_s);
	    }
	  if (bad_grapheme)
	    gp->gflags = GFLAGS_BAD;
	}
      break;
    case g_c:
      gp = compound(g);
      break;
    case g_q:
      gp = qualified(g);
      break;
    case g_p:
      gp = punct(g);
      break;
    default:
      /* shouldn't be passing non-grapheme types here */
      vwarning("malformed grapheme at %s",g);
      break;
    }
  if (gp)
    {
      if (gp->type == g_q)
	{
	  unsigned const char *value = gp->g.q.g->g.s.base;
	  unsigned const char *qual = getAttr(gp->g.q.q->xml,"form");
	  unsigned const char *vname = psl_get_sname(value);
	  
	  if (!*qual)
	    qual = gp->g.q.q->g.s.base;
	  
	  if (vname && qual)
	    {
	      if (strstr((const char*)vname,(const char*)psl_bounded_sname(qual)))
		{
		  if (!x_value(value))
		    {
		      if (qualifier_warnings)
			vnotice("value %s has redundant qualifier %s",value,qual);
		      gp->gflags |= GFLAGS_REDUNDANT;
		      gp->xml->user = (void*)1;
		    }
		}
	      else
		{
		  if (qualifier_warnings)
		    vnotice("value %s is qualified with %s but expected %s",
			    value,qual,vname);
		}
	    }
#if 0 /* this is redundant because of earlier sign name checks */
	  else
	    {
	      if (qualifier_warnings)
		vwarning("qualified value %s is not in OGSL",value);
	      vname = psl_is_sname(qual);
	      if (!vname && qualifier_warnings)
		vwarning("qualifier %s is not a sign-name in OGSL",qual);
	    }
#endif
	}
      gp->atf = orig;
      if (gp->xml && (gp->gflags & GFLAGS_DOTS))
	    gp->xml->grapheme = gp; /* give rendering process access to parent grapheme not just struct node */
      if (gp->xml && gp->xml->children.lastused > 0
	  && !strcmp(((struct node*)(gp->xml->children.nodes[0]))->type,"e")) 
	{
	  static unsigned char buf[1024];
	  unsigned char *insertp = buf;
	  unsigned char *cleang = orig;
	  if (strpbrk((const char *)orig,bad_cg_chars))
	    cleang = gclean(cleang);

	  if (curr_lang->values && !hash_find(curr_lang->values,cleang))
	    {
	      if (!curr_lang->snames || !hash_find(curr_lang->snames,cleang))
		{
		  if ((*cleang != 'x' || cleang[1])
		      && ((*cleang != 'n' && *cleang != 'N') 
			  || (cleang[1] && cleang[1] != '(')))
		    vwarning("%s: grapheme not in %s",
			     cleang,curr_lang->signlist);
		}
	    }
	  insertp = render_g(gp->xml, insertp, buf);
	  *insertp = '\0';
	  if (*buf)
	    {
	      if (gp->type == g_c && compound_warnings)
		{
		  if ('#' == *curr_lang->signlist)
		    {
		      if (!psl_is_sname(buf))
			vwarning("%s: compound not in OGSL",buf);
		    }
		  else
		    {
		      if (!hash_find(curr_lang->snames,buf))
			vwarning("%s: compound not in %s",
				 buf,curr_lang->signlist);
		    }
		}

	      appendAttr(gp->xml,gattr(a_form,buf));
	      if (do_cuneify && cuneifiable(curr_lang))
		{
		  static const unsigned char *cattr = NULL;
		  if (gp->type == g_q)
		    cattr = cuneify(gp->g.q.q->g.s.base);
		  else
		    cattr = cuneify(buf);
		  if (cattr)
		    appendAttr(gp->xml,gattr(a_g_utf8,cattr));
		}

	      if (do_signnames)
		{
		  static const unsigned char *cattr = NULL;
		  if (gp->type == g_q)
		    cattr = signify(gp->g.q.q->g.s.base);
		  else
		    cattr = signify(buf);
		  if (cattr)
		    appendAttr(gp->xml,gattr(a_g_sign,cattr));
		}

	      if (!inner_parse && f_graphemes)
		fprintf(f_graphemes,"%s ",buf);
	    }
	}
      else
	{
	  if (!inner_parse)
	    {
	      static unsigned char buf[1024] = { '\0' };
	      unsigned char *insertp = NULL;

	      if (do_cuneify && cuneifiable(curr_lang))
		{
		  insertp = render_g(gp->xml,buf,buf);
		  *insertp = '\0';
		  if (cbd_rules)
		    {
		      unsigned char *a = utf2atf(buf);
		      if (a)
			{
			  const unsigned char *m = g2utf(a);
			  if (m)
			    strcpy((char*)buf,(const char *)m);
			}
		    }
		  if (*buf)
		    {
		      const unsigned char *cattr = cuneify(buf);
		      if (cattr)
			appendAttr(gp->xml,gattr(a_g_utf8,cattr));

		      if (do_signnames)
			{
			  cattr = NULL;
			  if (gp->type == g_q)
			    cattr = signify(gp->g.q.q->g.s.base);
			  else
			    cattr = signify(buf);
			  if (cattr)
			    appendAttr(gp->xml,gattr(a_g_sign,cattr));
			}
		    }
		  else
		    {
		      if (gp->type == g_p)
			{
			  const unsigned char *cattr = cuneify(getAttr(gp->xml,"g:type"));
			  if (cattr)
			    appendAttr(gp->xml,gattr(a_g_utf8,cattr));

			  if (do_signnames)
			    {
			      cattr = signify(getAttr(gp->xml,"g:type"));
			      if (cattr)
				appendAttr(gp->xml,gattr(a_g_sign,cattr));
			    }
			}
		    }
		}
	      if (f_graphemes)
		{
		  if (!insertp)
		    {
		      insertp = render_g(gp->xml,buf,buf);
		      *insertp = '\0';
		    }
		  fprintf(f_graphemes,"%s ",buf);
		}
	    }
	}

      /* FIXME: this allows instance attributes to be put onto the
	 gp->xml that is in the hash table; need to scan CG's once
	 without processing flags and brackets, then clone and
	 process again */
      if (*cued_opener)
	{
	  appendAttr(gp->xml,gattr(a_g_o,ucc(cued_opener)));
	  *cued_opener = '\0';
	}
      if (gp->type == g_c)
	{
	  unsigned const char *form = getAttr(gp->xml,"form");
	  if (!psl_is_sname(form) && qualifier_warnings)
	    vnotice("unknown compound sign %s",form);
	}
      else if (gp->type == g_v && !inner_parse)
	{
	  if (x_value(gp->g.s.base) && qualifier_warnings)
	    vnotice("%s: x-values should be qualified",gp->g.s.base);
	}
    }
#ifdef NEW_ERROR_RECOVERY
  else
    {
      /* make it impossible for gparse to return NULL */
      gp = singleton(g,g_v);
      gp->gflags = GFLAGS_BAD;
    }
#endif

  return gp;
}

static int
gnr(register unsigned char *g)
{
  return ((*g == 'n' || *g == 'N') && (g[1]=='(' || g[1]=='+')) ? '1' : 0;
}

static void
set_128(int *start)
{
  int i;
  for (i = 0; i <128; ++i)
    start[i] = 1;
}

void
graphemes_init()
{
  register int c;

  graphemes_pool = npool_init();

  for (c = 0; grapheme_base_chars[c]; ++c)
    is_grapheme_base[(int)grapheme_base_chars[c]] = 1;
  set_128(is_grapheme_base+128);
  if (cbd_rules)
    is_grapheme_base['*'] = 1;

  for (c = 0; compound_base_chars[c]; ++c)
    is_compound_base[(int)compound_base_chars[c]] = 1;
  set_128(is_compound_base+128);

  for (c = 0; bad_cg_chars[c]; ++c)
    is_bad_cg[(int)bad_cg_chars[c]] = 1;

  for (c = 0; c < o_top; ++c)
    {
      op_nodes[c] = *(gelem(e_g_o,NULL,lnum,GRAPHEME));
      op_nodes[c].user = (void*)(uintptr_t)op_chars[c];
      appendAttr((&op_nodes[c]),gattr(a_g_type,ucc(op_names[c])));
      ops_by_char[(int)op_chars[c]] = &op_nodes[c];
    }

  brokenattr = gattr(a_g_break,ucc("missing"));
  hashattr = gattr(a_g_break,ucc("damaged"));
  bangattr = gattr(a_g_remarked,ucc("1"));
  queryattr = gattr(a_g_queried,ucc("1"));
  collatedattr = gattr(a_g_collated,ucc("1"));
  implattr = gattr(a_g_status,ucc("implied"));
  supplattr = gattr(a_g_status,ucc("supplied"));
  excisattr = gattr(a_g_status,ucc("excised"));
  okattr = gattr(a_g_status,ucc("ok"));

  gtags[g_v] = e_g_v;
  gtags[g_c] = e_g_c;
  gtags[g_p] = e_g_p;
  gtags[g_q] = e_g_q;
  gtags[g_s] = e_g_s;
  gtags[g_n] = e_g_n;
  gtags[g_m] = e_g_m;
  gtags[g_a] = e_g_a;
  gtags[g_f] = e_g_f;
  gtags[g_g] = e_g_g;
  gtags[g_b] = e_g_b;

  psl_init();
}

void
graphemes_term()
{
  register int c;
  for (c = 0; c < o_top; ++c)
    {
      if (op_nodes[c].children.nodes)
	{
	  free(op_nodes[c].children.nodes);
	  op_nodes[c].children.nodes = NULL;
	  op_nodes[c].children.lastused = 0;
	}
      if (op_nodes[c].attr.nodes)
	{
	  free(op_nodes[c].attr.nodes);
	  op_nodes[c].attr.nodes = NULL;
	  op_nodes[c].attr.lastused = 0;
	}
    }

  npool_term(graphemes_pool);
  psl_term();
}

enum t_type
gtype(register unsigned char *g)
{
  if ((*g > 127 || isalpha(*g) || *g == '\'') && !gnr(g))
    {
      if (xstrchr(g,'('))
	{
	  while (*g)
	    if ('!' == *g)
	      return g_v;
	    else if ('(' == *g)
	      return g_q;
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
		  return g_v;
		else if (isdigit(*g) || '@' == *g || '~' == *g)
		  break;
		else
		  g += mbtowc(NULL,(const char *)g,6);
	      return g_s;
	    }
	  else
	    return g_v;
	}
    }
  else if (isdigit(*g) || *g == 'n' || *g == 'N')
    return g_n;
  else if (*g == '|')
    {
      register unsigned char *e = g;
      while (*++e != '|')
	;
      if (*e == '(')
	return g_q;
      else
	return g_c;
    }
  else if (*g == '*' || *g == ':')
    return g_p;
  else
    return -1;
}

static struct grapheme *
compound(register unsigned char *g)
{
  struct grapheme *gp = calloc(1,sizeof(struct grapheme));
  int status = 0;
  gp->type = g_c;
  gp->xml = gelem(gtags[g_c],NULL,lnum,GRAPHEME);
  status = cparse(gp->xml,g+1,'|',NULL);
  if (!status)
    {
      if (gp->xml->children.lastused == 1)
	{
	  warning("unnecessary pipes");
	  free(gp);
	  return NULL;
	}
      else
	{
	  if (strchr(cc(g),'['))
	    gp->gflags |= GFLAGS_HAVE_SQUARE;
	  return gp;
	}
    }
  else
    {
      free(gp);
      return NULL;
    }
}

static struct attr *
presence_attr_of(const char *endpat)
{
  if (!strcmp(endpat,">"))
    return supplattr;
  else if (!strcmp(endpat,">>"))
    return excisattr;
  else
    return implattr;
}

static int
is_sub_x(unsigned char *p)
{
  wchar_t wbuf[2];
  (void)mbtowc(wbuf,(const char *)p,6);
  return *wbuf == 0xd7;
}

#define set_endptr(p)			  \
  if (endptr)				  \
    {					  \
      while (*p && '|' != *p)		  \
	++p;				  \
      *endptr = p;			  \
    }					  \
  else

static int
cparse(struct node *parent, unsigned char *g, const char end, 
       unsigned char **endptr)
{
  struct node *last_g = NULL;

  while (*g)
    {
      struct node *np = NULL;
      if (end == *g)
	{
	  if (endptr)
	    *endptr = g;
	  return 0;
	}
      else if (isdigit(*g) 
	       && (('x' == g[1])
		   || (g[2] && g[1] == 0xc3 && g[2] == 0x97))) /* UTF-8 TIMES SYMBOL */
	{
	  /* 4xLU2 and the like is a rare construct; there is no need
	     to worry about conserving nodes or efficiency here */
	  unsigned char buf[2];
	  buf[0] = *g;
	  buf[1] = '\0';
	  last_g = np = gtextElem(e_g_o,NULL,lnum,GRAPHEME,buf);
	  np->user = (void*)(uintptr_t)'x';
	  /* it has to be wrong to put these attributes on a g:o node */
	  appendAttr(np,gattr(a_g_type,ucc("repeated")));
	  g += 2;
	  if (*g == 0x97)
	    ++g;
	}
      else if (is_compound_base[*g])
	{
	  unsigned char *endp = g;
	  unsigned char save;
	  struct grapheme *gp;
	  if (*g > 127 && is_sub_x(g))
	    {
	      np = ops_by_char['x'];
	      last_g = NULL;
	      mbincr(endp);
	      g = endp;
	      appendChild(parent,np);
	      continue;
	    }
	  else if (isdigit(*endp) || ('N' == *endp && '(' == endp[1]))
	    {
	      int nesting = 0;
	      if ('N' == *endp)
		++endp;
	      else
		while (isdigit(*endp))
		  ++endp;
	      if ('(' == *endp)
		{
		  while (*endp)
		    {
		      if ('(' == *endp)
			++nesting;
		      else if (')' == *endp)
			{
			  if (--nesting == 0)
			    {
			      ++endp;
			      break;
			    }
			  if (nesting < 0)
			    {
			      warning("mismatched (...) in compound");
			      set_endptr(endp);
			      return 1;
			    }
			}
		      ++endp;
		    }
		}
	    }
	  else
	    {
	      while (is_compound_base[*endp] && (*endp < 128 || !is_sub_x(endp)))
		mbincr(endp);
	      if (*endp != 'x' && u_islower(endp))
		mbincr(endp);
	    }
	  while (('@' == *endp && !u_isupper(endp+1)) || '~' == *endp)
	    {
	      ++endp;
	      while (*endp < 128 && isalnum(*endp) && 'x' != *endp)
		++endp;
	    }
	  if ('(' == *endp)
	    {
	      while (*endp && ')' != *endp)
		++endp;
	      if (!*endp)
		{
		  warning("qualified grapheme in compound missing ')'");
		  return 1;
		}
	      ++endp;
	    }
	  save = *endp;
	  *endp = '\0';
	  ++inner_parse;
	  gp = gparse(g,type_top);
	  --inner_parse;
	  if (gp)
	    {
	      last_g = np = gp->xml;
	      free(gp);
	      gp = NULL;
	      if (in_square)
		appendAttr(np,brokenattr);
	      if (in_presence)
		appendAttr(np,presence_attr_of(curr_presence));
	      *endp = save;
	      g = endp;
	    }
	  else
	    {
	      set_endptr(endp);
	      return 1;
	    }
	}
      else
	{
	  unsigned char *eptr;
	  switch (*g)
	    {
	    case '(':
	      last_g = np = gelem(gtags[g_g],NULL,lnum,GRAPHEME);
	      if (!cparse(np,g+1,')',&eptr))
		{
		  unsigned char *mods = NULL;
		  if (np->children.lastused == 1)
		    {
		      warning("unnecessary group in pipes");
		      return 1;
		    }
		  g = eptr+1;
		  if (('@' == *g && (g[1] && !u_isupper(g+1) && '(' != g[1])) || '~' == *g)
		    {
		      mods = g;
		      while ('@' == *g || '~' == *g)
			{
			  ++g;
			  while (*g < 128 && isalnum(*g) && 'x' != *g)
			    ++g;
			}
		    }
		  if (mods)
		    {
		      static struct mods modsbuf[MODS_MAX];
		      int nmods = 0;
		      unsigned char save;
		      save = *g;
		      *g = '\0';
		      appendChild(parent,np);
		      nmods = gmods(mods,modsbuf);
		      if (nmods >= 0)
			{
			  process_mods(parent,nmods,modsbuf);
			  *g = save;
			  /* skip the default appendChild(parent,np) at bottom of loop */
			  continue;
			}
		      else
			{
			  if (g)
			    ++g;
			  continue;			  
			}
		    }
		}
	      else
		last_g = np = NULL;
	      break;
	    case 'x':
	      {
		struct node *prev = lastChild(parent);
		if (!prev || !xstrcmp(prev->names->pname,"g:o"))
		  {
		    warning("misplaced 'x' (did you mean 'X'?)");
		    np = NULL;
		  }
		else
		  {
		    np = ops_by_char['x'];
		    last_g = NULL;
		    ++g;
		  }
	      }
	      break;
	    case '.':
	    case ':':
	    case '+':
	    case '&':
	    case '%':
	    case '@':
	      if (g[1] && strchr("x.:+&%@",g[1]))
		{
		  vwarning("%c%c: double boundary in |...|",g[0],g[1]);
		}
	      else if ('|' == g[1] || !g[1])
		{
		  vwarning("%c: compound grapheme must not end with boundary",
			   g[0]);
		}
	      np = ops_by_char[*g];
	      last_g = NULL;
	      ++g;
	      break;
#define cflag(nodeptr,a,s,c)			\
	      if (nodeptr)				\
		{						\
		  if (gsetAttr(nodeptr,a,ucc(s)))		\
		    vwarning("%c: superfluous flag within |...|",c);		\
		} \
	      else						\
		vwarning("%s: misplaced flag within |...|",g)
	    case '#':
	      if (in_square)
		{
		  warning("hash flag inside [...] within |...|");
		}
	      else
		{
		  cflag(last_g,a_g_break,"damaged",'#');
		}
	      ++g;
	      continue;
	    case '?':
	      cflag(last_g,a_g_queried,"1",'?');
	      ++g;
	      continue;
	    case '!':
	      cflag(last_g,a_g_remarked,"1",'!');
	      ++g;
	      continue;
	    case '*':
	      cflag(last_g,a_g_collated,"1",'*');
	      ++g;
	      continue;
#undef cflag
	    case '[':
	      cue_opener("[");
	      ++g;
	      continue;
	    case ']':
	      in_square = 0;
	      appendCloser(last_g,"]");
	      ++g;
	      continue;
	    case '<':
	      in_presence = 1;
	      switch (g[1])
		{
		case '<':
		  curr_presence = ">>";
		  cue_opener("<<");
		  break;
		case '(':
		  curr_presence = ")>";
		  cue_opener("<(");
		  break;
		case '{':
		  curr_presence = "}>";
		  cue_opener("<{");
		  break;
		default:
		  curr_presence = ">";
		  cue_opener("<");
		  break;
		}
	      g+=strlen(curr_presence);
	      continue;
	    case '>':
	      if (g[1] == '>')
		{
		  if (in_presence && !strcmp(curr_presence,">>"))
		    {
		      in_presence = 0;
		      curr_presence = NULL;
		      appendCloser(last_g,">>");
		      g+=2;
		    }
		  else
		    {
		      warning("unexpected >> in |...|");
		      in_presence = 0;
		      curr_presence = NULL;
		      ++g;
		    }
		}
	      else
		{
		  if (in_presence && !strcmp(curr_presence,">"))
		    {
		      in_presence = 0;
		      curr_presence = NULL;
		      appendCloser(last_g,">");
		      ++g;
		    }
		  else
		    {
		      warning("unexpected > in |...|");
		      in_presence = 0;
		      curr_presence = NULL;
		      ++g;
		    }
		}
	      continue;
	    case ')':
	      if (g[1] == '>')
		{
		  in_presence = 0;
		  curr_presence = NULL;
		  appendCloser(last_g,")>");
		  g+=2;
		}
	      else
		{
		  warning("unexpected ) in |...|");
		  ++g;
		}
	      continue;
	    case '}':
	      if (g[1] == '>')
		{
		  in_presence = 0;
		  curr_presence = NULL;
		  appendCloser(last_g,"}>");
		  g+=2;
		}
	      else
		{
		  warning("unexpected } in |...|");
		  ++g;
		}
	      continue;
	    default:
	      warning("illegal character in |...|");
	      set_endptr(g);
	      return 1;
	    }
	  if (!np)
	    break;
	}
      if (np)
	appendChild(parent,np);
      else
	{
	  set_endptr(g);
	  return 1;
	}
    }
  warning("end character not found");
  if (endptr)
    *endptr = g;
  return 1;
}

static struct grapheme *
qualified(register unsigned char *g)
{
  register unsigned char *g2 = g;
  struct grapheme *gp = NULL;
  if ('|' == *g2)
    while (*++g2 != '|')
      ;
  while (*g2 && '(' != *++g2)
    ;
  if ('(' == *g2)
    {
      struct grapheme *q_g;
      *g2++ = '\0';
      if ('x' == *g && !g[1])
	{
	  vwarning("x(%s: use 'n' for numbers or don't qualify 'x'",g2);
	  return NULL;
	}
      ++inner_parse;
      inner_qual = 1;
      q_g = gparse(g,type_top);
      --inner_parse;
      inner_qual = 0;
      if (q_g)
	{
	  g = g2;
	  g2 += xxstrlen(g2);
	  while (')' != g2[-1] && g2> g)
	    --g2;
	  if (')' == g2[-1])
	    {
	      struct grapheme *q_q;
	      g2[-1] = '\0';
	      ++inner_parse;
	      q_q = gparse(g,type_top);
	      --inner_parse;
	      if (q_q)
		{
		  if (q_q->type == g_v)
		    {
		      vwarning("%s: grapheme values not allowed as qualifiers", g);
		      return NULL;
		    }
		  else
		    {
		      gp = calloc(1,sizeof(struct grapheme));
		      gp->gflags = q_g->gflags;
		      gp->type = g_q;
		      gp->g.q.g = q_g;
		      gp->g.q.q = q_q;
		      gp->xml = gelem(e_g_q,NULL,lnum,GRAPHEME);
		      appendChild(gp->xml,q_g->xml);
		      appendChild(gp->xml,q_q->xml);
		    }
		}
	    }
	  else
	    {
	      warning("malformed qualified grapheme: no (...)");
	      return NULL;
	    }
	}
    }
  else
    {
      warning("malformed qualified grapheme: no (...)");
      return NULL;
    }
  return gp;
}

static int
last_is_em(const unsigned char *sp, const unsigned char *ip)
{
  int last_is_em = 0;
  if (ip - sp > 3 && ip[-1] == 0x94)
    {
      if (ip[-2] == 0x80 && ip[-3] == 0xe2)
	{
	  last_is_em = 1;
	  /*fprintf(stderr, "last_is_em = 1\n");*/
	}
    }
  return last_is_em;
}

static struct grapheme *
punct(register unsigned char *g)
{
  register unsigned char *g2;
  struct grapheme *gp = NULL;
  struct grapheme *p_q = NULL;
  char ptok[4];
  *ptok = g[0];
  if (g[1] == ':' || g[1] == '.' || g[1] == '\'' || g[1] == '"' || (g[1] == 'r' && g[2] == ':'))
    {
      ptok[1] = g[1];
      if (g[1] == 'r')
	{
	  ptok[2] = ':';
	  ptok[3] = '\0';
	}
      else
	ptok[2] = '\0';
      ++g;
    }
  else
    ptok[1] = '\0';

  if ((ptok[0] == '*' && !ptok[1])
      || (ptok[0] == '/' && !ptok[1])
      || (ptok[0] == '|' && !ptok[1])
      || (ptok[0] == ':' 
	  && ((!ptok[1] || ptok[1] == ':' || ptok[1] == '.' || ptok[1] == '\'' || ptok[1] == '"')
	      || (ptok[1] == 'r' && ptok[2] == ':'))))
    {
      /* do nothing; we're valid */
    }
  else
    {
      vwarning("%s: unknown punctuation (known: * : :. :: / |)",ptok);
      return NULL;
    }

  if (g[1] == '(')
    {
      g += 2;
      g2 = g + xxstrlen(g);
      g2[-1] = '\0';
      ++inner_parse;
      p_q = gparse(g,type_top);
      --inner_parse;
    }
  gp = calloc(1,sizeof(struct grapheme));
  gp->type = g_p;
  gp->g.p.g = p_q;
  gp->xml = gelem(e_g_p,NULL,lnum,GRAPHEME);
  appendAttr(gp->xml,gattr(a_g_type,uc(ptok)));
  if (p_q)
    appendChild(gp->xml,p_q->xml);
  return gp;
}

static struct grapheme *
numerical(register unsigned char *g)
{
  struct grapheme*gp;
  unsigned char *n, *q, *end = g+xxstrlen(g),*orig_g = g;
  while (*g && '(' != *g && '@' != *g && '~' != *g)
    ++g;
  if ('(' == *g)
    {
      unsigned char *gsaveo = g;
      gp = calloc(1,sizeof(struct grapheme));
      gp->type = g_n;
      if (g[-1] == '/')
	{
	  vwarning("%s: incomplete fraction", gp->g.n.r);
	  free(gp);
	  return NULL;
	}
      *g++ = '\0';
      gp->g.n.r = pool_copy(orig_g); /*FIXME: HASH or ARRAY THESE REPEATERS*/
      n = g;
      g = end;
      while (')' != *--g && *g)
	;
      *gsaveo = '(';
      if (*g)
	{
	  unsigned char *gsavec = g;
	  *g++ = '\0';
	  if (g < end && *g)
	    q = g;
	  else
	    q = NULL;
	  ++inner_parse;
	  gp->g.n.n = gparse(n,type_top);
	  --inner_parse;
	  *gsavec = ')';
	  if (gp->g.n.n)
	    {
	      gp->gflags = gp->g.n.n->gflags;
	      if (gp->g.n.n->type == g_n)
		{
		  warning("number graphemes not nestable");
		  free(gp);
		  gp = NULL;
		}
	      else
		{
		  int nmods = 0;
		  static struct mods modsbuf[MODS_MAX];
		  if (q)
		    nmods = gmods(q,modsbuf);
		  if (nmods >= 0)
		    {
		      struct node *r = gtextElem(e_g_r,NULL,lnum,GRAPHEME,gp->g.n.r);
		      struct node *n = gp->g.n.n->xml;
		      /* build an elem with an empty text child */
		      gp->xml = build_singleton((unsigned char*)"",g_n,nmods,modsbuf);
		      gp->type = g_n;
		      /* replace the empty text child with the r/n pair */
		      replaceChild(gp->xml,0,r);
		      insertBefore(gp->xml,1,n);
		    }
		  else
		    {
		      free(gp);
		      gp = NULL;
		    }
		}
	    }
	  else
	    {
	      free(gp);
	      gp = NULL;
	    }
	}
      else
	{
	  warning("malformed numeric grapheme: no closing ')'");
	  free(gp);
	  gp = NULL;
	}
    }
  else if ('n' == *orig_g) /*(*gp->g.n.r == 'n') */
    {
      struct node *r;
      gp = calloc(1,sizeof(struct grapheme));
      gp->type = g_n;
      gp->g.n.r = orig_g;
      r = gtextElem(e_g_r,NULL,lnum,GRAPHEME,gp->g.n.r);

      /* build an elem with an empty text child */
      gp->xml = build_singleton((unsigned char*)"",g_n,0,NULL);
      gp->type = g_n;
      /* replace the empty text child with the "n" node in g:r */
      replaceChild(gp->xml,0,r);
    }
  else
    {
      if (saa_mode || math_mode)
	{
	  /* in SAA mode any unadorned sexagesimal number is OK;
	     in math_mode they should be limited to 60 */
	  struct node *r;
	  int nmods = 0;
	  static struct mods modsbuf[MODS_MAX];
	  unsigned char *qnum = NULL, *qtmp = NULL;
	  gp = calloc(1,sizeof(struct grapheme));
	  gp->g.n.r = orig_g;
	  g = orig_g;
	  while (is_grapheme_base[*g] || '/' == *g)
	    ++g;
	  if (*g)
	    {
	      nmods = gmods(g,modsbuf);
	      if (nmods > 1 || modsbuf[0].data[0] != 'v' || modsbuf[0].data[1])
		warning("mods on unqualified number are not allowed, say, e.g., 1(disz@c) not 1@c");
	    }
	  if (strchr((char*)gp->g.n.r, '/'))
	    {
	      qnum = qtmp = malloc(strlen((char*)gp->g.n.r) + 7);
	      sprintf((char*)qnum, "%s(disz)", gp->g.n.r);
	    }
	  else
	    {
	      unsigned char *sx = sexify(atoi((char*)gp->g.n.r), "disz");
	      if (!sx)
		sx = (unsigned char *)"00(disz)";
	      qnum = malloc(strlen((char*)sx) + 3);
	      if (nmods == 1 && modsbuf[0].data[0] == 'v')
		sprintf((char*)qnum, "%s@v", (char*)sx);
	      else
		strcpy((char*)qnum, (char*)sx);
	    }
	  r = gtextElem(e_g_r,NULL,lnum,GRAPHEME,gp->g.n.r);
	  gp->g.n.n = NULL;
	  gp->xml = build_singleton((unsigned char*)"",g_n,nmods,modsbuf);
	  gp->type = g_n;
	  /* replace the empty text child with the "n" node in g:r */
	  replaceChild(gp->xml,0,r);
	  gsetAttr(gp->xml,a_sexified,qnum);
	  if (qtmp)
	    free(qtmp);
	}
      else
	{
	  /* A bit of black magic: if the char after the number is
	     UNICODE TIMES in UTF-8, the first byte has been 
	     overwritten by the NULL and the second byte, 151, is
	     one byte after the NULL */
	  if (g[1] != 151)
	    warning("malformed numeric grapheme: no (...)");
	  /*	  free(gp);*/
	  gp = NULL;
	}
    }
  return gp;
}

static struct grapheme *
singleton(register unsigned char *g, enum t_type type)
{
  struct grapheme*gp = calloc(1,sizeof(struct grapheme));
  int nmods = 0;
  static struct mods modsbuf[MODS_MAX];
  const unsigned char *utf8g = NULL;
  
  assert(gp!=NULL);
  assert(g!=NULL);
  gp->type = type;
  gp->g.s.base = g;
  while (is_grapheme_base[*g])
    ++g;
  if (*g)
    nmods = gmods(g,modsbuf);
  if (nmods < 0)
    {
      vwarning("error parsing modifiers");
      nmods = 0;
      gp->gflags = GFLAGS_BAD;
    }
  utf8g = gp->g.s.base;

  if (!use_unicode && !is_signlist(g))
    {
      if ((utf8g = g2utf(gp->g.s.base)))
	utf8g = gp->g.s.base = pool_copy(utf8g);
      else
	utf8g = gp->g.s.base = pool_copy(gp->g.s.base);
    }
  else if (curr_lang->cset)
    {
      size_t wlen = 0;
      wchar_t *wbase = utf2wcs(gp->g.s.base, &wlen);
      curr_lang->cset->val(wbase,wlen);
    }
  
  gp->xml = build_singleton(utf8g,gp->type, nmods, modsbuf);
  return gp;
}

void
cue_opener(const char *o)
{
  strcat(cued_opener,o);
}

void
appendCloser(struct node *gp,const char *c)
{
  static char buf[128];
  if (gp)
    {
      strcpy(buf,cc(getAttr(gp,"g:c")));
      strcat(buf,c);
      gsetAttr(gp,a_g_c,ucc(buf));
    }
}

unsigned char *
render_g_text(struct node*tp, unsigned char *insertp, unsigned char *startp)
{
  if (rg_verbose && tp)
    fprintf(stderr,"render_g_text: tp=%p; data=%s\n", (void*)tp, (char*)tp->data);
  
  if (tp->data)
    insertp += xxstrlen(xstrcpy(insertp, tp->data));
  else
    {
      *insertp = '\0';
      vwarning("attempt to render null tp->data: form so far=%s", startp);
    }
  return insertp;
}

unsigned char *
_render_g(struct node *np, unsigned char *insertp, unsigned char *startp, const char *FILE, int LINE)
{
  const unsigned char *aval;
  extern int suppress_next_hyphen, suppress_hyphen_delay;
  static int depth = 0;

  ++depth;
  
  if (rg_verbose && np)
    fprintf(stderr,"render_g:%s:%d: np=%p (depth %d)\n", FILE, LINE, (void*)np, depth);

  if (!np || *np->names->pname == '\0')
    {
      vwarning("attempt to render NULL grapheme [np=%p]",np);
      return NULL;
    }

  if (!xstrcmp(getAttr(np, "g:status"),"excised")
      || (!xstrcmp(np->names->pname, "g:x") && strcmp("ellipsis", (const char*)getAttr(np,"g:type"))))
    {
      /* This is probably never right any more, because those '.' and '-' have 
       * been put there by a g:delim attr which knows what it is doing 
       */
#if 0
      if (insertp > startp && (insertp[-1] == '.' || insertp[-1] == '-'))
	*--insertp = '\0';
#endif
      return insertp;
    }

  /* N.B.: we do not render g:p here so that punctuation creates
     a g:nonw node rather than a g:w */
  if (np->names)
    switch (np->names->pname[2])
      {
      case 'b':
	if (np->children.nodes)
	  insertp = render_g_text(np->children.nodes[0], insertp, startp);
	break;
      case 'v':
      case 's':
	if (np->names->pname[3] && !strcmp(np->names->pname,"g:surro"))
	  {
	    if (*((struct node *)(np->children.nodes[1]))->type == 't') /* normalization */
	      insertp = render_g_text(np->children.nodes[1], insertp, startp);
	    else
	      insertp = render_g(np->children.nodes[1],insertp,startp);
	  }
	else
	  {
	    if (np->children.lastused)
	      {
		struct node *cp1 = np->children.nodes[0];
		if (!strcmp((char*)getAttr(np,"g:role"),"sign"))
		  *insertp++ = '$';
		if (*cp1->type == 't')
		  {
		    if (np->grapheme)
		      insertp += xxstrlen(xstrcpy(insertp, np->grapheme->raw));
		    else
		      insertp = render_g_text(cp1, insertp, startp);
		  }
		else
		  {
		    int i;
		    for (i = 0; i < np->children.lastused; ++i)
		      insertp = render_g(np->children.nodes[i], insertp, startp);
		  }
	      }
	  }
	break;
      case 'n':
	if (np->children.nodes)
	  {
	    insertp = render_g(np->children.nodes[0], insertp, startp);
	    if (np->children.lastused > 1)
	      {
		int render_me_next = 1;
		int i;
		char c1type = ((struct node *)np->children.nodes[1])->names->pname[2];
		if (c1type != 'm' && c1type != 'a' && c1type != 'f')
		  {
		    *insertp++ = '(';
		    insertp = render_g(np->children.nodes[1], insertp, startp);
		    *insertp++ = ')';
		    render_me_next = 2;
		  }
		for (i = render_me_next; i < np->children.lastused; ++i)
		  insertp = render_g(np->children.nodes[i], insertp, startp);
	      }
	  }
	break;
      case 'c':
	*insertp++ = '|';
	{
	  int i;
	  for (i = 0; i < np->children.lastused; ++i)
	    insertp = render_g(np->children.nodes[i], insertp, startp);
	}
	*insertp++ = '|';
	break;
      case 'g':
	if (!xstrcmp(np->names->pname,"g:gloss"))
	  {
	    suppress_next_hyphen = 1;
#if 0
	    if (!xstrcmp(getAttr(np,"g:pos"),"post"))
	      {
		/* probably wrong w new g:delim based code */
		if (insertp[-1] != '+')
		  *--insertp = '\0'; /* unhyphenate */
	      }
	    else
	      suppress_next_hyphen = 1;
#endif
	  }
	else if (!xstrcmp(np->names->pname,"g:gg"))
	  {
	    const char *gtype = cc(getAttr(np,"g:type"));
	    if (!xstrcmp(gtype, "reordering"))
	      {
		int i;
		for (i = 0; i < np->children.lastused; ++i)
		  {
		    if (i)
		      {
			if (xstrcmp(getAttr(np->children.nodes[i-1],"g:pos"),"pre"))
			  *insertp++ = '-';
		      }
		    insertp = render_g(np->children.nodes[i], insertp, startp);
		  }
	      }
	    else if (!xstrcmp(gtype, "correction"))
	      /*omit second element in form rendering*/
	      insertp = render_g(np->children.nodes[0], insertp, startp);
	    else if (!xstrcmp(gtype,"alternation"))
	      {
		/*omit second element in form rendering*/
		if (np->children.nodes)
		  insertp = render_g(np->children.nodes[0], insertp, startp);
	      }
	    /* WE NEED A DET GROUP TYPE SO WE CAN KNOW FOR SURE WHEN TO
	       EMIT DELIMS EVEN WHEN SUPPRESS_NEXT_HYPHEN IS TRUE */
	    else if (!xstrcmp(gtype, "group")
		     || !xstrcmp(gtype, "logo"))
	      {
		if (np->children.nodes)
		  {
		    int i;
		    /*int last_was_logo = !xstrcmp(gtype,"logo");*/
		    /* if (!xstrcmp(gtype, "group")) */
		      ++suppress_hyphen_delay;
		    for (i = 0; i < np->children.lastused; ++i)
		      {
			if (i)
			  {
			    /* never add a hyphen after a pre-det */
			    if (xstrcmp(getAttr(np->children.nodes[i-1],"g:pos"),"pre"))
			      {
				if (!suppress_next_hyphen || suppress_hyphen_delay)
				  {
				    const unsigned char *gdelim = getAttr(np->children.nodes[i-1], "g:delim");
				    if (*gdelim)
				      if (!last_is_em(startp,insertp) && insertp[-1] != '-' && insertp[-1] != '.')
					{
					  if (gdelim[1])
					    *insertp++ = '-';
					  else
					    *insertp++ = *gdelim++;					  
					}
				    if (!suppress_hyphen_delay)
				      suppress_next_hyphen = 0;
				  }
			      }
			    else
			      {
				if (!suppress_hyphen_delay)
				  suppress_next_hyphen = 0;
				/* fprintf(stderr, "no hyphen\n"); */
			      }
			  }
			if (*((struct node *)(np->children.nodes[i]))->type == 't') /* normalization */
			  insertp = render_g_text(np->children.nodes[i], insertp, startp);
			else
			  {
			    insertp = render_g(np->children.nodes[i],insertp,startp);
			  }
		      }
		    if (!xstrcmp(gtype, "group"))
		      --suppress_hyphen_delay;
		  }
	      }
	    else if (!xstrcmp(gtype, "ligature"))
	      {
		int i;
		for (i = 0; i < np->children.lastused; ++i)
		  {
		    const unsigned char *gdelim
		      = getAttr(np->children.nodes[i], "g:delim");
		    insertp = render_g(np->children.nodes[i], insertp, startp);
#if 1
		    if (*gdelim)
		      {
			const unsigned char *tmp = gdelim;
			while (*tmp)
			  *insertp++ = *tmp++;
		      }
#else
		    if (i)
		      *insertp++ = '+';
#endif
		  }
	      }
	    else
	      {
		int i;
		*insertp++ = '(';
		for (i = 0; i < np->children.lastused; ++i)
		  insertp = render_g(np->children.nodes[i], insertp, startp);
		*insertp++ = ')';
	      }
	    if (*(cc(getAttr(np, "g:delim"))))
	      {
		const unsigned char *gdelim = getAttr(np, "g:delim");
		if (*gdelim && !last_is_em(startp,insertp) && insertp[-1] != '-' && insertp[-1] != '.')
		  {
		    if (gdelim[1])
		      *insertp++ = '-';
		    else
		      *insertp++ = *gdelim++;
		  }
	      }
	    else if (lastChild(np))
	      {
		const unsigned char *gdelim = getAttr(lastChild(np), "g:delim");
		if (*gdelim && !last_is_em(startp,insertp) && insertp[-1] != '-' && insertp[-1] != '.')
		  {
		    if (gdelim[1])
		      *insertp++ = '-'; /* only em-dash is more than one char */
		    else
		      *insertp++ = *gdelim++;
		  }
	      }
	  }
	else if (!xstrcmp(np->names->pname,"g:g"))
	  {
	    int i;
	    *insertp++ = '(';
	    for (i = 0; i < np->children.lastused; ++i)
	      insertp = render_g(np->children.nodes[i], insertp, startp);
	    *insertp++ = ')';
	  }
	break;
      case 'q':
	insertp = render_g(np->children.nodes[0], insertp, startp);
	if (np->children.lastused 
	    && (!np->user || np->user != (void*)1))
	  {
	    *insertp++ = '(';
	    insertp = render_g(np->children.nodes[1], insertp, startp);
	    *insertp++ = ')';
	  }
	break;
      case 'r':
	if (!xstrcmp(np->names->pname,"surro"))
	  {
	    if (*((struct node *)(np->children.nodes[1]))->type == 't') /* normalization */
	      insertp = render_g_text(np->children.nodes[1], insertp, startp);
	    else
	      insertp = render_g(np->children.nodes[1],insertp,startp);
	  }
	else
	  {
	    if (np->children.nodes)
	      insertp = render_g_text(np->children.nodes[0], insertp, startp);
	  }
	break;
      case 'a':
	/* Most languages won't use the ~-allographs in a way that affects
	   the analysis of the form--we can't be certain that this is the case 
	   in proto-cuneiform, and we may need to add more cases as well */
	if (!rendering_word_form || curr_lang->core->code == c_qcu) {
	  *insertp++ = '~';
	  if (np->children.nodes)
	    insertp = render_g_text(np->children.nodes[0], insertp, startp);
	}
	break;
      case 'f':
	; /* never render formvars */
	break;
      case 'm':
	*insertp++ = '@';
	if (np->children.nodes)
	  insertp = render_g_text(np->children.nodes[0], insertp, startp);
	break;
      case 'd':
	if (np->names->pname[3])
	  insertp += xxstrlen(xstrcpy(insertp, "x-x-x"));
	else
	  {
	    if (!xstrcmp(getAttr(np,"g:pos"),"post"))
	      {
		if (in_split_word)
		  {
		    if (insertp == startp)
		      {
			*insertp++ = 0x1; /* put a flag in byte 0 to tell us
					     to unhyphenate when assembling
					     the components of a form */
		      }
		    else if (insertp - startp == 2 
			     && startp[0] == '%' 
			     && startp[1] == '%')
		      {
			startp[0] = 0x1;
			startp[1] = startp[2] = '%';
			insertp = startp + 3;
		      }
		    else if (insertp_is_delim())
		      *--insertp = '\0';
		  }
		else if (insertp_is_delim())
		  *--insertp = '\0'; /* unhyphenate */
	      }
	    else
	      suppress_next_hyphen = 1;
	    *insertp++ = '{';
	    if (!xstrcmp(getAttr(np,"g:role"),"phonetic"))
	      *insertp++ = '+';
	    {
	      int i;
	      for (i = 0; i < np->children.lastused; ++i)
		{
		  if (i)
		    {
		      if (!last_is_em(startp,insertp) && insertp[-1] != '-' && insertp[-1] != '.')
			*insertp++ = '-';
		    }
		  if (*((struct node*)(np->children.nodes[i]))->type == 't')
		    insertp = render_g_text(np->children.nodes[i], insertp, startp);
		  else
		    insertp = render_g(np->children.nodes[i], insertp, startp);
		}
	    }
	    if (insertp[-1] == '{')
	      *--insertp = '\0'; /* excised determinative */
	    else
	      *insertp++ = '}';
	  }
	break;
      case 'o':
	if ((uintptr_t)np->user == 'x')
	  {
	    if (!strcmp("repeated", (char*)getAttr(np, "g:type")))
	      insertp = render_g_text(np->children.nodes[0], insertp, startp);
	    insertp += xxstrlen(xstrcpy(insertp, utf8_times()));
	  }
	else
	  *insertp++ = (char)(uintptr_t)np->user;
	break;
      case 'x':
	aval = getAttr(np,"g:type");
	if (!xstrcmp(aval,"ellipsis"))
	  *insertp++ = 'x';
	else if (!xstrcmp(aval,"newline"))
	  {
	    if (insertp_is_delim())
	      *--insertp = '\0'; /* unhyphenate */
	  }
	else if (!xstrcmp(aval,"empty"))
	  *insertp++ = '-';
	else if (!xstrcmp(aval,"disamb"))
	  {
	    if (insertp_is_delim())
	      *--insertp = '\0'; /* unhyphenate */
	    suppress_next_hyphen = 1;
	    insertp = render_g_text(np->children.nodes[0], insertp, startp);
	  }
	break;
      case 'p':
	break;
      case 'w':
	if (np->etype == e_n_word_group)
	  {
	    int i;
	    for (i = 0; i < np->children.lastused; ++i)
	      {
		struct node *n_seg = ((struct node*)np->children.nodes[i])->children.nodes[0];
		if (i)
		  *insertp++ = '/';
		insertp = render_g_text(n_seg->children.nodes[0], insertp, startp);
	      }
	  }
	else
	  insertp = render_g_text(np->children.nodes[0], insertp, startp);
	break;
      default:
	fprintf(stderr,"render_g passed non-grapheme: %s\n",np->names->pname);
	break;
      }
  --depth;
  return insertp;
}

static struct grapheme *
icmt_grapheme(const unsigned char *icmt_tok)
{
  struct grapheme *g = calloc(1,sizeof(struct grapheme));
  g->type = icmt;
  g->g.s.base = pool_copy(icmt_tok);
  return g;
}

const unsigned char *
signify(const unsigned char *utf)
{
  const unsigned char *ret = NULL;
  const unsigned char *tilde;

  if (!utf)
    return NULL;
  
  if (psl_is_value(utf))
    return psl_get_sname(utf);
  else if (cuneify_fuzzy_allographs 
	   && ((tilde = (const unsigned char *)strchr((char*)utf,'~'))))
    {
      const unsigned char *no_allo = de_allograph(utf,tilde);
      const unsigned char *try = signify(no_allo);
      if (try)
	return try;
    }
  if (psl_is_sname(utf))
    {
      return utf;
    }
  else if (psl_looks_like_sname(utf))
    {
      const unsigned char *altname = psl_get_sname(utf_lcase(utf)), *try;
      if (altname && strcmp((char*)altname,(char*)utf))
	{
	  try = signify(altname);
	  if (try)
	    return try;
	}
      else
	altname = utf;
#if 0
      withpipes = addpipes(altname);
      if ((try = psl_cuneify(withpipes)))
	return try;
#endif
    }
  return ret;
}

#ifdef TEST
int
main(int argc, char **argv)
{
  ssize_t retsize;
  size_t bufsize = 128;
  char *line = malloc(bufsize);
  file = "stdin";
  pool_init();
  graphemes_init();
  while (-1 != (retsize = getline(&line,&bufsize,stdin)))
    {
      while (isspace(line[retsize-1]))
	line[--retsize] = '\0';
      fprintf(stderr,"%s\n",line);
      struct grapheme *gp = gparse(line,type_top);
      if (gp && gp->xml)
	{
	  puts(gp->xtf);
	  fflush(stdout);
	}
      ++lnum;
    }
  return 0;
}
#endif
