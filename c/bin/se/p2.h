#ifndef P2_H_
#define P2_H_
struct p2_options
{
  const char *sort_fields;
  const char *sort_labels;
  const char *heading_punct;
  const char *catalog_fields;
  const char *catalog_widths;
  const char *catalog_labels;
};

#include "npool.h"

extern struct p2_options*p2_load(const char *project, const char *state, struct npool *pool);
extern char *p2_maybe_append_designation(const char *s, struct npool *pool);

#endif/*P2_H_*/
