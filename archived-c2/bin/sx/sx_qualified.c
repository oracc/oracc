#include <list.h>
#include <signlist.h>
#include <sx.h>

extern int oid_char_cmp(const void *a, const void *b);

/* Values that occur under more than one sign or form may need to be
   qualified with the sign or form name.

   (1) All x-values must be qualified.

   (2) If a value has a singleton sign parent, the unqualified version of
   the value refers to the value(SIGN).

   (3) A value can only have multiple sign parents if it is an x-value--see above.
   
   (4) If a value has a singleton form parent and no sign parent the
   unqualified version of the value refers to the value(FORM).

   (5) If a value has no sign parent and multiple form parents and value
   must be qualified.
 */
void
sx_qualified(struct sl_signlist *sl)
{
  int i;
  /* Gather the valid parents--both value and sign/form need to be marked valid */
  for (i = 0; i < sl->nvalues; ++i)
    {
      if (!sl->values[i]->atf)
	{
	  struct sl_parents *p = sl->values[i]->parents;
	  int nqvoids = 0, j = 0;
	  const char **qvoids = NULL;
	  struct sl_inst *ip;
	  if (sl->values[i]->xvalue)
	    {
	      sl->values[i]->qvform
		= sl->values[i]->qvsign
		= sl->values[i]->qvmust = 1;	/* Must case (1) */
	    }
	  else
	    {
	      if (p)
		{
#if 0
		  /* This can only happen with x-values, handled above */
		  if (p->signs)
		    {
		      if (list_len(p->signs) > 1)
			sl->values[i]->qvmust = sl->values[i]->qvsign = 1;
		    }
#endif
		  if (p->forms)
		    {
		      if (list_len(p->signs) || list_len(p->forms) > 1)
			sl->values[i]->qvform = 1;

		      if (0 == list_len(p->signs) && list_len(p->forms) > 1) /* Must case (5) */
			sl->values[i]->qvmust = 1;
		    }
		}
	    }
	  if (sl->values[i]->qvsign)
	    nqvoids = list_len(p->signs);
	  if (sl->values[i]->qvform)
	    nqvoids += list_len(p->forms);
	  if (nqvoids)
	    {
	      qvoids = malloc((1+nqvoids)*sizeof(const char *));
	      if (sl->values[i]->qvsign)
		for (ip = list_first(p->signs); ip; ip = list_next(p->signs))
		  qvoids[j++] = ip->u.s->oid;
	      if (sl->values[i]->qvform)
		for (ip = list_first(p->forms); ip; ip = list_next(p->forms))
		  qvoids[j++] = ip->u.f->oid;
	      qvoids[j] = NULL;
	      if (j > 0)
		qsort(qvoids, j, sizeof(const char *), oid_char_cmp);
	    }
	  if (p)
	    p->qvoids = qvoids;
	  else
	    if (qvoids)
	      fprintf(stderr, "sx: internal error: found qvoids but parent is NULL; v=%s\n", sl->values[i]->name);
	}
    }
}
