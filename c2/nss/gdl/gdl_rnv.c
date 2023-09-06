#include <stdio.h>
#include <tree.h>
#include <rnvxml.h>
#include <gdl.h>

/**This module requires rnvif setup to have been done first
 *
 * The implementation here is very preliminary--just enough to support
 * the GDL that occurs in OGSL.
 */
static void
grx_node(Node *np)
{
  Node *npp;
  int need_closer = 1;
  const char *nodename = np->name;

  if (!strcmp(np->name, "g:b"))
    return;
  
  if (!strcmp(np->name, "g:l"))
    nodename = "g:s";
  else if (!strcmp(np->name, "g:gp"))
    nodename = "g:g";
  
  if (!strcmp(np->name, "g:d"))
    {
      const char *type = NULL;
      char text[2];
      text[0] = '\0';
      switch (*(unsigned char *)np->text)
	{
	case ':':
	  type = "reordered";
	  break;
	case '.':
	  type = "beside";
	  break;
	case '&':
	  type = "above";
	  break;
	case '@':
	  type = "opposing";
	  break;
	case '%':
	  type = "crossing";
	  break;
	case '+':
	  type = "joining";
	  break;
	case 0xc3:
	  type = "containing";
	  break;
	case '3':
	case '4':
	  type = "repeated";
	  *text = *np->text;
	  text[1] = '\0';
	  break;
	default:
	  fprintf(stderr, "unhandled character %c in delimiter node\n", *np->text);
	  break;
	}
      if (*text)
	rnvxml_et("g:o", rnvval_aa("x", "g:type", type, NULL), text);
      else
	rnvxml_ec("g:o", rnvval_aa("x", "g:type", type, NULL));
      need_closer = 0;
    }
  else if (!np->kids && (!strcmp(nodename, "g:s") || !strcmp(nodename, "g:v")
			 || !strcmp(nodename, "g:b") || !strcmp(nodename, "g:m")
			 || !strcmp(nodename, "g:a") || !strcmp(nodename, "g:p")
			 || !strcmp(nodename, "g:r")
			 ))
    {
      rnvxml_et(nodename, rnvval_aa_ccpp(prop_ccpp(np->props, GP_ATTRIBUTE, PG_GDL_INFO)),
		np->user ? (ccp)((gvl_g*)np->user)->orig : np->text);
      need_closer = 0;
    }
  else
    rnvxml_ea(nodename, rnvval_aa_ccpp(prop_ccpp(np->props, GP_ATTRIBUTE, PG_GDL_INFO)));

  for (npp = np->kids; npp; npp = npp->next)
    grx_node(npp);

  if (need_closer)
    rnvxml_ee(nodename);
}

void
grx_xml(Node *np, const char *gdltag)
{
  if (NULL == gdltag)
    gdltag = np->name;

  rnvxml_ea(gdltag, rnvval_aa_ccpp(prop_ccpp(np->props, GP_ATTRIBUTE, PG_GDL_INFO)));
  /* This can happen with gdl_literal; what to do in such cases? Print
     np->text as form attribute? */
  if (np->kids)
    grx_node(np->kids);
  rnvxml_ee(gdltag);
}
