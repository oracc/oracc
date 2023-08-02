#include <stdio.h>
#include <tree.h>
#include <rnvxml.h>
#include <gdl.h>

/**This module requires rnvif setup to have been done first
 */
static void
grx_node(Node *np)
{
  Node *npp;
  int need_closer = 1;
  
  if (!np->kids && (!strcmp(np->name, "g:s") || !strcmp(np->name, "g:v") || !strcmp(np->name, "g:b") || !strcmp(np->name, "g:m")))
    {
      rnvxml_et(np->name, rnvval_aa_ccpp(prop_ccpp(np->props, GP_ATTRIBUTE, PG_GDL_INFO)),
		np->user ? (ccp)((gvl_g*)np->user)->orig : np->text);
      need_closer = 0;
    }
  else
    rnvxml_ea(np->name, rnvval_aa_ccpp(prop_ccpp(np->props, GP_ATTRIBUTE, PG_GDL_INFO)));

  for (npp = np->kids; npp; npp = npp->next)
    grx_node(npp);

  if (need_closer)
    rnvxml_ee(np->name);
}

void
grx_xml(Node *np, const char *gdltag)
{
  if (NULL == gdltag)
    gdltag = np->name;

  rnvxml_ea(gdltag, rnvval_aa_ccpp(prop_ccpp(np->props, GP_ATTRIBUTE, PG_GDL_INFO)));
  grx_node(np->kids);
  rnvxml_ee(gdltag);
}
