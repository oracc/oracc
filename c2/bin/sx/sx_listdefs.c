#include <signlist.h>
#include <sx.h>

static void
sx_listdefs_one(struct sl_signlist *sl, const char *listname)
{
  struct sl_listdef *ldp = hash_find(sl->listdefs, (uccp)listname);

  if (ldp)
    {
      /* sort the list's entries */
      qsort(ldp->names, ldp->nnames, sizeof(const char *), toks_cmp);
      int i;
      for (i = 0; i < ldp->nnames; ++i)
	if (!hash_find(ldp->seen, (uccp)ldp->names[i]))
	  mesg_verr(sl->mloc, "list name %s not found in signlist", ldp->names[i]);
    }
  else
    mesg_verr(sl->mloc, "request to check missing items in unknown list %s", listname);
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
      while (',' != *n)
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
  if (listnames)
    {
      int ncomma = sx_ld_count(listnames), i;
      char *n, *ln;
      ln = (char*)pool_copy((uccp)listnames, sl->p);
      names = malloc(ncomma * sizeof(const char *));
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
    }
  else
    {
      names = hash_keys(sl->listdefs);
    }

  int i;
  for (i = 0; names[i]; ++i)
    sx_listdefs_one(sl, names[i]);

  free(names);
}
