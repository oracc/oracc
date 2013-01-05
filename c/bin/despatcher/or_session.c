#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "resolver.h"

static const char *
ip4_hex(const char *ip)
{
  int ip1,ip2,ip3,ip4;
  static char hex[9];
  sscanf(ip,"%d.%d.%d.%d",&ip1,&ip2,&ip3,&ip4);
  sprintf(hex,"%02x%02x%02x%02x",ip1,ip2,ip3,ip4);
  return hex;
}

/* FIXME: ERROR CHECKING */
const char *
create_session(void)
{
  static char dirbuf[512];
  char *last_slash = NULL, *d = NULL;
  const char *remote_addr = getenv("REMOTE_ADDR");
  if (remote_addr)
    {
      sprintf(dirbuf, "/var/tmp/oracc/pager/P2_%s", ip4_hex(remote_addr));
      d = dirbuf;
      if (-1 == mkdir(d,0700) && errno != EEXIST)
	{
	  fprintf(stderr, "oracc-despatcher: mkdir %s failed\n", d);
	  exit(1);
	}
    }
  else
    {
      strcpy(dirbuf, "/var/tmp/oracc/pager/p2_XXXXXX");
      if (!(d = mkdtemp(dirbuf)))
	{
	  fprintf(stderr, "oracc-despatcher: mkdtemp %s failed\n", dirbuf);
	  exit(1);
	}
    }
  last_slash = strrchr(d, '/');
  return last_slash + 1;
}

