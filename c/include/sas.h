#ifndef _SAS_H
#define _SAS_H

#include "hash.h"
#include "list.h"

enum sc_type { sc_cons_env , sc_cons_lem };

struct sas_info
{
  const char *name;
  Hash_table *cand;
  Hash_table *nodumb;
  Hash_table *post;
  unsigned char *file;
  size_t flen;
  struct sas_alias*alias_mem;
  int alias_used;
  struct sas_constraint*constraint_mem;
  int constraint_used;
  struct sas_ep_block *ep_blocks;
  int ep_blocks_alloced;
};

struct sas_ep_block
{
  unsigned char *ptrs[256];
  int ptrs_used;
};

struct sas_alias
{
  const unsigned char *head;
  List *constraints;
  int global;
};

struct sas_constraint
{
  enum sc_type type;
  union sas_cunion
  {
    struct sas_cunion_env
    {
      unsigned char **pre;
      unsigned char **pos;
      int pre_len;
      int pos_len;
      int pre_neg;
      int pos_neg;
    } e;
    struct sas_cunion_lem
    {
      unsigned char *cf;
      unsigned char *gw;
      unsigned char *pos;
      int neg;
    } l;
  } u;
};

struct sas_map
{
  char*tmp_ptr_in_map0;
  const unsigned char *v;
  const unsigned char *a;
  int det; /* -1 = predet; 0 = nondet; +1 = postdet */
};

extern struct sas_info*sas_asa_load(const char *fname);
extern void sas_asa_unload(struct sas_info *sip);
extern unsigned char * sas_alias_form(struct sas_info *sip, const unsigned char *form, 
				      const unsigned char *cf, const unsigned char *gw,
				      const unsigned char *pos);
extern struct sas_map *sas_map_form(const unsigned char *form, int *maplen);

#endif/*_SAS_H*/
