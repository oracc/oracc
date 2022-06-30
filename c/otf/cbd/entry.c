#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype128.h>
#include "gx.h"

static struct entry *
init_entry(void)
{
  struct entry *e = calloc(1,sizeof(struct entry));
  e->aliases = list_create(LIST_SINGLE);
  e->forms = list_create(LIST_SINGLE);
  e->senses = list_create(LIST_SINGLE);
  return e;
}
static void
term_entry(struct entry *e)
{
  if (e->b_pri)
    {
      hash_free(e->b_pri, NULL);
      hash_free(e->b_alt, NULL);
      hash_free(e->b_sig, NULL);
      e->b_pri = e->b_alt = e->b_sig = e->b_allow = NULL;
    }
  if (e->b_allow)
    {
      hash_free(e->b_allow, NULL);
      e->b_allow = NULL;
    }
}

unsigned char **
parse_entry(struct cbd *c, unsigned char **ll)
{
  unsigned char *s = NULL, end;
  struct entry *e = NULL;
  static struct cgp cgp;

  if (ll[0][0] == '+')
    s = &ll[0][1];
  else
    s = ll[0];

  if (!strncmp((ccp)s, "@entry", strlen("@entry")))
    {
      unsigned char *cgpstr = NULL;
      e = init_entry();
      e->owner = c;
      e->lang = c->lang;
      list_add(c->entries, e);

      s = ll[0];
      while (!isspace(*s))
	{
	  switch (*s)
	    {
	    case '+':
	      e->plus = 1;
	      ++s;
	      break;
	    case '!':
	      e->bang = 1;
	      ++s;
	      break;
	    case '*':
	      e->usage = 1;
	      ++s;
	      break;
	    default:
	      if (!strncmp((ccp)s,"@entry",strlen("@entry")))
		s += strlen("@entry");
	      else
		warning("junk detected in @entry line");
	      break;
	    }
	}

      memset(&cgp,'\0',sizeof(struct cgp));
      cgp_parse(&cgp, s);
      cgpstr = cgp_cgp_str(&cgp,0);
      hash_add(c->hentries, npool_copy(cgpstr, e->owner->pool), e);
      free(cgpstr);
      cgpstr = NULL;

      cgp_entry(&cgp, e);
      if (strchr((ccp)e->cf, ' '))
	e->compound = 1;

      if (verbose)
	fprintf(stderr, "@entry %s[%s]%s\n", e->cf, e->gw, e->pos);
      if (entries)
	printf("%s [%s] %s\n", e->cf, e->gw, e->pos);
      ++ll;
      ++lnum;
    }
  else
    {
      vwarning("expected @entry but got %s",ll[0]);
      return ll;
    }
  while (*ll)
    {
      if (ll[0][0])
	{
	  if (ll[0][0] == '@')
	    {
	      if (!strncmp((ccp)ll[0], "@end", strlen("@end")))
		{
		  s = ll[0] + strlen("@end");
		  s = tok(s, &end);
		  if (s && !strcmp((ccp)s,"entry"))
		    {
		      /* finish entry code goes here */
		      term_entry(e);
		      ++ll;
		      ++lnum;
		      return ll;
		    }
		  else
		    {
		      warning("bad @end entry line");
		      ++ll;
		      ++lnum;
		      break;
		    }
		}
	      else
		{
		  unsigned char *tag = &ll[0][1], *es /*, save = '\0'*/;
		  struct cbdtag *p = NULL;
		  for (es = tag; *es && !isspace(*es); ++es)
		    ;
		  if (*es)
		    {
		      /*save = *es;*/
		      *es++ = '\0';
		      while (isspace(*es))
			++es;
		    }
		  if ((p = cbdtags((ccp)tag, strlen((ccp)tag))))
		    {
		      /* fprintf(stderr, "found %s with parser %p\n", tag, (void*)p->parser); */
		      (p->parser)(e,es);
		    }
		  else if (parse_dcf(e, es))
		    {
		      ++ll;
		      ++lnum;
		    }
		  else
		    {
		      vwarning("unknown tag %s", tag);
		      ++ll;
		      ++lnum;
		      break;
		    }
		  ++ll;
		  ++lnum;
 		}
	    }
	  else if (ll[0][0] == '>')
	    {
	      /* edit tag processing goes here */
	      ++ll;
	      ++lnum;
	    }
	  else
	    {
	      warning("bad character at start of line in entry");
	      ++ll;
	      ++lnum;
	      break;
	    }
	}
      else
	{
	  warning("glossary ended in mid-entry");
	  break;
	}
    }
  return ll;
}
