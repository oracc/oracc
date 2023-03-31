#include <stdio.h>
#include <string.h>
#include "oraccsys.h"

static unsigned char ret[4096];

#define szargal_str "(šar₂{gal})"
#define szaru_str   "(šaru)"
#define szar2_str   "(šar₂)"
#define geszu_str   "(gešʾu)"
#define geszd_str   "(geš₂)"
#define u_str       "(u)"
#define disz_str    "(diš)"
#define asz_str     "(aš)"

static void
append(unsigned char *buf,int i,const char *str)
{
  if (*buf)
    strcat((char*)buf," ");
  sprintf((char*)buf+strlen((char*)buf),"%d%s",i,str);
}

unsigned char *
sexify(int n, const char *aszdisz)
{
  int szargal = 0, szaru = 0, szar2 = 0, geszu = 0, geszd = 0, u = 0, disz = 0;
  if (n <= 0)
    return NULL;
  
  szargal = n / 216000;
  n %= 216000;
  
  szaru = n / 36000;
  n %= 36000;
  
  szar2 = n / 3600;
  n %= 3600;
  
  geszu = n / 600;
  n %= 600;
  
  geszd = n / 60;
  n %= 60;
  
  u = n / 10;
  n %= 10;
  
  disz = n;
  
  *ret = '\0';

  if (szargal)
    sprintf((char*)ret,"%d%s",szargal,szargal_str);
  if (szaru)
    append(ret,szaru,szaru_str);
  if (szar2)
    append(ret,szar2,szar2_str);
  if (geszu)
    append(ret,geszu,geszu_str);
  if (geszd)
    append(ret,geszd,geszd_str);
  if (u)
    append(ret,u,u_str);
  if (disz)
    {
      if (*aszdisz == 'a')
	append(ret,disz,asz_str);
      else
	append(ret,disz,disz_str);
    }
  
  return &ret[0];
}
