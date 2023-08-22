#include <stdlib.h>
#include <stdio.h>
#include <oraccsys.h>
#include <loadfile.h>
#include <hash.h>
#include <oid.h>

static Hash *oids = NULL;
static const char *oidtab = NULL;
static unsigned char *oidmem = NULL;
static unsigned char **oidlines= NULL;

static int
cmpstringp(const void *p1, const void *p2)
{
  return strcmp(* (char * const *) p1, * (char * const *) p2);
}

static const char *
oid_tab(void)
{
  int len = strlen(oracc_home() + strlen("/oid/oid.tab") + 1);
  char *tmp = malloc(len);
  sprintf(tmp, "%s%s", oracc_home(), "/oid/oid.tab");
  return oidtab = tmp;
}

void
oid_load(void)
{
  ssize_t nlines;
  
  oids = hash_create(2048);
  oidlines = loadfile_lines3((uccp)oid_tab(), (size_t*)&nlines, &oidmem);
  qsort(oidlines, nlines, sizeof(unsigned char *), cmpstringp);
}

int32_t
oid_next_id(void)
{
  int i;
  int32_t last = -1;
  for (i = 0; oidlines[i]; ++i)
    {
      int32_t this;
      this = strtol((const char *)&oidlines[i][1], NULL, 10);
      if (last >= 0) {
	if (this - last > 1)
	  return last + 1;
      }
      last = this;
    }
  return last;
}
