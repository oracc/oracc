#ifndef IOME_H_
#define IOME_H_

struct iome_io {
  const char *fn;
  FILE *fp;
  char *str;
  char *normed;
  int use_expat;
  int use_stdio;
  size_t len; /* len of str for use with flex; == strlen(str)+2 bc
		 yy_scan_buffer needs double final \0 */
};

typedef struct iome_io Iome_io;

typedef void * (*iome_i_fnc)(Iome_io *);
typedef void (*iome_o_fnc)(void *, Iome_io *);

enum iome_types {
  iome_tg1 , iome_tg2,
  iome_xc1 , iome_xc2,
  iome_x11 , iome_x12,
  iome_x21 , iome_x22,
  iome_x31 , iome_x32,
  iome_gdl ,
  iome_atf , iome_xtf ,
  iome_xtr ,
  iome_xpd ,
  iome_tmd , iome_xmd , /* tabular md, xml md */
  iome_asl , iome_xsl , /* ascii signlist , xml signlist */
  iome_htm ,
  iome_jsn 
};

struct iome {
  const char *name;
  enum iome_types type;
  const char *uri;
  const char *desc;
  iome_i_fnc ifnc;
  iome_o_fnc ofnc;
};

typedef struct iome Iome;

#endif/*IOME_H_*/
