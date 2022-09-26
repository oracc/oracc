#include "ox.h"
#include <stdarg.h>

/* Stub routines to make lib/oracc/warning.c happy; ox doesn't use this yet */
void
msglist_warning(const char *file, int ln, const char *str)
{
  ;
}

void
msglist_vwarning(const char *file, int ln, const char *str, va_list ap)
{
  ;
}
