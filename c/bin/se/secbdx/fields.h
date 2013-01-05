#ifndef _FIELDS_H
#define _FIELDS_H

#define field_names				\
  C(sn_cf)					\
  C(sn_gw)					\
  C(sn_t)					\
  C(sn_mng)					\
  C(sn_mean)					\
  C(sn_cfgw)					\
  C(sn_term)					\
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

struct sn_tab *statnames (register const char *str, register unsigned int len);

#endif /*_FIELDS_H*/
