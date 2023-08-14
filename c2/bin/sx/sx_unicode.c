#include <signlist.h>
#include <sx.h>

void
sx_unicode(struct sl_signlist *sl)
{

  for (i = 0; i < sl->nsigns; ++i)
    {
      if (!sl->signs[i]->U.uchar && sl->signs[i]->U.ucode)
	sl->signs[i]->U.uchar = pool_copy(uhex2utf8((uccp)sl->signs[i]->U.ucode), sl->p);
    }

  for (i = 0; i < sl->nforms; ++i)
    {
      if (!sl->forms[i]->U.uchar && sl->forms[i]->U.ucode)
	sl->forms[i]->U.uchar = pool_copy(uhex2utf8((uccp)sl->forms[i]->U.ucode), sl->p);
    }

}
