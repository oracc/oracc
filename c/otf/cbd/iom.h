#ifndef _IOM_H
#define _IOM_H

struct iom_io {
  const char *fn;
  FILE *fp;
  char *str;
  char *normed;
  int use_expat;
  int use_stdio;
  size_t len; /* len of str for use with flex; == strlen(str)+2 bc
		 yy_scan_buffer needs double final \0 */
};

typedef struct cbd * (*iom_i_fnc)(struct iom_io *);
typedef void (*iom_o_fnc)(struct cbd *, struct iom_io *);

enum iomtypes {
  iom_tg1 , iom_tg2,
  iom_xc1 , iom_xc2,
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
