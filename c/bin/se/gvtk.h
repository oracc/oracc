#ifndef _GVTK_H
#define _GVTK_H

#include "dbi.h"

struct gvtk_g {
  const char *type;
  const char *sign;
  const char *utf8;
  const char *mess;
};

typedef struct gvtk_g gvtk_g;

extern gvtk *gvtk_find(const char *name);
extern void gvtk_init(const char *name, Dbi_index *dbi);
extern void gvtk_term(const char *name)
extern gvtk_g * gvtk_validate(const char *g);

#endif/*_GVTK_H*/
