#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype128.h>
#include <psdtypes.h>
#include "warning.h"
#include "memblock.h"
#include "npool.h"
#include "lang.h"
#include "atf.h"
#include "f2.h"

#if 1
#define AUGMENT_STR "\\"
#define DISAMBIG_STR "\\"
#else
#define AUGMENT_STR "±"
#define DISAMBIG_STR "∖"
#endif

#ifndef strdup
extern char *strdup(const char *);
#endif

static Uchar *field_end(Uchar *lp);

#define cc(x) ((const char *)(x))

/* It's hard to do really complete validation, but we catch a few
 * obvious things like probably malformatted CONT and use of the
 * CF for BASE in qpn
 */
static void
validate_base(const char *file, size_t line, const Uchar *p)
{
  const Uchar *b = p;
  int has_non_ascii = 0;
  if (p && !strlen((const char *)p))
    {
      vwarning2(file, line, "empty BASE in Sumerian lemmatization");
    }
  else if (p && file && strcmp(file, "cache"))
    {
      if (isupper(p[0]) && islower(p[1]))
	vwarning2((const char *)file, line, "%s: mixed case in BASE", p);
      ++p;
      while (*p)
	{
	  if ('+' == *p && '{' != p[-1] 
	      && '-' != p[1] && '.' != p[1])
	    {
	      while (*p && *p != '-' && *p != '.' && *p != '{' && *p != '}' && *p != '=')
		++p;
	      if ('=' == *p)
		{
		  vwarning2(file, line, "%s: probable malformed CONT; say, e.g., +-ga=g.a", b);
		  break;
		}
	    }
	  if (*p > 128)
	    ++has_non_ascii;
	  ++p;
	}
      if (hasacc(b))
	vwarning2(file,line,"%s: accented characters not allowed in BASE",b);
    }
}

static void
validate_pos(const char *file, size_t line, const Uchar *p)
{
  const Uchar *orig = p;
  if (!p)
    return;
  if (*p == 'n' || *p == 'u')
    ++p;
  else
    {
      while (isupper(*p))
	++p;
      if ('/' == *p)
	{
	  ++p;
	  if ('i' == *p || 't' == *p)
	    ++p;
	}
    }
  if (*p)
    vwarning2(file,line,"%s: bad POS; allowed forms are 'u', 'n', or A-Z+ then optional /i or /t",orig);
}

static struct npool *f2_pool;
void
f2_init(void)
{
  f2_pool = npool_init();
}

void
f2_term(void)
{
  npool_term(f2_pool);
}

Uchar *
f2_parse_basesig(const Uchar *file, size_t line, Uchar *sig)
{
  Uchar *pivot = (Uchar*)strstr((const char *)sig, "//"), *start, *end;
  Uchar *ret = NULL, *retp = NULL;
  for (start = pivot; start[-1] != '='; --start)
    ;
  for (end = pivot; end[1] != '\''; ++end)
    ;
  ret = retp = malloc(end - pivot);
  while (start[0] != '/' || start[1] != '/')
    *retp++ = *start++;
  while (']' != *start)
    ++start;
  while (start < end)
    *retp++ = *start++;
  *retp = '\0';
  return ret;
}

/* Parse the COF head into the top-level f2, then parse tails into
   the f2->parts array, and NULL-terminate the array */
void
f2_parse_cof(const Uchar *file, size_t line, Uchar *lp, struct f2 *f2p, 
	     Uchar **psu_sense, Uchar *ampamp, struct sig_context *scp)
{
  List *cofs = list_create(LIST_SINGLE);
  Uchar *form = NULL;
  int i = 0;

  while (1)
    {
      list_add(cofs, lp);
      if (!ampamp)
	break;
      *ampamp = '\0';
      ampamp += 2;
      lp = ampamp;
      ampamp = (unsigned char *)strstr((char*)lp,"&&");
    }
  f2p->parts = mb_new_array(scp->mb_f2ps, list_len(cofs));

  /* Parse the head in the top-level f2 structure */
  form = list_first(cofs);
  f2_parse(file,line,form,f2p,NULL,NULL);
  f2p->cof_id = (uintptr_t)f2p;
  BIT_SET(f2p->flags, F2_FLAGS_COF_HEAD);

  /* Now parse the tails into the parts array */
  for (i = 0, form = list_next(cofs); 
       form; 
       form = list_next(cofs), ++i)
    {
      f2p->parts[i] = mb_new(scp->mb_f2s);
      f2_parse(file,line,form,f2p->parts[i],NULL,NULL);
      f2p->parts[i]->cof_id = (uintptr_t)f2p;
      BIT_SET(f2p->parts[i]->flags, F2_FLAGS_COF_TAIL);
    }
  f2p->parts[i] = NULL;
}

