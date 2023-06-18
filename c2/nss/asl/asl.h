#ifndef ASL_H_
#define ASL_H_ 1

#include <tree.h>

extern int aslflextrace;

extern const char *curraslfile, *aslfile;

extern int asl_at_check(const char *atp);
extern int asl_grapheme(const char *gp);
extern Tree *aslyacc(void);
extern void asl_init(void);
extern void asl_term(void);
extern Tree *aslyacc(void);
extern int aslparse(void);

#endif
