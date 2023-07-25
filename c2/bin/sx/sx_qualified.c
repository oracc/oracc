#include <list.h>
#include <signlist.h>
#include <sx.h>

extern int oid_char_cmp(const void *a, const void *b);

void
sx_qualified(struct sl_signlist *sl)
{
  int i;
  /* Gather the valid parents--both value and sign/form need to be marked valid */
  for (i = 0; i < sl->nvalues; ++i)
    {
      struct sl_parents *p = sl->values[i]->parents;
      int nqvoids = 0, j;
      const char **qvoids = NULL;
      struct sl_inst *ip;
      if (sl->values[i]->xvalue)
	sl->values[i]->qvsign = sl->values[i]->qvform = 1;
      else
	{
	  if (p)
	    {
	      if (p->signs)
		{
		  if (list_len(p->signs) > 1)
		    sl->values[i]->qvsign = 1;
		}
	      if (p->forms)
		{
		  if (list_len(p->signs) > 1)
		    sl->values[i]->qvsign = 1;
		}
	    }
	}
      if (sl->values[i]->qvsign)
	nqvoids = list_len(p->signs);
      if (sl->values[i]->qvform)
	nqvoids += list_len(p->forms);
      qvoids = malloc((1+nqvoids)*sizeof(const char *));
      if (sl->values[i]->qvsign)
	for (ip = list_first(p->signs); ip; ip = list_next(p->signs))
	  qvoids[j++] = ip->u.s->oid;
      if (sl->values[i]->qvform)
	for (ip = list_first(p->forms); ip; ip = list_next(p->forms))
	  qvoids[j++] = ip->u.f->oid;
      qvoids[j] = NULL;
      qsort(qvoids, j, sizeof(const char *), oid_char_cmp);
      p->qvoids = qvoids;
    }
}
