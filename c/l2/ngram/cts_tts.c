#include <string.h>
#include <ctype128.h>
#include "list.h"
#include "memblock.h"
#include "ngram.h"
#include "props.h"
#include "warning.h"

static char *
parse_angled_preds(struct CF *cfp, int tts_mode, char *s)
{
  List *preds = list_create(LIST_SINGLE);
  struct PRED *predp;
  char *anglec = NULL;

  while (*s && '<' == *s)
    {
      predp = new_pred(preds);
      predp->owner = cfp;
      ++s;
      if ('!' == *s)
	{
	  predp->neg = 1;
	  ++s;
	}
      if ((anglec = strchr(s,'>')))
	{
	  char *e = NULL;
	  if ((e = strchr(s,'=')) && e < anglec)
	    {
	      predp->key = s;
	      s = e;
	      *s++ = '\0';
	      predp->value = s;
	    }
	  else
	    {
	      predp->key = s;
	      predp->value = "";
	    }
	  *anglec = '\0';
	  s = ++anglec;
	}
      else
	{
	  fprintf(stderr, "no close >\n"); /* FIXME */
	  return ++s;
	}
    }

  cfp->preds = (struct PRED**)list2array(preds);
  list_free(preds, NULL);
  return s;
}

static char *
parse_wild_cf(struct CF *cfp, int tts_mode, char *s)
{
  cfp->cf = "*";
  if (tts_mode)
    {
      cfp->wild = 1;
      ++s;
    }
  else
    {
      ++s;
      if (isdigit(*s))
	{
	  cfp->wild = atoi(s);
	  while (isdigit(*s))
	    ++s;
	}
      else
	cfp->wild = 1;
    }
  return s;
}

static int
is_a_pos(unsigned char *s, unsigned char *tmp)
{
  unsigned char c = *tmp;
  int retval = 0;
  *tmp = '\0';
  if (!strcmp((const char*)s, "n"))
    {
      *tmp = c;
      return 1;
    }
  else
    {
      while (*s)
	if (*s > 127 || !isupper(*s))
	  break;
	else
	  ++s;

      retval = !*s;
      *tmp = c;
      return retval;
    }
}

static char *
parse_cts_f2(struct CF *cfp, int tts_mode, char *s)
{
  int len = 0;
  int found_square = 0;
  char *tmp = s;
  
  cfp->f2 = mb_new(cfp->owner->owner->owner->owner->owner->mb_f2s);

  while (*tmp)
    {
      if (*tmp == '[')
	{
	  ++found_square;
	  break;
	}
      else if (*tmp == ' ' || *tmp == '\t')
	break;
      else
	++tmp;
    }
  
  if (found_square || !is_a_pos((unsigned char *)s, (unsigned char *)tmp))
    {
      len = f2_parse((unsigned char *)cfp->owner->owner->file, cfp->owner->lnum, 
		     (unsigned char *)s, cfp->f2, NULL, 
		     cfp->owner->owner->owner->owner->owner);
    }
  else
    {
      /* FIXME: should do some POS validation */
      /*cfp->f2->pos = (unsigned char*)strndup(s, len);*/
      char save = *tmp;
      *tmp = '\0';
      len = f2_parse((unsigned char *)cfp->owner->owner->file, cfp->owner->lnum, 
		     (unsigned char *)s, cfp->f2, NULL, 
		     cfp->owner->owner->owner->owner->owner);
      cfp->f2->pos = (const Uchar *)strdup((const char*)cfp->f2->pos);
      *tmp = save;
    }
  if (cfp->f2 && cfp->f2->gw && !strcmp((const char*)cfp->f2->gw, "X"))
    cfp->f2->gw = NULL;
  if (len > 0)
    return s + len;
  else
    return ++s;
}

static char *
cts_props(struct prop **p, char *s)
{
  char *tmp = s;
  char *buf = NULL, save = '\0';
  buf = malloc(strlen(tmp)+1);
  while (*tmp == '@')
    {
      char *group = NULL, *name = NULL, *value = NULL, *end = tmp+1;
      while (*end)
	if ('@' == *end || isspace(*end))
	  {
	    save = *end;
	    *end = '\0';
	    break;
	  }
	else
	  ++end;
      strcpy(buf, tmp+1);
      s = end;
      *end = save;
      tmp = buf;
      if ((end = strchr(tmp, ':')))
	{
	  group = buf;
	  *end = '\0';
	  tmp = end + 1;
	}
      if ((end = strchr(tmp, '=')))
	{
	  name = tmp;
	  *end = '\0';
	  tmp = end + 1;
	}
      value = tmp;
      if (!group && !name)
	{
	  if (!strcmp(value, "yn"))
	    name = "field";
	  else if (!strcmp(value, "date"))
	    name = "discourse";
	}
      *p = props_add_prop_sub(*p, (const unsigned char *)group, (const unsigned char *)name,
			      (const unsigned char *)value, NULL, NULL, NULL, -1);
    }
  return s;
}

