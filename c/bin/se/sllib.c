#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <wctype.h>
#include <locale.h>
#include <psd_base.h>
#include <fname.h>
#include <options.h>
#include <dbi.h>
#include "sllib.h"

static const char *oracc = NULL;

unsigned char *
sl_lookup(Dbi_index *dbi, char *key)
{
  if (dbi && key)
    {
      dbi_find(dbi,(unsigned char *)key);
      if (dbi->data)
	return dbi->data;
    }
  return NULL;
}

Dbi_index *
sl_init(char *project, char *name)
{
  Dbi_index *dbi = NULL;
  char *db;

  /* Figure out the db and open it */
  if (!project)
    project = "ogsl";

  if (!name)
    name = "ogsl";

  oracc = oracc_home();
  db = malloc(strlen(oracc)+strlen("/pub/sl/") + strlen(project) + 1);
  sprintf(db, "%s/pub/%s/sl", oracc, project);

  if (!(dbi = dbi_open(name, db)))
    fprintf(stderr, "sllib: failed to open %s (project=%s; name=%s)\n", db, project, name);

  return dbi;
}

void
sl_term(Dbi_index *dbi)
{
  if (dbi)
    dbi_close(dbi);
}
