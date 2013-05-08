#include <string.h>
#include <ctype128.h>
#include "list.h"
#include "memblock.h"
#include "ngram.h"

static char *
parse_angled_preds(struct CF *cfp, int tts_mode, char *s)
{
  List *preds = list_create(LIST_SINGLE);
  struct PRED *predp;
  char *anglec = NULL;

  while (*s && '<' == *s)
    {
      predp = new_pred(preds);
      ++s;
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
	      predp->value = "1";
	    }
	  ++anglec;
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

static char *
parse_cts_f2(struct CF *cfp, int tts_mode, char *s)
{
  int len = 0;
  cfp->f2 = mb_new(cfp->owner->owner->owner->owner->owner->mb_f2s);
  len = f2_parse((unsigned char *)cfp->owner->owner->file, cfp->owner->lnum, 
		 (unsigned char *)s, cfp->f2, NULL, 
		 cfp->owner->owner->owner->owner->owner);
  if (len > 0)
    return s + len;
  else
    return ++s;
}

int
nl_parse_cts(char *line, char *end, struct NLE *nlep, int tts_mode)
{
  char *s = line;
  List *cfs = list_create(LIST_SINGLE);
  while (s < end)
    {
      struct CF* cfp = new_cf(cfs);
      cfp->owner = nlep;
      if (*s == '!')
	{
	  cfp->neg = 1;
	  ++s;
	}

      if ('<' == *s)
	s = parse_angled_preds(cfp, tts_mode, s);
      else if (*s == '*' && s[1] != '[')
	s = parse_wild_cf(cfp, tts_mode, s);
      else
	{
	  s = parse_cts_f2(cfp, tts_mode, s);
	  cfp->cf = cfp->f2->cf;
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
