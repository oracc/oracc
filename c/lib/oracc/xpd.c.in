#include <stdlib.h>
#include <stdio.h>
#include <ctype128.h>
#include "npool.h"
#include "runexpat.h"
#include "hash.h"
#include "xpd.h"

static const char xpd_project[32];
static Hash_table *xpd_context = NULL;
static struct npool *xpd_pool = NULL;
extern const char *project;

static void xpds_load(const char *fname);

const char *
xpd_option(const char *name)
{
  return hash_find(xpd_context, (const unsigned char *)name);
}

int
xpd_option_int(const char *name)
{
  const char *var = hash_find(xpd_context, (const unsigned char *)name);
  if (var)
    return atoi(var);
  else
    return 0;
}

void
xpd_init(void)
{
  static char buf[64];
  if (!*xpd_project || strcmp(xpd_project,project))
    {
      xpd_term();
      sprintf(buf,"@@ORACC@@/www/%s/config.xml",project);
      if (xaccess(buf,R_OK,0))
	{
	  /* temporary, while we switch to config.xml permanently */
	  sprintf(buf,"@@ORACC@@/xml/%s/config.xml",project);
	}
      if (!xaccess(buf,R_OK,0))
	{
	  strcpy((char*)xpd_project,(char*)project);
	  xpd_context = hash_create(20);
	  xpd_pool = npool_init();
	  xpds_load(buf);
	}
    }
}

void
xpd_term(void)
{
  npool_term(xpd_pool);
  xpd_pool = NULL;
  if (xpd_context)
    {
      free(xpd_context);
      xpd_context = NULL;
    }
}

static const char *xpd_ns = "http://oracc.org/ns/xpd/1.0";
#define xpd_offset (strlen(xpd_ns))

static void
xpd_sH(void *userData, const char *name, const char **atts)
{
  if (!strncmp(name,xpd_ns,xpd_offset))
    {
      const char *lname = name + xpd_offset;
      if (*lname == ':')
	{
	  ++lname;
	  if (!strcmp(lname,"option"))
	    {
	      hash_add(xpd_context,
		       npool_copy((unsigned char *)findAttr(atts,"name"),xpd_pool),
		       npool_copy((unsigned char *)findAttr(atts,"value"),xpd_pool));
	    }
	}
    }
}

static void
xpd_eH(void *userData, const char *name)
{
}

static void
xpds_load(const char *fname)
{
  const char *fns[2];
  fns[0] = fname;
  fns[1] = NULL;
  runexpatNS(i_list, fns, xpd_sH, xpd_eH, ":");
}

void
xpd_echo(const char *xpd_project, FILE *fp)
{
  char tmp[_MAX_PATH];
  FILE *cfg = NULL;
  int ch;
  (void)sprintf(tmp,"@@ORACC@@/xml/%s/config.xml", xpd_project);
  cfg = xfopen(tmp,"r");
  while (EOF != (ch = fgetc(cfg)))
    fputc(ch,fp);
  xfclose(tmp,cfg);
}
