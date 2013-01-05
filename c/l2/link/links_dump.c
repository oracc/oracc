#include <stdlib.h>
#include <stdio.h>
#include "f2.h"
#include "links.h"

struct nsa_xcl_data;
extern void nsa_xcl_dump(FILE *,struct nsa_xcl_data *);
static const char *xlink_ns = "http://www.w3.org/1999/xlink";

void
linkset_dump(FILE*fp,struct linkset *lsp)
{
  int i;
  fprintf(fp,"<linkset xl:title=\"%s\" xml:id=\"%s\" xl:type=\"extended\" xl:role=\"%s\"",
	  lsp->title,lsp->xml_id,lsp->role);
  if (lsp->form && lsp->form->sig)
    fprintf(fp, " sig=\"%s\"", lsp->form->sig);
  fputc('>',fp);
  for (i = 0; i < lsp->used; ++i)
    {
      fprintf(fp,"<link xl:title=\"%s\" xl:type=\"locator\" xl:href=\"#%s\" xl:role=\"%s\">",
	      lsp->links[i].title,
	      lsp->links[i].lref,
	      lsp->links[i].role
	      );
      fprintf(fp,"</link>");
    }
#if 0
  if (lsp->form)
    f2_serialize_form_2(fp,lsp->form);/*FIXME: why do we need two f2_serialize_forms?*/
#endif
  if (lsp->user_dump_function)
    lsp->user_dump_function(fp,lsp->user);
  fprintf(fp,"</linkset>");
}

void
links_dump(FILE*fp,struct linkbase *lbp)
{
  struct linkset *lsp;
  if (lbp && lbp->first)
    {
      fprintf(fp,"<linkbase xmlns:xl=\"%s\">",xlink_ns);
      for (lsp = lbp->first; lsp; lsp = lsp->next)
	linkset_dump(fp,lsp);
      fprintf(fp,"</linkbase>");
    }
}
