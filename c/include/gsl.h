#ifndef _GSL_H
#define _GSL_H
#include "sas.h"
extern void gsl_map_ids(struct sas_map *m);
extern unsigned char *psl_bounded_sname(const unsigned char *v);
extern const unsigned char *psl_get_sname(const unsigned char *value);
extern const char *psl_get_id(const unsigned char *v);
extern void psl_init(void);
extern void psl_term(void);
extern const unsigned char *psl_cuneify(const unsigned char *g);
extern unsigned char *psl_hex_to_sign(const char *hex);
extern int psl_is_sname(const unsigned char *sname);
extern int psl_is_value(const unsigned char *value);
extern int psl_looks_like_sname(const unsigned char *str);
#endif/*_GSL_H*/
