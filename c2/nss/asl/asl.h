#ifndef ASL_H_
#define ASL_H_ 1

#include <tree.h>

#include "signlist.h"

extern struct sl_signlist *curr_asl;
extern int asl_raw_tokens;
extern int aslflextrace;

extern const char *curraslfile, *aslfile;

extern int asl_at_check(const char *atp);
extern int asl_grapheme(const char *gp);
extern struct sl_signlist *aslyacc(const char *file);
extern void asl_init(void);
extern void asl_term(void);
extern int aslparse(void);
extern char *longtext(struct sl_signlist *sl, char *t, char *a);
extern Hash *asl_get_oids(void);

#endif
