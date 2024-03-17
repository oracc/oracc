#ifndef _FIELDS_H
#define _FIELDS_H

#include "types.h"

#define field_names \
  C(lemma) \
  C(guide) \
  C(morph) \
  C(sense)

#define C(x) x,
enum fields 
{ 
  guide = IF_GUIDE,
  lemma = IF_LEMMA,
  morph = IF_MORPH,
  morph2 = IF_MORPH2,
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
