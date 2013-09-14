#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include "resolver.h"

void
xmlrpc(void)
{
  const char *prog = "/usr/local/oracc/www/cgi-bin/oracc-xmlrpc-cgi";
  fprintf(stderr, "oracc-resolver: invoking oracc-xmlrpc-cgi\n");
  if (execl(prog, "oracc-xmlrpc-cgi", (char *)0))
    {
      fprintf(stderr, "oracc-resolver: xmlrpc function failed to exec %s\n", prog);
      perror("xmlrpc");
      exit(1);
    }
}
