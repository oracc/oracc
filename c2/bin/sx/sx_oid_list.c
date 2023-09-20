#include <stdio.h>
#include <sx.h>

void
sx_oid_list(struct sl_signlist *sl)
{
  int i;
  for (i = 0; i < sl->nsigns; ++i)
    {
      struct sl_sign *s = sl->signs[i];
      if (s->type == sx_tle_sign && !s->xref && (!s->U.urev || strcmp(s->U.urev,"0")) && s->inst->valid)
	printf("%s\t%s\n", s->oid, s->name);
    }
  for (i = 0; i < sl->nforms; ++i)
    {
      struct sl_form *f = sl->forms[i];
      struct sl_inst *ip;
      if (!f->U.urev || strcmp(f->U.urev,"0"))
	{
	  for (ip = list_first(f->insts); ip; ip = list_next(f->insts))
	    {
	      if (ip->valid)
		{
		  printf("%s\t%s\n", f->oid, f->name);
		  break;
		}
	    }
	}
    }
}
