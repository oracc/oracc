#ifndef _PROTOCOLS_H
#define _PROTOCOLS_H
#include "tree.h"

#define C(x) x,
#define T_SCOPES C(s_global)C(s_file)C(s_text)C(s_inter)C(s_intra)
enum t_scope { T_SCOPES scopes_top };
#undef C

#include "lang.h"
#include "run.h"

extern int word_matrix;

extern const char *const scope_names[];
extern enum t_scope protocol_state;

extern struct node *protocols(struct run_context *run,
			      enum t_scope scope, enum block_levels level, 
			      unsigned char **lines, unsigned char ***end,
			      struct node *np);
extern int protocol(struct run_context *run,
		    enum t_scope scope, enum block_levels level,
		    struct node *proto, unsigned char *line);

#endif /*_PROTOCOLS_H*/
