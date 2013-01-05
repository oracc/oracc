#ifndef _SKL_H
#define _SKL_H

#include "hash.h"

struct skl
{
  const char values[16];
  const char type[16];
  unsigned long count;
  unsigned long offset;
  unsigned long length;
};

enum rt_types { rt_raw , rt_singleton , rt_multi };

struct reftab
{
  int type;
  union charhash { const char *id; Hash_table *hash; } c;
};

extern const char **ids;

extern const char *skl_file(const char *project,const char *skel, int sky_ext);
extern const char *skl_file_e(const char *project, const char *skel, const char *ext);
extern const char *skl_file_lang(const char *project,const char *skel, const char *lang, int sky_ext);
extern const char *skl_file_l(const char *project, const char *skel, const char *lang, const char *ext);

extern Hash_table *skl_load(const char *project,const char *skel, const char *type, const char *values);
extern const char *sky_find(Hash_table *hp, const char **keys);
extern const char **sky_last_ids(void);
extern Hash_table*smp_load(const char *project, const char *skel);
extern void skl_term(void);
extern void smp_term(void);

#endif /*_SKL_H*/
