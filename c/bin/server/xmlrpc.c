#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include "resolver.h"

void
xmlrpc(void)
{
  const char *prog = "/home/oracc/www/cgi-bin/oracc-xmlrpc";
  fprintf(stderr, "oracc-resolver: invoking oracc-xmlrpc\n");
  if (execl(prog, "oracc-xmlrpc", (char *)0))
    {
      fprintf(stderr, "oracc-resolver: xmlrpc function failed to exec %s\n", prog);
      perror("xmlrpc");
      exit(1);
    }
}
