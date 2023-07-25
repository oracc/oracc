#include <signlist.h>
#include <sx.h>

int itrace = 0;

static void sx_v_fowner(struct sl_signlist *sl, struct sl_inst *ip, unsigned const char *v);

/* Go through the forms and add to their inherited values as necessary.

   If a value in the sign is not listed in the form, inherit it

   UNLESS:
   
   If the @form exists as a @sign and values with the same base have
   different indexes in the @form instance and the @sign instance it
   is an error.

   This means that it is legal to say:

     @sign A
     @v a
     @form E
     @v aₓ

   And

     @sign E
     @v aₓ
     
   But it is not legal to say:

     @sign |LAGAB×A|
     @v aₓ
     @form A
     @v a₆
     
   And when inheriting

     @sign |LAGAB×A|
     @v aₓ
     @form A

   Value aₓ will not be inherit because it will be blocked by @sign A @v a.
 */ 

void
sx_inherited(struct sl_signlist *sl)
{
  int i;
  for (i = 0; i < sl->nforms; ++i)
    {
      struct sl_inst *form_inst = NULL;
      for (form_inst = list_first(sl->forms[i]->insts); form_inst; form_inst = list_next(sl->forms[i]->insts))
	{
	  if (itrace)
	    fprintf(stderr, "inherit: processing %s::%s\n", form_inst->parent->name, sl->forms[i]->name);
	  /* Each of form_owner's values is a candidate for inheriting; we can approach this via the sign's hvbases */
	  if (form_inst->parent->hvbases)
	    {
	      const char **keys;
	      int nkeys;
	      int j;
	      keys = hash_keys2(form_inst->parent->hvbases, &nkeys);
	      for (j = 0; j < nkeys; ++j)
		{
		  unsigned const char *b = NULL;
		  if (itrace)
		    fprintf(stderr, "inherit: testing parent base %s\n", keys[j]);
		  if (form_inst->lv && form_inst->lv->hvbases)
		    {
		      /* If the sign has a saman₀ and the form has a saman₀ don't inherit */
		      if (itrace)
			fprintf(stderr, "inherit: parent base %s found in form_inst->lv->hvbases\n", keys[j]);
		      if (!(b = hash_find(form_inst->lv->hvbases, (uccp)keys[j])))
			{
			  /* In @sign A @v a and @sign E @v a₆ @form A we have to reject inheritance */
			  const unsigned char *sb = NULL;
			  if (!(sb = hash_find(sl->forms[i]->sign->hvbases, (uccp)keys[j])))
			    {
			      b = hash_find(form_inst->parent->hvbases, (uccp)keys[j]);
			      if (itrace)
				fprintf(stderr, "inherit: value %s is inheritable in form %s under sign %s\n",
					b, sl->forms[i]->name, form_inst->parent->name);
			      if (!form_inst->lv)
				form_inst->lv = memo_new(sl->m_lv_data);
			      if (!form_inst->lv->hivalues)
				form_inst->lv->hivalues = hash_create(1);
			      hash_add(form_inst->lv->hivalues, (uccp)b, "");
			      hash_add(form_inst->lv->hventry, (uccp)b, hash_find(form_inst->parent->hventry, (uccp)b));
			      sx_v_fowner(sl, form_inst, b);
			    }
			}
		    }
		  else
		    {
		      if (itrace)
			fprintf(stderr, "inherit: parent base %s not found in form_inst->lv->hvbases\n", keys[j]);
		      b = hash_find(form_inst->parent->hvbases, (uccp)keys[j]);
		      /* inherit into an empty lv node */
		      if (!form_inst->lv)
			form_inst->lv = memo_new(sl->m_lv_data);
		      if (!form_inst->lv->hventry)
			form_inst->lv->hventry = hash_create(32);
		      if (!form_inst->lv->hivalues)
			  form_inst->lv->hivalues = hash_create(32);
		      if (itrace)
			fprintf(stderr, "inherit: adding %s to form_inst->lv->hventry/hivalues\n", b);
		      hash_add(form_inst->lv->hivalues, (uccp)b, "");
		      hash_add(form_inst->lv->hventry, (uccp)b, hash_find(form_inst->parent->hventry, (uccp)b));
		      sx_v_fowner(sl, form_inst, b);
		    }
		}
	    }
	  /* While we are iterating over the form insts we also
	     validate form values to ensure that they don't share
	     distinct values with the same base between the form and
	     its corresponding sign instance */
	  if (form_inst->lv && form_inst->lv->hvbases)
	    {
	      const char **keys;
	      int nkeys, k;

	      keys = hash_keys2(form_inst->lv->hvbases, &nkeys);
	      for (k = 0; k < nkeys; ++k)
		{
		  const unsigned char *sb = NULL;
		  if ((sb = hash_find(sl->forms[i]->sign->hvbases, (uccp)keys[k])))
		    {
		      const unsigned char *fb = hash_find(form_inst->lv->hvbases, (uccp)keys[k]);
		      /* @sign A @v a and @form A @v a₆ */
		      if (sb && fb && strcmp((ccp)sb, (ccp)fb))
			mesg_verr(&form_inst->mloc, "value %s in form %s::%s has the same base as value %s in sign %s\n",
				  fb, form_inst->parent->name, sl->forms[i]->name, sb, sl->forms[i]->sign->name);
		    }
		}
	    }
	}
    }
}

static void
sx_v_fowner(struct sl_signlist *sl, struct sl_inst *ip, unsigned const char *v)
{
  struct sl_value *vp = hash_find(sl->hventry, v);
  if (vp)
    {
      if (!vp->fowners)
	vp->fowners = list_create(LIST_SINGLE);
      list_add(vp->fowners, ip);
    }
}

