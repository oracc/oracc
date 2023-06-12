#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype128.h>
#include "mesg.h"
#include "memo.h"
#include "pool.h"
#include "bits.h"
/*#include "lang.h"*/
#include "form.h"

static Memo *formsmem = NULL;

#define Uchar unsigned char

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

static struct pool *form_pool;
void
form_init(void)
{
  form_pool = pool_init();
}

void
form_term(void)
{
  pool_term(form_pool);
}

Uchar *
form_parse_basesig(const Uchar *file, size_t line, Uchar *sig)
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

/* Parse the COF head into the top-level form, then parse tails into
   the form->parts array, and NULL-terminate the array */
void
form_parse_cof(const Uchar *file, size_t line, Uchar *lp, struct form *formp, 
	     Uchar **psu_sense, Uchar *ampamp)
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
  formp->parts = memo_new_array(formsmem, list_len(cofs));

  /* Parse the head in the top-level form structure */
  form = list_first(cofs);
  form_parse(file,line,form,formp,NULL);
  formp->cof_id = (uintptr_t)formp;
  bit_set(formp->flags, FORM_FLAGS_COF_HEAD);

  /* Now parse the tails into the parts array */
  for (i = 0, form = list_next(cofs); 
       form; 
       form = list_next(cofs), ++i)
    {
      formp->parts[i] = memo_new(formsmem);
      form_parse(file,line,form,formp->parts[i],NULL);
      formp->parts[i]->cof_id = (uintptr_t)formp;
      bit_set(formp->parts[i]->flags, FORM_FLAGS_COF_TAIL);
    }
  formp->parts[i] = NULL;
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
	      mesg_vwarning((char*)file,line,
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
   non-negative is the length of string parsed by form_parse.
 */
int
form_parse(const Uchar *file, size_t line, Uchar *lp, struct form *formp, Uchar **psu_sense)
{
  Uchar *err_lp = NULL,
    *disambig = NULL, *ampamp = NULL, 
    *orig_lp = lp, field = '\0', *psu_tmp = NULL, *psu_form = NULL;
  int ret = 0;
  const char *saved_phase = phase;
  char *at1 = NULL, *at2 = NULL;
  int square;

  if (!lp)
    return 1;
  /* err_lp = pool_copy(lp, scp->pool); */
  err_lp = (Uchar*)strdup((char*)lp);

  phase = "form";
    
  /* skip the old shadow lem codes */
  if (*lp == '`')
    {
      mesg_vwarning((char*)file,line,"%s: please remove deprecated shadow lem sequence '`' or '`?'",err_lp);      
      lp += 1 + (lp[1] == '?');
    }

  if (*lp == '$' || *lp == '\\' || *lp == '#' || *lp == '\'')
    {
      mesg_vwarning((char*)file,line,"%s: lemmatization cannot begin with '%c'",err_lp, *lp);
      return -1;
    }
  
  if ((ampamp = (unsigned char*)strstr((char*)lp, "&&")))
    {
      form_parse_cof(file, line, lp, formp, psu_sense, ampamp);
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
      formp->project = lp+1;
      lp = (Uchar*)strchr((char*)lp,'%');
      if (lp)
	{
	  *lp++ = '\0';
	  formp->lang = lp;
	  formp->core = langcore_of((const char*)lp);
	  lp = (Uchar*)strchr((char*)lp,':');
	  if (lp)
	    {
	      *lp++ = '\0';
	      formp->form = lp;
	      lp = (Uchar*)strchr((char*)lp,'=');
	      *lp++ = '\0';
	    }
	  else
	    {
	      mesg_vwarning((char*)file,line,"no FORM found after project");
	      return -1;
	    }
	}
      else
	{
	  mesg_vwarning((char*)file,line,"no LANG found after project");
	  return -1;
	}
    }
  else if (':' == *lp)
    {
      *lp++ = '\0';
      formp->form = lp;
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
      formp->pos = (const Uchar *)"n";
      ++lp;
      if (*lp == '\'')
	{
	  ++lp;
	  formp->epos = lp;
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
      mesg_vwarning((char*)file,line,"%s: lemmatization cannot begin with '['",err_lp);
      goto ret;
    }

  if (!strchr((const char *)lp,'['))
    {
      /* formp->cf = "X"; */
      formp->gw = (unsigned char *)"X";
      if (*lp == 'n')
	{
	  formp->pos = (Uchar*)"n";
	  ++lp;
	}
      else if (*lp == 'u')
	{
	  formp->pos = (Uchar*)"u";
	  ++lp;
	}
      goto pos_parse;
    }

  /* parse the CF[GW/SENSE]POS'EPOS which are constant: */
  formp->cf = lp;
  if (*lp == '"')
    {
      ++lp;
      formp->cf = lp; /* don't include quotes in the CF; 
		       WATCHME: what happens in post-cache
		       retrieval parse?
		     */
      bit_set(formp->flags,FORM_FLAGS_CF_QUOTED);
      /*  fp->explicit |= NEW_CF; */ 
      /* have to do something here, i.e., suppress charset translation */
    }

  while (*lp && (*lp != '[' || lp[-1] == '\\'))
    ++lp;

  if (BIT_ISSET(formp->flags,FORM_FLAGS_CF_QUOTED))
    {
      if (lp[-1] == '"')
	lp[-1] = '\0';
      else
	{
	  mesg_vwarning((char*)file,line,"%s: '\"' missing on quoted CF",err_lp);
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
      char *oparen = strchr((char*)formp->cf,'(');
      if (oparen && oparen[-1] != '\\')
	{
	  *oparen++ = '\0';
	  formp->restrictor = (unsigned char*)oparen;
	  lp[-1] = '\0';
	}
      else
	{
	  mesg_vwarning((char*)file,line,"%s: '(' missing on restrictor",err_lp);
	  ret = -1;
	  goto ret;
	}
    }
  else if ((tmp = (unsigned char*)strchr((char*)formp->cf,'(')) && tmp[-1] != '\\')
    {
      mesg_vwarning((char*)file,line,"%s: ')' missing on restrictor",err_lp);
      ret = -1;
      goto ret;
    }
#endif
  
  formp->gw = ++lp;
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
	  formp->sense = lp;
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
	      formp->pos = lp;
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
		  formp->epos = lp;
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
		  formp->epos = lp;
		  break;
#endif
		case '@':
		  formp->rws = lp;
		  break;
		case '%':
		  formp->lang = lp;
		  break;
		case ':':
		  formp->form = lp;
		  break;
		case '$':
		  if (!BIT_ISSET(formp->flags, FORM_FLAGS_LEM_BY_NORM))
		    formp->norm = lp;
		  /* else ignore normalization because we got it from the "FORM" */
		  break;
		case '/':
		  formp->base = lp;
		  break;
		case '+':
		  if (*lp == '-')
		    formp->cont = lp;
		  else if (*lp == '.')
		    formp->augment = lp;
		  else
		    mesg_vwarning((char*)file,line,"%s: '+' in signature should be followed by '-' or '.'", err_lp);
		  ++lp;
		  break;
		case '#':
		  if (*lp == '#')
		    {
		      ++lp;
		      formp->morph2 = lp;
		    }
		  else
		    formp->morph = lp;
		  break;
		case '*':
		  formp->stem = lp;
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
		  mesg_vwarning((char*)file,line,"%s: parse error at '%c'", err_lp, field);
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
      mesg_vwarning((char*)file,line,"%s: missing ']'", err_lp);
    }

 break_switch_loop:

  /* A RWS may only occur after the POS because the use of @ in transliteration
     could complicate parsing bases otherwise. Handling it here is a hack, but it
     avoids having to add '@' to the field-end chars.
   */
  if ((formp->pos && (at1 = strchr((const char *)formp->pos, '@')))
      || (formp->epos && (at2 = strchr((const char *)formp->epos, '@'))))
    {
      /*fprintf(stderr, "FORM: found RWS in POS\n");*/
      if (at1)
	{
	  formp->pos = pool_copy(formp->pos, form_pool);
	  at1 = strchr((const char *)formp->pos,'@');
	  *at1 = '\0';
	  formp->rws = (const Uchar *)(at1+1);
	}
      if (at2)
	{
	  formp->epos = pool_copy(formp->epos, form_pool);
	  at2 = strchr((const char *)formp->epos,'@');
	  *at2 = '\0';
	  formp->rws = (const Uchar *)(at2+1);
	}
      at1 = at2 = NULL;
    }
		
  if (formp->pos)
    {
      if (*formp->pos)
	validate_pos((const char *)file, line, formp->pos);
      else
	formp->pos = NULL;
    }
  if (formp->epos)
    {
      if (*formp->epos)
	validate_pos((const char *)file, line, formp->epos);
      else
	formp->epos = NULL;
    }

  if (formp->base)
    validate_base((const char *)file, line, formp->base);

#if 0
  /* If lp is non-zero we didn't manage to parse the entire form: */
  if (*lp)
    {
      mesg_vwarning((char*)file,line,"%s: bad tense designator: only allowed with verb POS",err_lp);
      ret = -1;
      goto ret;
    }
#endif

  /* field == '$' occurs when $ is the end of the lem, e.g., ana[to]PRP$ */
  /* FIXME: THIS IS A POOR TEST BECAUSE IT FAILS ON ]N$#M1 */
  if (field == '$')
    {
      if (!formp->norm || !*formp->norm)
	formp->norm = formp->cf;
    }
  
  if (BIT_ISSET(formp->flags, FORM_FLAGS_LEM_BY_NORM))
    {
      if (formp->norm && formp->cf && !strcmp((char*)formp->cf,(char*)formp->norm))
	{
	  bit_set(formp->flags, FORM_FLAGS_NORM_IS_CF);
	  formp->cf = NULL;
	}
    }

  if (formp->gw)
    {
      char *bs = strchr((char*)formp->gw, '\\');
      if (bs)
	{
	  unsigned char *gwtmp = pool_copy(formp->gw, form_pool);
	  bs = (char *)(gwtmp + (bs - (char*)formp->gw));
	  *bs++ = '\0';
	  formp->gw = gwtmp;
	  if (*bs == 'i' || *bs == 't')
	    {
	      if ((!formp->pos || *formp->pos == 'V')
		  && (!formp->epos || *formp->epos == 'V'))
		{
		  if (*bs == 'i')
		    formp->epos = pool_copy((unsigned char *)"V/i",form_pool);
		  else
		    formp->epos = pool_copy((unsigned char *)"V/t",form_pool);
		}
	      else
		{
		  mesg_vwarning((char*)file,line,"%s: bad designator: only allowed with verb POS",err_lp);
		  ret = -1;
		  goto ret;
		}
	    }
	  else
	    {
	      mesg_vwarning((char*)file,line,"%s: bad designator: only 'i' or 't' allowed",err_lp);
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

  if (!formp->gw || !*formp->gw)
    {
      if (formp->sense && *formp->sense)
	formp->gw = formp->sense;
      else
	formp->gw = (unsigned char *)"1";
    }

  clean_cf((char*)file, line, (unsigned char *)formp->cf);

  clean_gw_sense((char*)file, line, (unsigned char *)formp->gw);
  if (formp->sense)
    clean_gw_sense((char*)file, line, (unsigned char *)formp->sense);

#if 0
  /* NO: it's wrong to add augment in the way that we add disambig--the point
     of augment is that it gets added to the morphology not the FORM
     FIXME: should print it to MORPH on output
   */
  if (formp->augment)
    {      
      char buf[1024];
      sprintf(buf,"%s%s%s",formp->form,AUGMENT_STR,formp->augment);
      formp->form = pool_copy((unsigned char *)buf,form_pool);
    }
#endif

  if (disambig)
    {
      char buf[1024];
      if (*disambig == *(DISAMBIG_STR))
	sprintf(buf,"%s%s",formp->form,disambig);
      else
	sprintf(buf,"%s%s%s",formp->form,DISAMBIG_STR,disambig);
      formp->form = pool_copy((unsigned char *)buf,form_pool);
    }

  if (formp->cf && strchr((char*)formp->cf,' '))
    bit_set(formp->flags, FORM_FLAGS_IS_PSU);
  phase = saved_phase;

  return (ret < 0) ? -1 : (lp - orig_lp);
}

static Uchar *
field_end(Uchar *lp)
{
  while (*lp)
    {
      size_t n = strcspn((char*)lp,FORM_FIELD_ENDS_STR); /*'*/
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
