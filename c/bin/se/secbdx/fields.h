#ifndef _FIELDS_H
#define _FIELDS_H


/*
 * c = cf
 * g = gw
 * p = pos
 * b = base
 * m = morph1
 * M = morph2
 * s = sense
 * t = transliteration (form)
 * l = lemma (CF[GW])
 * n = norm
 */
#define field_names				\
  C(sn_c)					\
  C(sn_g)					\
  C(sn_p)					\
  C(sn_b)					\
  C(sn_m)					\
  C(sn_M)					\
  C(sn_t)					\
  C(sn_s)					\
  C(sn_l)					\
  C(sn_n)

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
  const char *name;
  const char *alias;
};

struct sn_tab *statnames (register const char *str, register size_t len);
struct sn_alias_tab *sn_alias (register const char *str, register size_t len);

#endif /*_FIELDS_H*/
