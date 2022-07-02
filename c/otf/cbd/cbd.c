#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype128.h>

#include "gx.h"

static int parse_cbd(unsigned char *ftext, ssize_t fsize);

extern int setenv(const char *,const char *, int);

struct cbd *
init_cbd(void)
{
  struct cbd *c = NULL;
  c = malloc(sizeof(struct cbd));
  c->pool = npool_init();
  c->letters = list_create(LIST_SINGLE);
  c->entries = list_create(LIST_SINGLE);
  c->haliases = hash_create(16);
  c->hentries = hash_create(1024);
  return c;
}

void
setup_cbd(struct cbd*c)
{
  c->iname = malloc(strlen((ccp)c->project) + strlen((ccp)c->lang) + 2);
  sprintf((char*)c->iname, "%s:%s", c->project, c->lang);
  hash_add(cbds, c->iname, c);
  /* xpdinit */
  /* cuneify_init(c->xpd); */
}

void
term_cbd(struct cbd*c)
{
  npool_term(c->pool);
  hash_add(cbds, c->iname, NULL);
  cuneify_term();
  free(c);
}

int
cbd(const char *fname)
{
  static ssize_t fsize;
  unsigned char *ftext = NULL;
  int ret = 0;
  
  ftext = slurp("gx", fname, &fsize);
  if (ftext)
    {
      ret = parse_cbd(ftext, fsize);
      free(ftext);
    }
  else
    ret = -1;
  return ret;
}

static int
parse_cbd(unsigned char *ftext, ssize_t fsize)
{
  unsigned char *ftext_post_bom, **lines, **rest;
  struct cbd *c;

  ftext_post_bom = check_bom(ftext);
  if (!ftext_post_bom)
    return 1;

  (void)vchars(ftext_post_bom,fsize);
  rest = lines = setup_lines(ftext_post_bom);

  c = init_cbd();

  while (*rest)
    {
      if (rest[0][0] == '@')
	break;
      else if (!rest[0][0])
	{
	  ++lnum;
	  ++rest;
	}
      else if (isspace(rest[0][0]))
	{
	  int i = 1;
	  while (isspace(rest[0][i]))
	    ++i;
	  if (!rest[0][i])
	    {
	      ++lnum;
	      ++rest;
	    }
	  else
	    {
	      warning("malformed line; spaces followed by non-spaces");
	      ++lnum;
	      ++rest;
	    }
	}
      else if (**rest == '#')
	{
	  warning("comments not allowed before text");
	  ++lnum;
	  ++rest;
	}
      else
	{
	  warning("unexpected start-of-line");
	  ++lnum;
	  ++rest;
	}
    }
  rest = parse_header(c, rest);
  setup_cbd(c);
  while (*rest)
    {
      while (*rest)
	{
	  if ('@' == rest[0][0]
	      || (rest[0][0] == '+' && rest[0][1] == '@'))
	    break;
	  else
	    {
	      ++lnum;
	      ++rest;
	    }
	}
      if (*rest)
	{
	  if (!strncmp((ccp)rest[0], "@letter", strlen("@letter")))
	    {
	      /* letter() */
	      ++lnum;
	      ++rest;
	    }
	  else
	    {
	      int saved_lnum = lnum;
	      rest = parse_entry(c, rest);
	      if (lnum == saved_lnum)
		{
		  warning("entry never moved lnum");
		  break;
		}
	    }
	}
    }
  free(lines);
  return status;
}
