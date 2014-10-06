#include <ctype128.h>
#include "warning.h"
#include "hash.h"
#include "runexpat.h"
#include "atf.h"
#include "charsets.h"
#include "cdf.h"
#include "gsl.h"
#include "xpd2.h"
#include "cuneify.h"
#include "sexify.h"

#ifndef strdup
#define strdup(a) (unsigned char *)strcpy(malloc(strlen((const char *)a)+1),(const char *)a)
#endif

int cuneify_fuzzy_allographs = 1;
int cuneify_notices = 0;
int do_cuneify = 0;
#if 0
static const unsigned char *Xvalue = ucc("\xe2\x96\x91\xe2\x96\x91");
#endif

static const unsigned char *cuneify_sequence(const unsigned char *seq);
static const unsigned char *split_compound(const unsigned char *c);

static const char *Qret = "???";
struct render_tab
{
  const char *key;
  const char *val;
} rtab[] = 
  {
    { "1(IKU)","diš" },
    { "1/3","1/3(diš)" },
    { "1/2","1/2(diš)" },
    { "2/3","2/3(diš)" },
    { "5/6","2/3(diš)" },
    { "6(ban₂)","1(diš)" },
    { "7(ban₂)","1(diš) 1(ban₂)" },
    { "8(ban₂)","1(diš) 2(ban₂)" },
    { "9(ban₂)","1(diš) 3(ban₂)" },
    { "KA×KIB","gasa" },
    { "|KA×KIB|","gasa" },
    { ":\"", "P₃" },
    { ":&quot;", "P₃" },
    { "n", "x" },
    { NULL , NULL },
  };

static const unsigned char *
addpipes(const unsigned char *u)
{
  static unsigned char buf[32];
  if (strlen((char*)u) > 29)
    return NULL;
  *buf = '|';
  strcpy((char*)buf+1,(char*)u);
  strcat((char*)buf,"|");
  return buf;
}

static int
all_digits(const char *str)
{
  if (!str || !*str)
    return 0;
  while (isdigit(*str))
    ++str;
  return !*str || '~' == *str;
}

const unsigned char *
de_allograph(const unsigned char *u,const unsigned char *tilde)
{
  static unsigned char buf[32];
  if (tilde - u < 32)
    {
      strncpy((char*)buf,(char*)u,tilde-u);
      buf[tilde-u] = '\0';
      return buf;
    }
  else
    return NULL;
}

static const unsigned char *
find_in_rtab(unsigned const char *key)
{
  int i;
  for (i = 0; rtab[i].key; ++i)
    if (!strcmp((const char *)key,rtab[i].key))
      return (unsigned const char *)rtab[i].val;
  return NULL;
}

static const unsigned char *
hack_decimal(const unsigned char *utf)
{
  int n;
  const unsigned char *tilde;
  unsigned char *ret, *retp;

  if ((tilde = (const unsigned char *)strchr((char*)utf,'~')))
    n = atoi((char*)de_allograph(utf,tilde));
  else
    n = atoi((char*)utf);
  
  ret = sexify(n,"disz");
  if (ret)
    {
      for (retp = ret; *retp; ++retp)
	if (strchr("{}",*retp))
	  *retp = ' ';
    }
  return ret;
}

const unsigned char *
cuneify_one(const unsigned char *utf)
{
  const unsigned char *rtab_equiv;
  const unsigned char *tilde;

  if (psl_cuneify(utf))
    return psl_cuneify(utf);

  if (psl_is_value(utf))
    return psl_cuneify(psl_get_sname(utf));
  else if (cuneify_fuzzy_allographs 
	   && ((tilde = (const unsigned char *)strchr((char*)utf,'~'))))
    {
      const unsigned char *no_allo = de_allograph(utf,tilde);
      const unsigned char *try = cuneify_one(no_allo);
      if (try)
	return try;
    }
  if (psl_is_sname(utf))
    {
      unsigned const char *c = psl_cuneify(utf);
      return c ? c : (const unsigned char *)"X";
    }
  else if (psl_looks_like_sname(utf))
    {
      const unsigned char *altname = psl_get_sname(utf_lcase(utf)), *try, *withpipes;
      if (altname && strcmp((char*)altname,(char*)utf))
	{
	  try = cuneify(altname);
	  if (try)
	    return try;
	}
      else
	altname = utf;
      withpipes = addpipes(altname);
      if ((try = psl_cuneify(withpipes)))
	return try;
    }
  if ((rtab_equiv = find_in_rtab(utf)))
    return cuneify_sequence(rtab_equiv);
  else if (all_digits((const char *)utf))
    return cuneify_sequence(hack_decimal(utf));

  return NULL;
}

const unsigned char *
cuneify(const unsigned char *utf)
{
  const unsigned char *ret = NULL;

  if (!utf)
    return NULL;
  if ((ret = cuneify_one(utf)) && strcmp(ret, "X"))
    return ret;
  if ('|' == *utf && (ret = cuneify_sequence(split_compound(utf))))
    return ret;
  else
    {
      if (cuneify_notices)
	vnotice("cuneify failed on %s", utf);
      return (const unsigned char *)utf;
    }
}

static const unsigned char *
cuneify_sequence(const unsigned char *seq)
{
  unsigned char rtab_buf[128],*rtab_bufp = rtab_buf;
  const unsigned char *rtabp = seq, *ret = NULL;
  unsigned char save = '\0';
  /*static*/ unsigned char ret_buf[1024];

  memset(rtab_buf,'\0',128);
  memset(ret_buf,'\0',1024);

  if (!seq)
    return NULL;

  for (*ret_buf = '\0'; *rtabp; (void)(*rtabp && ++rtabp))
    {
      *rtab_buf = '\0';
      rtab_bufp = rtab_buf;
      while (' ' == *rtabp)
	++rtabp;
      while (*rtabp && ' ' != *rtabp)
	*rtab_bufp++ = *rtabp++;
      if (*rtab_bufp)
	{
	  save = ' ';
	  *rtab_bufp = '\0';
	}
      else
	save = '\0';
      ret = cuneify_one(rtab_buf);
      if (ret)
	{
	  xstrcat(ret_buf,ret);
	  if (save)
	    *rtab_bufp = save;
	}
      else
	{
	  if (cuneify_notices)
	    vnotice("cuneify failed trying to render %s in %s", rtab_buf, seq);
	  return (const unsigned char *)Qret;
	}
    }
  return ret_buf;
}

static const unsigned char *
split_compound(const unsigned char *c)
{
  static unsigned char cbuf[128], *cbufp;
  if (strlen((const char*)c) > 127)
    {
      vnotice("cuneify can't split compounds longer than 127 characters\n");
      vnotice("%s",c);
      return NULL;
    }
  strcpy((char*)cbuf,(char*)c);
  for (cbufp = cbuf; *cbufp; ++cbufp)
    {
      if ('|' == *cbufp)
	*cbufp = ' ';
      else if ('(' == *cbufp && cbufp > cbuf && !isdigit(cbufp[-1]) && cbufp[-1] != 'n')
	{
	  *cbufp = ' ';
	  while (*cbufp && ')' != *cbufp)
	    ++cbufp;
	  if (*cbufp)
	    *cbufp = ' ';
	}
      else if ('.' == *cbufp || '+' == *cbufp || ':' == *cbufp)
	*cbufp = ' ';
    }
  return cbuf;
}

void
cuneify_init(struct xpd *xp)
{
  const char *p = xpd_option(xp, "atf-cuneify-notices");
  if (p)
    cuneify_notices = atoi(p);
}

void
cuneify_term(void)
{
}

