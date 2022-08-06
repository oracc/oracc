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

struct entry *
entry_init(YYLTYPE l, struct cbd* c)
{
  struct entry *e = init_entry();
  e->l = c->l;
  e->owner = c;
  e->lang = c->lang;
  list_add(c->entries, e);
  e->meta = mb_new(c->metamem);
  e->l = l;
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

void
entry_term(struct entry *e)
{
  term_entry(e);
  free(e);
}

#if 0

unsigned char **
parse_entry(struct cbd *c, unsigned char **ll)
{
  unsigned char *s = NULL, end;
  struct entry *e = NULL;

  if (ll[0][0] == '+' || ll[0][0] == '-')
    s = &ll[0][1];
  else
    s = ll[0];

  if (!strncmp((ccp)s, "@entry", strlen("@entry")))
    {
      unsigned const char *cgpstr = NULL;
      e = init_entry();
      e->l = c->l;
      e->owner = c;
      e->lang = c->lang;
      list_add(c->entries, e);

      s = ll[0];
      while (!isspace(*s))
	{
	  switch (*s)
	    {
	    case '+':
	    case '-':
	      edit_add(ll, e);
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

      /*memset(&cgp,'\0',sizeof(struct cgp));*/
      cgp_parse(e->cgp, s, &e->l);
      cgpstr = cgp_str(e->cgp,0);
      hash_add(c->hentries,(e->cgp->tight = npool_copy(cgpstr, e->owner->pool)), e);
      free((void*)cgpstr);
      cgpstr = NULL;

      if (strchr((ccp)e->cgp->cf, ' '))
	e->compound = 1;

      if (verbose)
	fprintf(stderr, "@entry %s[%s]%s\n", e->cgp->cf, e->cgp->gw, e->cgp->pos);
      if (entries)
	printf("%s [%s] %s\n", e->cgp->cf, e->cgp->gw, e->cgp->pos);
      ++ll;
      ++c->l.line;
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
	  int plus = 0;
	  unsigned char *plus_orig = NULL;
	  if ('#' == ll[0][0])
	    {
	      ++ll;
	      ++c->l.line;
	    }
	  else if ('+' == ll[0][0] || '-' == ll[0][0])
	    {
	      unsigned char *dst, *src;
	      plus = (ll[0][0] == '+' ? 1 : -1);
	      plus_orig = (ucp)strdup((ccp)ll[0]);
	      for (dst = ll[0], src = &ll[0][1]; *src;)
		*dst++ = *src++;
	      *dst = '\0';
	      if (strncmp((ccp)ll[0],"@sense", strlen("@sense")))
		{
		  warning("misplaced +/- will be ignored (only valid on +@entry or +@sense");
		  plus = 0;
		}
	    }
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
		      ++c->l.line;
		      return ll;
		    }
		  else
		    {
		      warning("bad @end entry line");
		      ++ll;
		      ++c->l.line;
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
		      (p->parser)(e,es,&e->l);
		      ++ll;
		      ++c->l.line;
		      if (plus)
			{
			  /* This only happens with +@sense or -@sense which
			     means edit_add's context can never look
			     at the previous pointer barring a later
			     coding error */
			  edit_add(&plus_orig, e);
			}
		    }
		  else if (parse_dcf(e, es))
		    {
		      ++ll;
		      ++c->l.line;
		    }
		  else
		    {
		      vwarning("unknown tag %s", tag);
		      ++ll;
		      ++c->l.line;
		      break;
		    }
 		}
	    }
	  else if (ll[0][0] == '>')
	    {
	      edit_add(ll, e);
	      ++ll;
	      ++c->l.line;
	    }
	  else
	    {
	      warning("bad character at start of line in entry");
	      ++ll;
	      ++c->l.line;
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
#endif
