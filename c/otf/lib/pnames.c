#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#undef TEST

#ifndef TEST
#include "warning.h"
extern int vchars(unsigned char *ftext, ssize_t fsize);
#endif

int pmax = 0;
int *pvalues;
unsigned char **plines;
unsigned char **pnames;
static const char *pfname = "/usr/local/oracc/pub/cdli/Pnames.tab";

void
init_pnames()
{
  struct stat finfo ;
  unsigned char *ftext;
  ssize_t fsize;
  int fdesc;
  int nnames = 0;
  register unsigned char *s;
  register int i;

  if (-1 == stat(pfname,&finfo))
    {
      fprintf(stderr,"atf2xtf: stat failed on %s\n",pfname);
      exit(1);
    }
  if (!S_ISREG(finfo.st_mode))
    {
      fprintf(stderr,"atf2xtf: %s not a regular file\n",pfname);
      exit(1);
    }
  fsize = finfo.st_size;
  if (!fsize)
    {
      fprintf(stderr,"atf2xtf: %s: empty file\n",pfname);
      exit(1);
    }
  if (NULL == (ftext = malloc(fsize+2)))
    {
      fprintf(stderr,"atf2xtf: %s: couldn't malloc %d bytes\n",
	      pfname,(int)fsize);
      exit(1);
    }
  fdesc = open(pfname,O_RDONLY);
  if (fdesc >= 0)
    {
      ssize_t ret = read(fdesc,ftext,fsize);
      if (ret == fsize)
	{
	  if (ftext[fsize-1] != '\n')
	    ftext[fsize++] = '\n';
	  ftext[fsize] = '\0';
	}
      else
	{
	  fprintf(stderr,"atf2xtf: read %d bytes failed\n",(int)fsize);
	  exit(1);
	}
    }
  else
    {
      fprintf(stderr, "atf2xtf: %s: open failed\n", pfname);
      exit(1);
    }

#ifndef TEST
  if (vchars(ftext,fsize))
    exit(1);
#endif

  for (nnames = 1, s = ftext; *s; ++s)
    {
      if ('\n' == *s)
	++nnames;
    }
  pvalues = malloc(nnames * sizeof(int));
  plines = malloc(nnames * sizeof(char*));
  for (i = 0,s=ftext; i <= nnames; ++i)
    {
      if ('P' != *s)
	{
	  if ('\0' == *s)
	    break;

	  fprintf(stderr,"%s:%d: malformed pnames line: no P\n",pfname,i);
	  exit(1);
	}

      pvalues[i] = strtol((const char *)(s+1),NULL,10);
      if (pmax < pvalues[i])
	pmax = pvalues[i];

      while (!isspace(*s))
	++s;
      if ('\t' != *s)
	{
	  fprintf(stderr,"%s:%d: malformed pnames line: no tab\n",pfname,i);
	  exit(1);
	}
      else
	++s;
      plines[i] = s;
      while ('\n' != *++s)
	;
      *s++ = '\0';
    }
  pnames = calloc(pmax+1, sizeof(char*));
  for (i = 0; i <= nnames; ++i)
    pnames[pvalues[i]] = plines[i];
}

int
check_pname(const char*id, const unsigned char *name)
{
  int p = strtol(id+1,NULL,10);
  if (p > pmax)
    {
#ifdef TEST
      fprintf(stderr,"%s: P > max (P%d)\n", id, pmax);
#else
      vwarning("%s: P > max (P%d)", id, pmax);
#endif
      return 1;
    }
  if (pnames[p])
    {
      if (strcmp((const char *)name,(const char *)pnames[p]))
	{
#ifdef TEST
	  fprintf(stderr,"%s: %s should be %s\n",id,name,pnames[p]);
#else
	  vwarning("%s: %s should be %s",id,name,pnames[p]);
#endif
	  return 1;
	}
#ifdef TEST
      else
	fprintf(stderr,"%s ok: %s = %s\n",id,name,pnames[p]);
#endif
    }
  else
    {
#ifdef TEST
      fprintf(stderr,"%s: not in catalog\n",id);
#else
      vwarning("%s: not in catalog",id);
#endif
      return 1;
    }
  return 0;
}

#ifdef TEST
int
main(int argc, char **argv)
{
  if (argc < 3)
    {
      fprintf(stderr,"must give P-id and P-name on command line\n");
      exit(1);
    }
  init_pnames();
  (void)check_pname(argv[1],argv[2]);
  return 0;
}
#endif
