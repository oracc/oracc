#ifndef _FILE_H
#define _FILE_H
#include "run.h"
extern int ods_mode;
extern const char *const xmlns[];
extern int process_file(struct run_context *run, const char *fname);
extern int process_string(struct run_context *run, unsigned char *ftext, ssize_t fsize);
extern void begin_file(void);
extern void end_file(void);
#endif
