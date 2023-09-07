#ifndef _ATF_H
#define _ATF_H
#include <stddef.h>
#include <sys/types.h>

#include <mesg.h>
#include <unidef.h>

extern unsigned char *atf2utf(Mloc *locp, register const unsigned char *s, int rx);
extern void atf2utf_init(void);
extern unsigned char *Unicode2UTF (unsigned short ch);

#endif /*ATF_H*/
