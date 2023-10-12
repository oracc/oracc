#include <stdio.h>
#include <xmlify.h>

int
main(int argc, const char **argv)
{
  int ch;
  while ((EOF != (ch = getchar())))
    {
      const char *x = xmlify_char(ch);
      if (x)
	fputs(x, stdout);
      else
	putchar(ch);
    }
}
