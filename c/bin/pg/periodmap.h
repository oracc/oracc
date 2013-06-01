#ifndef PERIOD_MAP_
#define PERIOD_MAP_
enum period_code { PC, EDa, EDb, Eb, OAk, LA, UR, OB, LS, XX, PCODE_MAX };
struct periodmap 
{
  const char *name;
  enum period_code p;
};
extern struct periodmap *periodmap(register const char *str, register unsigned int len);

#endif/*PERIOD_MAP_*/
