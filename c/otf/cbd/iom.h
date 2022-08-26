#ifndef _IOM_H
#define _IOM_H

struct iom_io {
  const char *fn;
  FILE *fp;
  const char *str;
  int use_expat;
};

typedef struct cbd * (*iom_i_fnc)(struct iom_io *);
typedef void (*iom_o_fnc)(struct cbd *, struct iom_io *);

enum iomtypes {
  iom_tg1 , iom_tg2,
  iom_xg1 , iom_xg2,
  iom_x11 , iom_x12,
  iom_x21 , iom_x22,
  iom_x31 , iom_x32,
  iom_htm ,
  iom_jsn ,
};

struct iom {
  const char *name;
  enum iomtypes type;
  const char *uri;
  const char *desc;
  iom_i_fnc ifnc;
  iom_o_fnc ofnc;
};

#endif/*_IOM_H*/
