#ifndef _FIELDS_H
#define _FIELDS_H

#define field_names				\
  C(sn_c)					\
  C(sn_g)					\
  C(sn_p)					\
  C(sn_t)					\
  C(sn_m)					\
  C(sn_l)					\
  C(sn_norm)

#define C(x) x,
enum fields 
{ 
  field_names
  nofield,
  not_in_use,
  next_uid
};

struct sn_tab
{
  const char *name;
  enum fields uid;
};

struct sn_alias_tab
{
  const char *alias;
  const char *name;
};

struct sn_tab *statnames (register const char *str, register unsigned int len);
struct sn_alias_tab *sn_alias (register const char *str, register unsigned int len);

#endif /*_FIELDS_H*/
