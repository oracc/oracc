#ifndef _FIELDS_H
#define _FIELDS_H


/*
 * c = cf
 * g = gw
 * p = pos
 * b = base
 * m1= morph1
 * m2= morph2
 * t = transliteration (form)
 * m = sense (meaning)
 * cg= cfgw (CF[GW])
 * n = norm
 *
 * Should probably implement Sum morph query as:
 *
 * ns1 ns2
 * vp1 vp2
 * vs1 vs2
 * is1 is2
 *
 */
#define field_names				\
  C(sn_c)					\
  C(sn_g)					\
  C(sn_p)					\
  C(sn_b)					\
  C(sn_m1)					\
  C(sn_m2)					\
  C(sn_t)					\
  C(sn_m)					\
  C(sn_cg)					\
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
