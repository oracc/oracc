#include <stdio.h>
#undef vwarning
#define vwarning(a,b) fprintf(stderr,a,b)
