#ifndef C1C2GVL_H_
#define C1C2GVL_H_

/* gvl routines analagous to old psl/gsl routines */
extern int gvl_is_sname(unsigned const char *g);
extern int gvl_is_value(unsigned const char *g);
extern unsigned const char *gvl_get_id(unsigned const char *g);
extern unsigned const char *gvl_get_sname(unsigned const char *g);
extern int gvl_looks_like_sname(unsigned const char *g);
extern unsigned const char *gvl_cuneify(unsigned const char *g);

/* gvl_legacy.c routines */
extern int gvl_psl_lookup(unsigned const char *g);
extern const char *gvl_legacy(const char *f,size_t l,const unsigned char *g,int t);

#endif/*C1C2GVL_H_*/
