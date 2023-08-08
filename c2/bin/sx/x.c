#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

int
npad(int b, int e)
{
  int d = e - b;
  if (d > 10000)
    return 4;
  else if (d > 1000)
    return 3;
  else if (d > 100)
    return 2;
  else if (d > 10)
    return 1;
  return 0;
}

void
x(const char *sl, const char *in)
{
  const char *str, *top;
  char buf[32], *end, *ins, *ins_base;
  
  if (strlen(sl) > 6)
    {
      fprintf(stderr, "signlist name restricted to maximum of 6 characters: %s is too long\n", sl);
      return;
    }
  strcpy(buf, sl);
  ins_base = ins = buf + strlen(sl);
  top = &buf[31];
  for (str = in; str; ++str)
    {
      if (strchr(str, '-'))
	{
	  int rb, re;
	  rb = strtol(str,&end, 10);
	  if (end && '-' == *end)
	    {
	      str = end + 1;
	      re = strtol(str, &end, 10);
	      if (end)
		{
		  int x, pow10 = 10;
		  int zeroes = npad(rb,re);
		  char pad[zeroes+1];
		  for (x = 0; x < zeroes; ++x)
		    pad[x] = '0';
		  pad[x] = '\0';
		  for (x = rb; x <= re; ++x)
		    {
		      if (x == pow10)
			{
			  pow10 *= 10;
			  pad[strlen(pad)-1] = '\0';
			}
		      fprintf(stderr, "%s%s%d\n", sl, pad, x);
		    }
		  str = end;
		}
	      else
		{
		  fprintf(stderr, "bad end in range %s\n", str);
		  while (*str && !isspace(*str))
		    ++str;
		}
	    }
	  else
	    {
	      fprintf(stderr, "bad start in range %s\n", str);
	      while (*str && !isspace(*str))
		++str;
	    }	  
	}
      else
	{
	  ins = ins_base;
	  while (*str && !isspace(*str))
	    {
	      if (ins == top)
		{
		  *ins = '\0';
		  fprintf(stderr, "signlist number %s is too long\n", buf);
		  return;
		}
	      else
		*ins++ = *str++;
	    }
	  *ins = '\0';
	  fprintf(stdout, "%s\n", buf);
	}
      if ('\0' == *str)
	break;
    }
}

int
main(int argc, char **argv)
{
  const char *in = "1-110 11a 11b";

  x("MZL", in);
}
