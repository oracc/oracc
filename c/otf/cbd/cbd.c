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
  c->edits = list_create(LIST_SINGLE);
  c->haliases = hash_create(16);
  c->hentries = hash_create(1024);
  c->l.file = file;
  c->l.line = 1;
  c->aliasmem = mb_init(sizeof(struct alias), 1024);
  c->cgpmem = mb_init(sizeof(struct cgp), 1024);
  c->editmem = mb_init(sizeof(struct edit), 1024);
  c->partsmem = mb_init(sizeof(struct parts), 1024);
  c->sensemem = mb_init(sizeof(struct sense), 1024);
  return c;
}

struct cbd *
cbd_init(void)
{
  struct cbd *c = init_cbd();
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
cbd_setup(struct cbd*c)
{
  setup_cbd(c);
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
	  ++c->l.line;
	  ++rest;
	}
      else if (isspace(rest[0][0]))
	{
	  int i = 1;
	  while (isspace(rest[0][i]))
	    ++i;
	  if (!rest[0][i])
	    {
	      ++c->l.line;
	      ++rest;
	    }
	  else
	    {
	      warning("malformed line; spaces followed by non-spaces");
	      ++c->l.line;
	      ++rest;
	    }
	}
      else if (**rest == '#')
	{
	  warning("comments not allowed before text");
	  ++c->l.line;
	  ++rest;
	}
      else
	{
	  warning("unexpected start-of-line");
	  ++c->l.line;
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
	      || ((rest[0][0] == '+' || rest[0][0] == '-') && rest[0][1] == '@'))
	    break;
	  else
	    {
	      ++c->l.line;
	      ++rest;
	    }
	}
      if (*rest)
	{
	  if (!strncmp((ccp)rest[0], "@letter", strlen("@letter")))
	    {
	      /* letter() */
	      ++c->l.line;
	      ++rest;
	    }
	  else if (rest[0][0] == '#')
	    {
	      warning("comments not allowed between @entry blocks");
	      ++c->l.line;
	      ++rest;
	    }
	  else
	    {
	      int saved_lnum = c->l.line;
	      rest = parse_entry(c, rest);
	      if (c->l.line == saved_lnum)
		{
		  warning("entry never moved lnum");
		  break;
		}
	    }
	}
    }
  if (check)
    {
      (void)edit_check(c);
      edit_script(c);
    }
  free(lines);
  return status;
}