int
nl_parse_cts(char *line, char *end, struct NLE *nlep, int tts_mode)
{
  char *s = line;
  List *cfs = list_create(LIST_SINGLE);
  int do_new_cf = 1;
  while (s < end)
    {
      static struct CF* cfp = NULL;

      if (do_new_cf)
	cfp = new_cf(cfs);
      else
	do_new_cf = 1;
      cfp->owner = nlep;
      if (*s == '!')
	{
	  cfp->neg = 1;
	  ++s;
	}
      else if (*s == ';')
	{
	  cfp->clear = 1;
	  ++s;
	}
      else if (tts_mode && *s == ':')
	{
	  vwarning2(nl_file, nl_lnum, "%s: syntax error in RHS (did you mean ';')",line);
	  ++s;
	}

      if ('<' == *s)
	s = parse_angled_preds(cfp, tts_mode, s);
      else if (*s == '*' && s[1] != '[')
	s = parse_wild_cf(cfp, tts_mode, s);
      else if (*s == '@')
	{
	  s = cts_props(&nlep->props, s);
	  do_new_cf = 0;
	}
      else
	{
	  s = parse_cts_f2(cfp, tts_mode, s);
	  /* POS handling */
	  if (NULL == cfp->f2->cf)
	    {
	      /*cfp->f2->cf*/ /* cfp->cf = "*"; */
	      if (cfp->f2->pos)
		cfp->cf = (const char*)cfp->f2->pos;
	    }
	  else
	    cfp->cf = (const char *)cfp->f2->cf;
	  if ('<' == *s)
	    s = parse_angled_preds(cfp, tts_mode, s);
	}
      while (isspace(*s))
	++s;
    }
  if (tts_mode)
    nlep->tts = (struct CF **)list2array(cfs);
  else
    {
      nlep->ncfs = list_len(cfs);
      nlep->cfs = (struct CF**)list2array(cfs);
    }
  list_free(cfs, NULL);
  return 0;
}

#if 0 /* BLOCK OF DEAD CODE */
	{
	  if (tts_mode)
	    {
	      predp->attr = "cf";
	      cfp->cf = predp->value = s;
	      predp = new_pred(preds);
	    }
	  else
	    {
	      cfp->cf = s;
	      if (*s == '*')
		cfp->wild = 1;
	    }
	  while (s < end && *s != '[')
	    ++s;
	  if (s == end)
	    {
	      /* notice2(nl_file,nl_lnum,"%s: no [-predicates in condition token",s);*/
	      return cfp->bad = 1;
	    }
	  else
	    *s++ = '\0';
	  while ((predc = strchr(s,']')) && predc < end)
	    {
	      if (!strncmp(s,"#ADV",3))
		{
		  predp->attr="#ADV";
		  s += 3;
		  if (s == predc)
		    *s++ = '\0';
		  else
		    {
		      vwarning2(nl_file, nl_lnum, "%s: #ADV should end with ']'",line);
		      return cfp->bad = 1;
		    }
		}
	      else
		{
		  char *e;
		  if ((e = strchr(s,'=')) && e < predc)
		    {
		      predp->attr = s;
		      s = e;
		      *s++ = '\0';
		      predp->value = s;
		    }
		  else
		    {
		      if ((e = strstr(s,"//")) && e < predc)
			{
			  if (e - s)
			    {
			      predp->attr = "gw";
			      predp->value = s;
			      predp = new_pred(preds);
			    }
			  predp->attr = "sense";
			  *e++ = '\0';
			  ++e;
			  predp->value = e;
			}
		      else
			{
			  /* to match by sense there must be a gw as well */
			  predp->attr = "gw";
			  predp->value = s;
			}
		    }
		  s = predc;
		  *s++ = '\0';
		  if (isspace(*s) || '\0' == *s)
		    break;
		}
	      if (isupper(*s) || *s == 'n')
		{
		  char *epos, save;

		  pos_predp = predp = new_pred(preds);
		  predp->attr = "pos";
		  predp->value = s;
		  while (*s && !isspace(*s))
		    if ('$' == *s)
		      break;
		    else
		      ++s;
		  save = *s;
		  *s = '\0';
		  if ((epos = strchr(predp->value,'\'')))
		    {
		      *epos++ = '\0';
		      predp = new_pred(preds);
		      predp->attr = "epos";
		      predp->value = epos;
		    }
		  *s = save;
		}
	      if ('$' == *s)
		{
		  *s++ = '\0';
		  predp = new_pred(preds);
		  predp->attr = "norm0";
		  predp->value = s;
		  while (*s && !isspace(*s))
		    ++s;
		}
	      if (isspace(*s))
		{
		  *s++ = '\0';
		  break;
		}
	    }
	}      
#endif /* BLOCK OF DEAD CODE */
