#ifndef _NOCORE_H
#define _NOCORE_H
#include <stdio.h>
extern const char *prog;
#define nocore() fprintf(stderr,"%s: out of memory\n",prog),exit(-1)
#endif /*_NOCORE_H*/
