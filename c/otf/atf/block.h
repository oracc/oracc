#ifndef _BLOCK_H
#define _BLOCK_H

#include "run.h"
#include "tree.h"

#define NO_WORD_LIST   0
#define WITH_WORD_LIST 1

extern int div_level;
extern unsigned char line_label_buf[];

extern const char * const roman[];
extern unsigned char **parse_block(struct run_context *run, struct node *text,unsigned char **lines);
extern struct node* scan_comment(unsigned char **lines,int *nlines,int badcolon);
extern unsigned char **skip_rest(unsigned char **lines);
#endif /*_BLOCK_H*/
