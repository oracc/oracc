#include "cdt.h"
extern FILE *f_log;
extern int status;
extern const char *phase, *errmsg_fn;
void
cdt_warning(const char *myfile, size_t mylnum, const char *fmt,...)
{
  va_list args;
  va_start(args,fmt);
  fprintf(f_log,"%s:%d: ",errmsg_fn ? errmsg_fn : myfile,(int)mylnum);
  if (phase)
    fprintf(f_log,"(%s) ", phase);
  (void)vfprintf(f_log,fmt,args);
  va_end(args);
  (void)fputc('\n',f_log);
  ++status;
}
