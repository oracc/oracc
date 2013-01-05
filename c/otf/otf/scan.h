#ifndef _SCAN_H
#define _SCAN_H
#include <stdlib.h>
#include "cdt.h"
typedef int (*scan_block_tester)(const unsigned char *p);
unsigned char *skip_white(unsigned char *fext, size_t *lnump);
unsigned char *scan_token(unsigned char *tokp, unsigned char **endtokp, 
			  unsigned char *savep);
char* scan_curly(char *p, char **endp);
void scan_square(struct cdt_node *np, unsigned char *endtok, 
		 unsigned char **text_start, size_t *local_lnump);
void scan_text(struct cdt_node *np, unsigned char *text_start, 
	       unsigned char **text_end, size_t *local_lnump,
	       scan_block_tester f);
int block_peek(unsigned char *p, scan_block_tester f);
#endif/*_SCAN_H*/
