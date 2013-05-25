#ifndef _LINKS_H
#define _LINKS_H

#include "xcl.h"

struct linkbase
{
  struct linkset *first;
  struct linkset *last;
  const char *textid;
  int idcounter;
  int nsets;
};

typedef void user_dump_func(FILE*,const void*);

struct linkset
{
  char xml_id[32];
  const char *role;
  const char *title;
  struct link *links;
  int used;
  int alloced;
  struct f2 *form;
  struct linkset *next;
  void *user;
  user_dump_func *user_dump_function;
  int chunkified; /* has this linkset been processed by xcl_chunkify? */
};

struct link
{
  const char *lref;
  struct xcl_l *lp;
  struct xcl_c *cp;
  const char *role;
  const char *title;
  void *user;
};

struct xcl_context;
struct ML;
extern void links_psu(struct xcl_context *xc,struct ML*mlp);

extern struct linkbase *new_linkbase(void);
extern struct linkset *new_linkset(struct linkbase *lbp,const char *role,
				   const char *title);
extern struct link *new_link(struct linkset *lsp, const char *role, 
			     const char *title);
extern void preallocate_links(struct linkset*lsp,int nlinks);
extern void links_dump(FILE*fp,struct linkbase *lbp);
extern struct xcl_chunk_spec *links_chunks(struct linkbase *lbp, 
					   int with_chunkified);
extern struct xcl_chunk_spec *links_chunks_part(struct linkbase *lbp,
						Hash_table *titles, 
						Hash_table *roles, 
						Hash_table *lnodes,
						int with_chunkified);
extern void linkbase_free(struct linkbase *lbp);

extern void links_serialize(FILE*fp,struct linkbase *lbp, int with_xml);

#endif /*_LINKS_H*/
