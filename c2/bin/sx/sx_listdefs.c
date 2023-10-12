#include <signlist.h>
#include <sx.h>

void
sx_listdefs_sort(struct sl_listdef *ldp)
{
  qsort(ldp->names, ldp->nnames, sizeof(const char *), via_tok_cmp);
}

static void
sx_listdefs_one(struct sl_signlist *sl, const char *listname)
{
  struct sl_listdef *ldp = hash_find(sl->listdefs, (uccp)listname);

  if (ldp)
    {
      /* sort the list's entries if necessary */
      if (!ldp->sorted++)
	sx_listdefs_sort(ldp);
      int i;
      for (i = 0; i < ldp->nnames; ++i)
	if (!hash_find(ldp->seen, (uccp)ldp->names[i]))
	  fprintf(stdout,"%s\n", ldp->names[i]);
    }
  else
    mesg_verr(&sl->mloc, "request to check missing items in unknown list %s", listname);
}

/**sx_ld_count
 *
 * Count commas in string, counting sequences of multiple commas as a
 * single comma.
 */
static int
sx_ld_count(const char *n)
{
  int i;
  for (i = 0; *n; ++i)
    {
      while (',' == *n)
	++n;
      while (*n && ',' != *n)
	++n;
    }
  return i;
}

/**sx_listdefs
 *
 * At the end of signlist processing, if -M or -m LISTS is given,
 * report on which listnums were declared in listdefs but didn't occur
 * in the signlist.
 *
 * argument listnames is a comma-separated list of names
 */
void
sx_listdefs(struct sl_signlist *sl, const char *listnames)  
{
  const char **names;
  int nnames;
  if (listnames)
    {
      int ncomma = sx_ld_count(listnames), i;
      char *n, *ln;
      ln = (char*)pool_copy((uccp)listnames, sl->p);
      names = malloc((ncomma+1) * sizeof(const char *));
      names[0] = ln;
      for (n = ln, i = 1; *n; ++n)
	{
	  if (',' == *n)
	    {
	      *n++ = '\0';
	      /* trap listnames ending with comma */
	      while (',' == *n)
		++n;
	      if ('\0' == *n)
		break;
	      names[i++] = n;
	    }
	}
      names[i] = NULL;
      nnames = i;
    }
  else
    {
      names = hash_keys2(sl->listdefs, &nnames);
    }

  qsort(names, nnames, sizeof(const char *), cmpstringp);  
  
  int i;
  for (i = 0; names[i]; ++i)
    sx_listdefs_one(sl, names[i]);

  free(names);
}

/* Dump either the names in @listdef tags or the numbers defined in
   all the listdefs */
void
sx_list_dump(FILE *f, struct sl_signlist *sl)  
{
  extern int list_names_mode;
  const char **n = NULL;
  int nn;
  n = hash_keys2(sl->listdefs, &nn);
  qsort(n, nn, sizeof(const char *), cmpstringp);
  int i;
  for (i = 0; n[i]; ++i)
    {
      if (list_names_mode)
	fprintf(stdout, "%s\n", n[i]);
      else
	{
	  struct sl_listdef *ldp = hash_find(sl->listdefs, (uccp)n[i]);
	  /* sort the list's entries if necessary */
	  if (!ldp->sorted++)
	    sx_listdefs_sort(ldp);
	  int j;
	  for (j = 0; ldp->names[j]; ++j)
	    {
	      struct sl_list *lp = hash_find(sl->hlentry, (uccp)ldp->names[j]);
	      if (lp)
		{
		  struct sl_inst *ip;
		  unsigned const char *name = (uccp)"", *note = (uccp)"";
		  if (lp->insts)
		    {
		      for (ip = list_first(lp->insts); ip; ip = list_next(lp->insts))
			{
			  if (lp->type == sl_ll_list)
			    name = (ip->type == 's') ? ip->u.s->name : ip->u.f->name;
			  else if (lp->type == sl_ll_lref)
			    mesg_verr(&lp->inst->mloc, "strange: @lref has instances more than just its own (this can't happen)");
			  else
			    mesg_verr(&ip->mloc, "untyped @list or @lref");
			  fprintf(f, "%s\t%s\t%s\t%s\n", ldp->names[j],
				  ip->type == 's' ? ip->u.s->oid : ip->u.f->oid,
				  name , note);
			}
		    }
		  else if (lp->inst)
		    {
		      if (lp->type == sl_ll_list)
			mesg_verr(&lp->inst->mloc, "strange: @list has no instances (this can't happen)");
		      else if (lp->type == sl_ll_lref)
			{
			  if (lp->inst->notes)
			    {
			      note = (uccp)((struct sl_note*)list_first(lp->inst->notes))->txt;
			      if (note && *note)
				{
				  if ('-' == *note && '\0' == note[1])
				    note = (uccp)"";
				}
			      else
				mesg_verr(&lp->inst->mloc, "strange: @lref @note is NULL");
			    }
			  else
			    mesg_verr(&lp->inst->mloc, "@lref requires @note (use '@note -' to suppress this message)");
			}
		      else
			mesg_verr(&ip->mloc, "untyped @list or @lref");
		      fprintf(f, "%s\t%s\t%s\t%s\n", ldp->names[j], "", name , note);
		    }
		}
	      else
		{
		  mesg_verr(&sl->mloc, "list entry %s missing (no @list or @lref)", ldp->names[j]);
		  fprintf(f, "%s\t\t\t\n", ldp->names[j]);
		}
	    }
	}
    }
}
