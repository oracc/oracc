#include <ctype128.h>
#include <tree.h>
#include <cat.h>
#include <gdl.h>
#include "ax.h"

void
ax_parse(Node *np, char *data)
{
  ;
}

/* This routine is passed the following types:

   LINE
   SIGLUM

   in each case, the first non-space token is the line-type and is
   stored in np->user; the rest is GDL.
 */
void
ax_gparse(Node *np, char *data)
{
  Tree *tp = NULL;
  char *s = data;
  np->user = s;
  while (*s && !isspace(*s))
    ++s;
  if (s > data && (s[-1] == '.' || s[-1] == ':'))
    s[-1] = '\0';
  while (isspace(*s))
    ++s;
  /*fprintf(stderr, "ax_gparse called with lnum=%d\n", np->mloc->line);*/
  tp = gdlparse_string(np->mloc, s);
  tree_graft(np, tp);
  gdlparse_reset();  
}

#if 0
/* This routine needs to wrap the preamble and/or postamble that some
   @v nodes have in (#...#) or the like before passing them to the gdl
   parser */   
void
sx_vpp_parse(Node *np, char *data)
{
  sx_gparse(np, data);
}
#endif
