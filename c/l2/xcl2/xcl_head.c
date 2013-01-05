#include "xcl.h"

struct xcl_l *
xcl_head(union xcl_u *xu)
{
  struct xcl_l *ret = NULL;
  int i;

  switch (xu->c->node_type)
    {
    case xcl_node_c:
      for (i = 0; i < xu->c->nchildren; ++i)
	{
	  if ((ret = xcl_head(xu)))
	    return ret;
	}
      break;
    case xcl_node_d:
      return NULL;
      break;
    case xcl_node_l:
      return xu->l;
      break;
    }
  return NULL;
}
