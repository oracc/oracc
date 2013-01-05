#include "xcl.h"

static void
xcl_map_one(struct xcl_context *xc, struct xcl_c *cp, 
	    xcl_c_fnc cfnc_o, xcl_c_fnc cfnc_c, xcl_d_fnc dfnc, xcl_l_fnc lfnc)
{
  int i;
  if (!cp)
    return;
  if (cfnc_o)
    cfnc_o(xc,cp);
  for (i = 0; i < cp->nchildren; ++i)
    {
      switch (cp->children[i].c->node_type)
	{
	case xcl_node_c:
	  xcl_map_one(xc,cp->children[i].c, cfnc_o, cfnc_c, dfnc, lfnc);
	  break;
	case xcl_node_d:
	  if (dfnc)
	    dfnc(xc,cp->children[i].d);
	  break;
	case xcl_node_l:
	  if (lfnc)
	    lfnc(xc,cp->children[i].l);
	  break;
	}
    }
  if (cfnc_c)
    cfnc_c(xc,cp);
}

void
xcl_map(struct xcl_context *xc, xcl_c_fnc cfnc_o, xcl_c_fnc cfnc_c, 
	xcl_d_fnc dfnc, xcl_l_fnc lfnc)
{
  if (!xc)
    return;
  xcl_map_one(xc,xc->root,cfnc_o,cfnc_c,dfnc,lfnc);
}
