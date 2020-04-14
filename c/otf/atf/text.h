#ifndef _TEXT_H
#define _TEXT_H
/*#define MAX_LINE_ID_BUF 1023*/
#include "blocktok.h"
#include "run.h"
extern int line_id;
extern unsigned char *text_n;
extern const char *textid;
extern char line_id_buf[];
extern char *line_id_insertp;
extern enum block_levels current_level;
extern const char *const xtf_xmlns[];
unsigned char **process_text(struct run_context *run, unsigned char **lines);
extern void text_vec_init(void);

extern void per_text_mem(void*);

extern enum e_type doctype;
#endif /*_TEXT_H*/
