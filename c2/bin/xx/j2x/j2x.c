#include <stdlib.h>
#include <stdio.h>
#include <j2x.tab.h>

extern int yy_flex_debug, yydebug;

int
main(int argc, const char **argv)
{
  int ret = 0;
  if (argv[1] && *argv[1])
    {
      yy_flex_debug = yydebug = 1;
      setvbuf(stdout, NULL, _IONBF, 0);
    }
  else
    yy_flex_debug = yydebug = 0;

  printf("<j2x>");
  ret = yyparse();
  printf("</j2x>");
  return ret;
}
