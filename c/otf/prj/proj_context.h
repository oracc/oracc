#ifndef PROJ_CONTEXT_
#define PROJ_CONTEXT_

#include "hash.h"

struct proj_context
{
  char *name;			/* project name, e.g., saao/saa01 */
  struct xpd *xpd;
  Hash_table *langs;		/* languages used in project; keys are
				   langtags values are lang_context */
  int tried_qpn;	   	/* did we already try PNs for this project? */
  struct run_context *owner;
};

extern struct proj_context *curr_project;

/* Each project in a run gets an entry in here pointing to its
   proj_context */
extern Hash_table *known_projects;

extern void proj_init(struct run_context *runp, const char *project);
extern void proj_term(struct proj_context *);

#endif/*PROJ_CONTEXT_*/
