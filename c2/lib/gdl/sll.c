#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <wchar.h>
#include <wctype.h>
#include <locale.h>

#include <oraccsys.h>
#include <pool.h>
#include "sll.h"

#define ccp const char *
#define ucp unsigned char *
#define uccp unsigned const char *

static const char *oracc = NULL;

static Pool *sllpool = NULL;

static Hash *h = NULL;

static int sll_trace = 0;

static int signindicator[256];

static List *kstrip;

/* Every sign name has at least one of these uppercase letters--this
   is validated by sl-xml */
void
sl_init_si(void)
{
  if (!signindicator['A'])
    signindicator['A'] = 
      signindicator['E'] = 
      signindicator['I'] = 
      signindicator['U'] = 
      signindicator['F'] = 
      signindicator['N'] = 
      signindicator['O'] = 
      signindicator['P'] = 
      signindicator['S'] = 
      signindicator['X'] = 1;
}

unsigned char *
sl_strip_pp(unsigned const char *g)
{
  unsigned char *no_p = (ucp)strdup((ccp)g), *end;
  unsigned const char *orig = g;
  end = no_p;
  while (*g)
    if ('+' == *g)
      *end++ = '.', ++g;
    else if ('(' != *g && ')' != *g)
      *end++ = *g++;
    else if ('(' == *g)
      {
	if (g > orig && (isdigit(g[-1]) || 'n' == g[-1] || 'N' == g[-1]))
	  {
	    /* copy the contents of this (...) including the '(' and
	       ')' because they belong to a number grapheme */
	    int nesting = 0;
	    while (*g)
	      {
		*end++ = *g++;
		if (')' == *g)
		  {
		    if (nesting)
		      --nesting;
		    else
		      {
			*end++ = *g++;
			break;
		      }
		  }
		else if ('(' == *g)
		  ++nesting;
	      }
	  }
	else
	  ++g;
      }
    else
      ++g;
  *end = '\0';
#if 0
  if (strcmp((const char*)no_p,(const char*)orig))
    fprintf(stderr, "sl_strip_pp %s => %s\n", (const char *)orig, (const char *)no_p);
#endif
  return no_p;
}

int
sl_has_sign_indicator(unsigned const char *g)
{
  while (*g)
    {
      if (signindicator[*g])
	return 1;
      ++g;
    }
  return 0;
}

static void
sl_kstrip(void *k)
{
  unsigned char *ks = sl_strip_pp((unsigned char *)k);
  if (strcmp((const char *)ks,(const char *)k))
    {
      unsigned char *kh = hash_find(h,ks);
      if (kh)
	; /* fprintf(stderr, "sl_init_h: stripped key %s=>%s duplicates known key with OID %s\n",
	     (const char*)k, (const char*)ks, (const char*)kh);*/
      else
	{
	  kh = hash_find(h,k);
	  if (kh)
	    {
	      if (sll_trace)
		fprintf(stderr, "adding stripped version %s for %s with OID %s\n",
			(const char *)ks, (const char *)k, (const char *)kh);
	      hash_add(h,pool_copy(ks, sllpool),kh);
	    }
	  else
	    fprintf(stderr, "sll: internal error: no OID for key %s stripped to %s\n", (const char *)k, ks);
	}
    }
  free(ks);
}

Hash *
sl_init(const char *project, const char *name)
{
  char *tsv_file;
  unsigned char *tsv_data = NULL, *p;
  ssize_t fsiz;

  sl_init_si();
  sllpool = NULL;

  /* Figure out the db and open it */
  if (!project)
    project = "ogsl";

  if (!name)
    name = "ogsl";

  oracc = oracc_home();
  tsv_file = malloc(strlen(oracc)+strlen("/pub/sl/") + strlen(project) + strlen("/sl.tsv") + 1);
  sprintf(tsv_file, "%s/pub/%s/sl/sl.tsv", oracc, project);

  tsv_data = slurp("sll", tsv_file, &fsiz);
  if (tsv_data)
    {
      sllpool = pool_init();
  
      if (sll_trace)
	fprintf(stderr, "sll: slurped %s\n", tsv_file);
      h = hash_create(1024);
      kstrip = list_create(LIST_SINGLE);
      for (p = tsv_data; *p; )
	{
	  unsigned char *k = p, *v = NULL;
	  while (*p && '\t' != *p)
	    ++p;
	  if (*p)
	    {
	      *p++ = '\0';
	      v = p;
	      while (*p && '\n' != *p)
		++p;
	      if ('\n' == *p)
		*p++ = '\0';
	      if (v)
		{
		  if (sll_trace)
		    fprintf(stderr, "sll: adding %s = %s\n", k, v);
		  hash_add(h, k, v);
		  if ('o' == *v && isdigit(v[1]))
		    {
		      if (!strchr((ccp)k,';') && sl_has_sign_indicator(k))
			{
			  unsigned char *k2 = hash_find(h,v);
			  if (k2)
			    fprintf(stderr, "sl_init_h: duplicate key/val %s = %s\n", k, v);
			  else
			    {
			      if (sll_trace)
				fprintf(stderr, "sll: adding %s = %s\n", v, k);
			      hash_add(h,v,k);
			    }
			  if (strpbrk((const char *)k,"()+"))
			    list_add(kstrip, k);
			}
		    }
		}
	    }
	}
      list_exec(kstrip, (list_exec_func*)sl_kstrip);
      list_free(kstrip,NULL);
    }
  else
    {
      fprintf(stderr, "sl_init_h: failed to load %s\n", tsv_file);
    }

  return h;
}

void
sl_term(Hash *h)
{
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
sll_tmp_key(unsigned const char *key, const char *field)
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

/* sl.tsv encodings homophones as an OID/INDEX pair; this routine
   */

unsigned char *
sll_v_from_h(const unsigned char *b, const unsigned char *qsub)
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

unsigned const char *
sll_try_h(unsigned const char *g)
{
  unsigned char *b = gvl_val_base(g);
  unsigned const char *h = sll_lookup(sll_tmp_key(b,"h"));
  unsigned const char *p = NULL;
  if (h)
    {
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
	      p = free1 = sll_v_from_h((uccp)b, (uccp)p_slash+1);
	      if (!p)
		{
		  fprintf(stderr, "sll: internal error in data: sll_from_h failed on %s\n", p_slash);
		  p = (ucp)"(null)";
		}
	    }
	  else
	    p = b;
	}
    }
  if (free2)
    free(free2);
  return p;
}
