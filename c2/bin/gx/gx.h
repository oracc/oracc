#ifndef GX_H_
#define GX_H_ 1

#include <cbd.h>

typedef void (*iterator_fnc)(void*);
extern Hash *cbds;

extern int check;
extern int entries;
extern int keepgoing;
extern int sigs;

extern int lnum;
extern int verbose;

extern const char *file;

extern FILE *f_xml;

extern void o_tg2(struct cbd*c);
extern void o_xc2(struct cbd*c);
extern void validator(struct cbd*c);

extern void common_init(void);
extern void common_term(void);
extern void untab(unsigned char *s);
extern unsigned char *tok(unsigned char *s, unsigned char *end);
extern unsigned char *check_bom(unsigned char *s);
extern unsigned char **setup_lines(unsigned char *ftext);
extern void cgp_init(struct cgp *c, unsigned char *cf, unsigned char *gw, unsigned char *pos);
extern unsigned const char *cgp_entry_str(struct entry *e, int spread);
extern const unsigned char *cgp_str(struct cgp *cp, int spread);
extern void cgp_entry(struct cgp *c, struct entry *e);
extern unsigned char *slurp(const char *caller, const char *fname, ssize_t *fsize);

#endif/*GX_H_*/
