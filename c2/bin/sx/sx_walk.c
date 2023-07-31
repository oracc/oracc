#include <signlist.h>
#include <sx.h>

/** Walk the signlist letter/group structure and apply the handler
    routines that are passed in via \c sl_functions.
 */
void
sx_walk(struct sx_functions *f, struct sl_signlist *sl)
{
  f->sll(f, sl, sx_pos_init);
  f->not(f, sl, sl->notes);
  if (sl->nletters)
    {
      int i;

      f->let(f, sl, NULL, sx_pos_init);
      for (i = 0; i < sl->nletters; ++i)
	{
	  f->let(f, sl, &sl->letters[i], sx_pos_inst);
	  if (sl->letters[i].ngroups)
	    {
	      int j;
	      f->grp(f, sl, NULL, sx_pos_init);
	      for (j = 0; j < sl->letters[i].ngroups; ++j)
		{
		  f->grp(f, sl, &sl->letters[i].groups[j], sx_pos_inst);
		  if (sl->letters[i].groups[j].nsigns)
		    {
#define QV(vp) ((vp)->u.v->qvsign || (vp)->u.v->qvform || (vp)->u.v->qvmust)
		      List *qv = list_create(LIST_SINGLE);
		      int k;
		      f->sgn(f, sl, NULL, sx_pos_init);
		      for (k = 0; k < sl->letters[i].groups[j].nsigns; ++k)
			{
			  f->sgn(f, sl, sl->letters[i].groups[j].signs[k], sx_pos_inst);
			  if ('s' == sl->letters[i].groups[j].signs[k]->type)
			    {
			      f->not(f, sl, sl->letters[i].groups[j].signs[k]);
			      f->uni(f, sl, &sl->letters[i].groups[j].signs[k]->u.s->U);
			      if (sl->letters[i].groups[j].signs[k]->u.s->nlists)
				{
				  int l;
				  f->lst(f, sl, NULL, sx_pos_init);
				  for (l = 0; l < sl->letters[i].groups[j].signs[k]->u.s->nlists; ++l)
				    {
				      f->lst(f, sl, sl->letters[i].groups[j].signs[k]->u.s->lists[l], sx_pos_inst);
				      f->not(f, sl, sl->letters[i].groups[j].signs[k]->u.s->lists[l]);
				    }
				  f->lst(f, sl, NULL, sx_pos_term);
				}
			      if (sl->letters[i].groups[j].signs[k]->u.s->nvalues)
				{
				  int l;
				  f->val(f, sl, NULL, sx_pos_init);
				  for (l = 0; l < sl->letters[i].groups[j].signs[k]->u.s->nvalues; ++l)
				    {
				      f->val(f, sl, sl->letters[i].groups[j].signs[k]->u.s->values[l], sx_pos_inst);
				      if (QV(sl->letters[i].groups[j].signs[k]->u.s->values[l]))
					list_add(qv, sl->letters[i].groups[j].signs[k]->u.s->values[l]);
				      f->not(f, sl, sl->letters[i].groups[j].signs[k]->u.s->values[l]);
				    }
				  f->val(f, sl, NULL, sx_pos_term);
				}
			      if (sl->letters[i].groups[j].signs[k]->u.s->nforms)
				{
				  int l;
				  f->frm(f, sl, NULL, sx_pos_init);
				  for (l = 0; l < sl->letters[i].groups[j].signs[k]->u.s->nforms; ++l)
				    {
				      f->frm(f, sl, sl->letters[i].groups[j].signs[k]->u.s->forms[l], sx_pos_inst);
				      f->not(f, sl, sl->letters[i].groups[j].signs[k]->u.s->forms[l]);
				      f->uni(f, sl, &sl->letters[i].groups[j].signs[k]->u.s->forms[l]->u.f->U);
				      if (sl->letters[i].groups[j].signs[k]->u.s->forms[l]->lv->nlists)
					{
					  int m;
					  f->lst(f, sl, NULL, sx_pos_init);
					  for (m = 0; m < sl->letters[i].groups[j].signs[k]->u.s->forms[l]->lv->nlists; ++m)
					    {
					      f->lst(f, sl, sl->letters[i].groups[j].signs[k]->u.s->forms[l]->lv->lists[m],
						     sx_pos_inst);
					      f->not(f, sl, sl->letters[i].groups[j].signs[k]->u.s->forms[l]->lv->lists[m]);
					    }
					  f->lst(f, sl, NULL, sx_pos_term);
					}
				      if (sl->letters[i].groups[j].signs[k]->u.s->forms[l]->lv->nvalues)
					{
					  int m;
					  f->val(f, sl, NULL, sx_pos_init);
					  for (m = 0; m < sl->letters[i].groups[j].signs[k]->u.s->forms[l]->lv->nvalues; ++m)
					    {
					      f->val(f, sl, sl->letters[i].groups[j].signs[k]->u.s->forms[l]->lv->values[m],
						     sx_pos_inst);
					      f->not(f, sl, sl->letters[i].groups[j].signs[k]->u.s->forms[l]->lv->values[m]);
					      if (QV(sl->letters[i].groups[j].signs[k]->u.s->forms[l]->lv->values[m]))
						list_add(qv, sl->letters[i].groups[j].signs[k]->u.s->forms[l]->lv->values[m]);
					    }
					  f->val(f, sl, NULL, sx_pos_term);
					}
				      if (sl->letters[i].groups[j].signs[k]->u.s->forms[l]->lv->nivalues)
					{
					  int m;
					  f->inh(f, sl, NULL, sx_pos_init);
					  for (m = 0; l < sl->letters[i].groups[j].signs[k]->u.s->forms[l]->lv->nivalues; ++m)
					    f->inh(f, sl, sl->letters[i].groups[j].signs[k]->u.s->forms[l]->lv->ivalues[m],
						   sx_pos_inst);
					  f->inh(f, sl, NULL, sx_pos_term);
					}
				    }
				}
			    }
			}
#undef QV
		      if (list_len(qv))
			{
			  struct sl_inst *ip;
			  f->qvs(f, sl, NULL, sx_pos_init);
			  for (ip = list_first(qv); ip; ip = list_next(qv))
			    f->qvs(f, sl, ip, sx_pos_inst);
			  f->qvs(f, sl, NULL, sx_pos_term);
			}
		      list_free(qv,NULL);
		      f->sgn(f, sl, NULL, sx_pos_term);
		    }
		}
	      f->grp(f, sl, NULL, sx_pos_term);
	    }
	}
      f->let(f, sl, NULL, sx_pos_term);
    }
  f->sll(f, sl, sx_pos_term);
}
