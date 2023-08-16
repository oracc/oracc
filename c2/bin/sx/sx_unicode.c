#include <oraccsys.h>
#include <signlist.h>
#include <sx.h>

void
sx_unicode(struct sl_signlist *sl)
{
  Hash *usigns = hash_create(1024);
  /* Index the signs that have unames -- those are encoded so we treat
     them as atoms even if they are compounds */
  int i;  
  for (i = 0; i < sl->nsigns; ++i)
    {
      struct sl_unicode *Up = (sl->signs[i]->xref ? &sl->signs[i]->xref->U : &sl->signs[i]->U);
      if (Up->uname)
	{
	  if (Up->uhex)
	    hash_add(usigns, (uccp)Up->uname, (ucp)Up->uhex);
	  else
	    mesg_verr(&sl->signs[i]->inst->mloc, "sign %s has uname but no uhex\n", sl->signs[i]->name);
	}
    }
  
  /* check the compounds by building ucode sequences for them:
   *   if there is one in the signlist already, check that the newly built ucode is the same
   *   else set the sign's U.ucode appropriately
   */
  struct sl_inst *ip;
  for (ip = list_first(sl->compounds); ip; ip = list_next(sl->compounds))
    {
#if 0
      unsigned const char *name = ip->type == 's' ? ip->u.s->name : ip->u.f->name;
      struct sl_token *tp = hash_find(sl->htoken, name);
      if (tp->gsig)
	{
	  fprintf(stderr, "sx_unicode: building @ucode for %s from sig %s\n", name, tp->gsig);
	}
#endif
    }
  for (i = 0; i < sl->nsigns; ++i)
    {
      if (!sl->signs[i]->U.utf8)
	{
	  if (sl->signs[i]->U.uhex)
	    sl->signs[i]->U.utf8 = pool_copy(uhex2utf8((uccp)sl->signs[i]->U.uhex), sl->p);
	  else if (sl->signs[i]->U.useq)
	    sl->signs[i]->U.utf8 = pool_copy(uhex2utf8((uccp)sl->signs[i]->U.useq), sl->p);
	}
    }

  for (i = 0; i < sl->nforms; ++i)
    {
      if (!sl->forms[i]->U.utf8)
	{
	  if (sl->forms[i]->U.uhex)
	    sl->forms[i]->U.utf8 = pool_copy(uhex2utf8((uccp)sl->forms[i]->U.uhex), sl->p);
	  else if (sl->forms[i]->U.useq)
	    sl->forms[i]->U.utf8 = pool_copy(uhex2utf8((uccp)sl->forms[i]->U.useq), sl->p);
	}
    }

}
