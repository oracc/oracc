#ifndef ATF_H_
#define ATF_H_

#include "cat.h"

extern int atfparse(void); /* bison */
extern char *atf_name(struct catchunk *cp, char **data);
extern struct catchunk *atf_read(const char *);
extern struct catchunk *atfyacc(void);
extern void atf_lex_init(FILE *fp);

#endif/*ATF_H_*/
