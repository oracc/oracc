#include "cdt.h"
#include "cdt_foreign.h"
void
cdt_data_handler(struct cdt_node *np, enum foreign_data_ops op, FILE *fp)
{
  struct cdt_foreign *fh = cdt_foreign_get_handlers(np->code);
  if (fh)
    {
      switch (op)
	{
	case op_read:
	  (void)fh->init(np->name);
	  fh->reader(np);
	  break;
	case op_write:
	  fh->writer(fp,np);
	  break;
	default:
	  break;
	}
    }
  else
    cdt_warning(np->file,np->lnum,"I don't know what to do with foreign data %s",np->name);
}
