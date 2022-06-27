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
static unsigned char *check_bom(unsigned char *s);
static unsigned char **setup_lines(unsigned char *ftext);

struct cbd *
init_cbd(void)
{
  struct cbd *c = NULL;
  c = malloc(sizeof(struct cbd));
  c->pool = npool_init();
  c->letters = list_create(LIST_SINGLE);
  c->entries = list_create(LIST_SINGLE);
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
  struct stat finfo;
  ssize_t fsize;
  int fdesc, ret;
  static unsigned char *ftext = NULL;

  if (-1 == stat(fname,&finfo))
    {
      fprintf(stderr,"gx: stat failed on %s\n",fname);
      return -1;
    }
  if (!S_ISREG(finfo.st_mode))
    {
      fprintf(stderr,"gx: %s not a regular file\n",fname);
      return -1;
    }
  fsize = finfo.st_size;
  if (!fsize)
    {
      fprintf(stderr,"gx: %s: empty file\n",fname);
      return -1;
    }
  if (NULL == (ftext = malloc(fsize+1)))
    {
      fprintf(stderr,"gx: %s: couldn't malloc %d bytes\n",
	      fname,(int)fsize);
      return -1;
    }

  fdesc = open(fname,O_RDONLY);
  if (fdesc >= 0)
    {
      ssize_t ret = read(fdesc,ftext,fsize);
      close(fdesc);
      if (ret == fsize)
	ftext[fsize] = '\0';
      else
	{
	  fprintf(stderr,"gx: read %d bytes failed\n",(int)fsize);
	  return -1;
	}
    }
  else
    {
      fprintf(stderr, "gx: %s: open failed\n", fname);
      return -1;
    }

  file = errmsg_fn ? errmsg_fn : fname;

  ret = parse_cbd(ftext, fsize);
  free(ftext);
  
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

static unsigned char *
check_bom(unsigned char *s)
{
  if (s[0] == 0xef && s[1] == 0xbb && s[2] == 0xbf)
    return s+3;
  else if ((s[0] == 0x00 && s[1] == 0x00 && s[2] == 0xfe && s[3] == 0xff)
	   || (s[0] == 0xff && s[1] == 0xfe && s[2] == 0x00 && s[3] == 0x00)
	   || (s[0] == 0xfe && s[1] == 0xff)
	   || (s[0] == 0xff && s[1] == 0xfe))
    {
      fprintf(stderr,"unhandled UTF-format (I only understand UTF-8)\n");
      return NULL;
    }
  else
    return s;
}

static unsigned char **
setup_lines(unsigned char *ftext)
{
  unsigned char **p,**ret;
  register unsigned char*s = ftext;
  int nlines = 0;
  while (*s)
    {
      if (*s == '\r')
	{
	  ++nlines;
	  if (s[1] != '\n')
	    *s++ = '\n';     /* for MAC \r, map to \n */
	  else
	    s+=2; 	     /* for DOS \r\n, skip \n */
	}
      else if (*s == '\n') /* UNIX */
	{
	  ++nlines;
	  ++s;
	}
      else
	{
	  ++s;
	}
    }
  if (s[-1] != '\n' && s[-1] != '\r')
    ++nlines;
  ++nlines; /* NULL ptr to terminate */
  ret = p = malloc(nlines*sizeof(unsigned char *));
  s = ftext;
#if 0 /* this screws with the line count, or with corruption detection */
  while ('\n' == *s)
    {
      *s++ = '\0';
      ++lnum;
    }
#endif
  while (*s)
    {
      *p++ = s;
      while (*s && '\n' != *s)
	++s;
      if (*s == '\n')
	{
	  if (s > ftext && s[-1] == '\r')
	    {
	      s[-1] = '\0';
	      ++s;
	    }
	  else
	    *s++ = '\0';
	}
    }
  *p = NULL;
  return ret;
}