static void
clean_cf(const char *file, int line, Uchar *s)
{
  if (!s)
    return;
  if (strchr((const char*)s, '&'))
    {
      Uchar *dst = s;
      while (*s)
	{
	  if (s[0] == '\\' && s[1] == '&')
	    ++s;
	  *dst++ = *s++;
	}
      *dst = '\0';
    }
}

static void
clean_gw_sense(const char *file, int line, Uchar *s)
{
  const Uchar *orig_s = s;
  if (strcspn((const char*)s, "\"[]") != strlen((const char*)s))
    {
      Uchar *dst = s;
      while (*s)
	{
	  if (*s == '"' || *s == '[' || *s == ']')
	    {
	      vwarning2((char*)file,line,
			"%s: dropping character '%c' which is not allowed in GW/SENSE", orig_s, *s);
	      ++s;
	    }
	  else
	    *dst++ = *s++;
	}
      *dst = '\0';
    }
}

/* return non-negative on success; -1 on error; 
   non-negative is the length of string parsed by f2_parse.
 */
int
f2_parse(const Uchar *file, size_t line, Uchar *lp, struct f2 *f2p, Uchar **psu_sense, struct sig_context *scp)
{
  Uchar *err_lp = NULL,
    *disambig = NULL, *ampamp = NULL, 
    *orig_lp = lp, field = '\0', *psu_tmp = NULL, *psu_form = NULL;
  int ret = 0;
  const char *saved_phase = phase;
  int square, saved_with_textid = with_textid;

  if (!lp)
    return 1;
  /* err_lp = npool_copy(lp, scp->pool); */
  err_lp = (Uchar*)strdup((char*)lp);

  phase = "f2";
  with_textid = 0;
  
  /* skip the old shadow lem codes */
  if (*lp == '`')
    {
      vwarning2((char*)file,line,"%s: please remove deprecated shadow lem sequence '`' or '`?'",err_lp);      
      lp += 1 + (lp[1] == '?');
    }

  if (*lp == '$' || *lp == '\\' || *lp == '#' || *lp == '\'')
    {
      vwarning2((char*)file,line,"%s: lemmatization cannot begin with '%c'",err_lp, *lp);
      return 1;
    }
  
  if ((ampamp = (unsigned char*)strstr((char*)lp, "&&")))
    {
      f2_parse_cof(file, line, lp, f2p, psu_sense, ampamp, scp);
      goto ret;
    }

  /* if the sig starts with @ parse the admin fields @PROJ%LANG:FORM=
   * first.
   *
   * N.B.: % and : are not recognized by field_end() as this causes
   * problems parsing morphology.
   */
  if ('@' == *lp)
    {
      f2p->project = lp+1;
      lp = (Uchar*)strchr((char*)lp,'%');
      if (lp)
	{
	  *lp++ = '\0';
	  f2p->lang = lp;
	  f2p->core = langcore_of((const char*)lp);
	  lp = (Uchar*)strchr((char*)lp,':');
	  if (lp)
	    {
	      *lp++ = '\0';
	      f2p->form = lp;
	      lp = (Uchar*)strchr((char*)lp,'=');
	      *lp++ = '\0';
	    }
	}
    }
  else if (':' == *lp)
    {
      *lp++ = '\0';
      f2p->form = lp;
      lp = (Uchar*)strchr((char*)lp,'=');
      *lp++ = '\0';
    }

  /* pass over the new PSU form and remember where it is; we don't do
     anything with it here for now but that should probably change
   */
  if ('{' == *lp)
    {
      char * tmp = strstr((char *)lp, " += ");
      if (tmp) {
	char *tmp2;
	*tmp = '\0';
	tmp2 = strstr((char *)lp, " = ");
	if (tmp2) {
	  psu_form = &lp[1];
	  *tmp2 = '\0';
	  lp = (Uchar *)(tmp2 + 3);
	  *tmp = ' ';
	}
      }      
    }

  /* Get numbers early */
  if ('n' == *lp && !strchr((const char *)lp,'['))
    {
      f2p->pos = (const Uchar *)"n";
      ++lp;
      if (*lp == '\'')
	{
	  ++lp;
	  f2p->epos = lp;
	  while (*lp && !isspace(*lp))
	    ++lp;
	  *lp = '\0';
	}
      free(err_lp);
      return lp - orig_lp;
    }
  
  if ('[' == *lp)
    {
      /* FIXME: this needs to be more rigorous and check for CF-legal char in initial position */
      vwarning2((char*)file,line,"%s: lemmatization cannot begin with '['",err_lp);
      goto ret;
    }

  if (!strchr((const char *)lp,'['))
    {
      /* f2p->cf = "X"; */
      f2p->gw = (unsigned char *)"X";
      if (*lp == 'n')
	{
	  f2p->pos = (Uchar*)"n";
	  ++lp;
	}
      else if (*lp == 'u')
	{
	  f2p->pos = (Uchar*)"u";
	  ++lp;
	}
      goto pos_parse;
    }

  /* parse the CF[GW/SENSE]POS'EPOS which are constant: */
  f2p->cf = lp;
  if (*lp == '"')
    {
      ++lp;
      f2p->cf = lp; /* don't include quotes in the CF; 
		       WATCHME: what happens in post-cache
		       retrieval parse?
		     */
      BIT_SET(f2p->flags,F2_FLAGS_CF_QUOTED);
      /*  fp->explicit |= NEW_CF; */ 
      /* have to do something here, i.e., suppress charset translation */
    }

  while (*lp && (*lp != '[' || lp[-1] == '\\'))
    ++lp;

  if (BIT_ISSET(f2p->flags,F2_FLAGS_CF_QUOTED))
    {
      if (lp[-1] == '"')
	lp[-1] = '\0';
      else
	{
	  vwarning2((char*)file,line,"%s: '\"' missing on quoted CF",err_lp);
	  ret = -1;
	}
    }

  if (lp)
    *lp = '\0';
  else
    goto ret;

#if 0
  /* The RESTRICTOR feature was hardly used (two times in GKAB) and not really suited to task.
     Now that stems are implemented (see CMAWRO) they can be used instead */
  if (lp[-1] == ')' && lp[-2] != '\\')
    {
      char *oparen = strchr((char*)f2p->cf,'(');
      if (oparen && oparen[-1] != '\\')
	{
	  *oparen++ = '\0';
	  f2p->restrictor = (unsigned char*)oparen;
	  lp[-1] = '\0';
	}
      else
	{
	  vwarning2((char*)file,line,"%s: '(' missing on restrictor",err_lp);
	  ret = -1;
	  goto ret;
	}
    }
  else if ((tmp = (unsigned char*)strchr((char*)f2p->cf,'(')) && tmp[-1] != '\\')
    {
      vwarning2((char*)file,line,"%s: ')' missing on restrictor",err_lp);
      ret = -1;
      goto ret;
    }
#endif
  
  f2p->gw = ++lp;
  if ((psu_tmp = (Uchar *)strstr(cc(lp),"+=")))
    {
      *psu_tmp = '\0';
      psu_tmp += 2;
      if (psu_sense)
	*psu_sense = psu_tmp;
      psu_tmp = (Uchar *)strchr((const char *)psu_tmp,']');
    }

  /* make SENSE optional here to support inline lem parsing */
  square = 0;
  while (*lp && (*lp != '/' || lp[1] != '/'))
    {
      if (*lp == '[' && lp[-1] != '\\')
	++square;
      else if (*lp == ']' && lp[-1] != '\\')
	{
	  if (square)
	    --square;
	  else
	    break;
	}
      ++lp;
    }

  /* If we didn't find ] but had a psu_sense with +=,
     reset lp to the closing square bracket after the
     psu_sense */
  if (!*lp && psu_tmp)
    lp = psu_tmp;

  if (*lp)
    {
      if ('/' == *lp)
	{
	  *lp++ = '\0';
	  ++lp;
	  f2p->sense = lp;
	  square = 0;
	  while (*lp)
	    {
	      if (*lp == '[' && lp[-1] != '\\')
		++square;
	      else if (*lp == ']' && lp[-1] != '\\')
		{
		  if (square)
		    --square;
		  else
		    break;
		}
	      ++lp;
	    }
	}
      if (*lp) /* lp is at closing square bracket of CF[GW] */
	{
	  *lp++ = '\0';
	  
	  /* This is either a POS or something
	     that starts with a field char */

	pos_parse:
	  if (isupper(*lp) || '\'' == *lp || 'n' == *lp)
	    {
	      f2p->pos = lp;
	      if (*lp == 'V' && '/' == lp[1] && (lp[2] == 't' || lp[2] == 'i'))
		{
		  lp += 3;
		}
	      else
		{
		  unsigned char *e = field_end(lp), *tmp = lp;
		  while (tmp < e)
		    {
		      if ('\'' == *tmp)
			break;
		      else
			++tmp;
		    }
		  if ('\'' == *tmp)
		    lp = tmp;
		  else
		    lp = e;
		}
	      if (*lp == '\'')
		{
		  *lp++ = '\0';
		  f2p->epos = lp;
		  if (*lp == 'V' && '/' == lp[1] && (lp[2] == 't' || lp[2] == 'i'))
		    lp += 3;
		  lp = field_end(lp);
		}
	      else
		lp = field_end(lp);
	    }

	  if (*lp)
	    {
	      field = *lp;
#if 0
	      if ('<' != *lp) /* leave < because it will exit loop below and return len for ngram to process preds */
#endif
		*lp++ = '\0';
	    }

	  /* Now we are at a (possibly empty) variable set of instance
	     fields; parse as though they can be in any order, though
	     in principle the order should always be fixed. */
	  while (*lp)
	    {
	      switch (field)
		{
#if 0
		  /* this must follow POS and ' is no longer a field
		   * ender because of conflict with ' in M1
		   */
		case '\'':
		  f2p->epos = lp;
		  break;
#endif
		case '@':
		  f2p->rws = lp;
		  break;
		case '%':
		  f2p->lang = lp;
		  break;
		case ':':
		  f2p->form = lp;
		  break;
		case '$':
		  if (!BIT_ISSET(f2p->flags, F2_FLAGS_LEM_BY_NORM))
		    f2p->norm = lp;
		  /* else ignore normalization because we got it from the "FORM" */
		  break;
		case '/':
		  f2p->base = lp;
		  break;
		case '+':
		  if (*lp == '-')
		    f2p->cont = lp;
		  else if (*lp == '.')
		    f2p->augment = lp;
		  else
		    vwarning2((char*)file,line,"%s: '+' in signature should be followed by '-' or '.'", err_lp);
		  ++lp;
		  break;
		case '#':
		  if (*lp == '#')
		    {
		      ++lp;
		      f2p->morph2 = lp;
		    }
		  else
		    f2p->morph = lp;
		  break;
		case '*':
		  f2p->stem = lp;
		  break;
		case '\\':
		  disambig = lp;
		  while (isalnum(*lp) || '\\' == *lp)
		    ++lp;
		  break;
		case '<':
		case ' ':
		case '\t':
		case 0:
		  goto break_switch_loop;
		default:
		  vwarning2((char*)file,line,"%s: parse error at '%c'", err_lp, field);
		  ++ret;
		  goto ret;
		}
	      lp = field_end(lp);
	      if (*lp)
		{
		  field = *lp;
		  *lp++ = '\0';
		}
	    }
	}
    }
  else
    {
      vwarning2((char*)file,line,"%s: missing ']'", err_lp);
    }

 break_switch_loop:

  if (f2p->pos)
    {
      if (*f2p->pos)
	validate_pos((const char *)file, line, f2p->pos);
      else
	f2p->pos = NULL;
    }
  if (f2p->epos)
    {
      if (*f2p->epos)
	validate_pos((const char *)file, line, f2p->epos);
      else
	f2p->epos = NULL;
    }

  if (f2p->base)
    validate_base((const char *)file, line, f2p->base);

#if 0
  /* If lp is non-zero we didn't manage to parse the entire form: */
  if (*lp)
    {
      vwarning2((char*)file,line,"%s: bad tense designator: only allowed with verb POS",err_lp);
      ret = -1;
      goto ret;
    }
#endif

  /* field == '$' occurs when $ is the end of the lem, e.g., ana[to]PRP$ */
  /* FIXME: THIS IS A POOR TEST BECAUSE IT FAILS ON ]N$#M1 */
  if (field == '$')
    {
      if (!f2p->norm || !*f2p->norm)
	f2p->norm = f2p->cf;
    }
  
  if (BIT_ISSET(f2p->flags, F2_FLAGS_LEM_BY_NORM))
    {
      if (f2p->norm && f2p->cf && !strcmp((char*)f2p->cf,(char*)f2p->norm))
	{
	  BIT_SET(f2p->flags, F2_FLAGS_NORM_IS_CF);
	  f2p->cf = NULL;
	}
    }

  if (f2p->gw)
    {
      char *bs = strchr((char*)f2p->gw, '\\');
      if (bs)
	{
	  unsigned char *gwtmp = npool_copy(f2p->gw, f2_pool);
	  bs = (char *)(gwtmp + (bs - (char*)f2p->gw));
	  *bs++ = '\0';
	  f2p->gw = gwtmp;
	  if (*bs == 'i' || *bs == 't')
	    {
	      if ((!f2p->pos || *f2p->pos == 'V')
		  && (!f2p->epos || *f2p->epos == 'V'))
		{
		  if (*bs == 'i')
		    f2p->epos = npool_copy((unsigned char *)"V/i",f2_pool);
		  else
		    f2p->epos = npool_copy((unsigned char *)"V/t",f2_pool);
		}
	      else
		{
		  vwarning2((char*)file,line,"%s: bad designator: only allowed with verb POS",err_lp);
		  ret = -1;
		  goto ret;
		}
	    }
	  else
	    {
	      vwarning2((char*)file,line,"%s: bad designator: only 'i' or 't' allowed",err_lp);
	      ret = -1;
	      goto ret;
	    }
	}
    }
 
 ret:
  free(err_lp);

  if (isspace(*lp))
    *lp++ = '\0';
  else if (field == '<')
    *--lp = '<';

  if (!f2p->gw || !*f2p->gw)
    {
      if (f2p->sense && *f2p->sense)
	f2p->gw = f2p->sense;
      else
	f2p->gw = (unsigned char *)"1";
    }

  clean_cf((char*)file, line, (unsigned char *)f2p->cf);

  clean_gw_sense((char*)file, line, (unsigned char *)f2p->gw);
  if (f2p->sense)
    clean_gw_sense((char*)file, line, (unsigned char *)f2p->sense);

#if 0
  /* NO: it's wrong to add augment in the way that we add disambig--the point
     of augment is that it gets added to the morphology not the FORM
     FIXME: should print it to MORPH on output
   */
  if (f2p->augment)
    {      
      char buf[1024];
      sprintf(buf,"%s%s%s",f2p->form,AUGMENT_STR,f2p->augment);
      f2p->form = npool_copy((unsigned char *)buf,f2_pool);
    }
#endif

  if (disambig)
    {
      char buf[1024];
      if (*disambig == *(DISAMBIG_STR))
	sprintf(buf,"%s%s",f2p->form,disambig);
      else
	sprintf(buf,"%s%s%s",f2p->form,DISAMBIG_STR,disambig);
      f2p->form = npool_copy((unsigned char *)buf,f2_pool);
    }

  if (f2p->cf && strchr((char*)f2p->cf,' '))
    BIT_SET(f2p->flags, F2_FLAGS_IS_PSU);
  phase = saved_phase;
  with_textid = saved_with_textid;

  return (ret < 0) ? -1 : (lp - orig_lp);
}

static Uchar *
field_end(Uchar *lp)
{
  while (*lp)
    {
      size_t n = strcspn((char*)lp,F2_FIELD_ENDS_STR); /*'*/
      Uchar *tmp = lp + n;
      if (!*tmp || '<' == *tmp || isspace(*tmp)) /* end is terminating null or '<' */
	return tmp;
      /* FIXME: This could be coded more efficiently but be careful only
	 to take + followed by - as field_end */
      if (*tmp == '+')
	{
	  if (tmp[1] == '-' || tmp[1] == '.')
	    return tmp;
	}
      else if (*tmp == '*')
	{
	  switch (tmp[-1])
	    {
	    case '.':
	    case ';':
	    case '!':
	    case ',':
	      break;
	    default:
	      return tmp;
	    }
	}
      else if (*tmp != '\\'
	       || (tmp[1] != '&' && tmp[1] != '(' && tmp[1] != '|'))
	/* char is not an escape; end is the special char */
	return tmp;
      lp = tmp+1; /* char was escaped; look for next instance */
    }
  return lp;
}
