#include <stdlib.h>
#include <stdio.h>
#include <oraccsys.h>
#include <loadfile.h>
#include <hash.h>
#include <oid.h>

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
  return tmp;
}

Oids *
oid_load(void)
{
  Oids *o = calloc(1, sizeof(Oids));
  o->oids = hash_create(2048);
  o->lines = loadfile_lines3((uccp)(o->file = oid_tab()), (size_t*)&o->nlines, &o->mem);
  qsort(o->lines, o->nlines, sizeof(unsigned char *), cmpstringp);
  return o;
}

