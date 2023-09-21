#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <oraccsys.h>

#define STRREP_MAX_ 31

void
strrep_f_h(const char *str, FILE *fp, Hash*reps)
{
  const char *s = str;
  while (*s)
    {
      if ('@' == *s && '@' == s[1])
	{
	  const char *e;
	  s += 2;
	  e = strstr(s, "@@");
	  if (e)
	    {
	      if (e - s > STRREP_MAX_)
		{
		  fprintf(stderr, "@@...@@ string too long");
		  s = e + 2;
		}
	      else
		{
		  char buf[STRREP_MAX_ + 1];
		  strncpy(buf, s, e-s);
		  buf[e-s] = '\0';
		  const char *rep = hash_find(reps, (uccp)buf);
		  if (!rep)
		    rep = getenv(buf);
		  if (rep)
		    fputs(rep, fp);
		  else
		    {
		      fprintf(stderr, "strrep: replacement key %s not found\n", buf);
		      fprintf(fp, "@@%s@@", buf);
		    }
		  s = e+2;
		}
	    }
	  else
	    {
	      fputs("@@", fp);
	      fputc(*s, fp);
	      ++s;
	    }
	}
      else
	{
	  fputc(*s, fp);
	  ++s;
	}
    }
}
