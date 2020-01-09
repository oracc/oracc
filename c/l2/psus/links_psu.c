#include "pool.h"
#include "hash.h"
#include "ilem_form.h"
#include "links.h"
#include "ngram.h"
#include "warning.h"
#include "memblock.h"
#include "ilem_form.h"
#include "sigs.h"
#include <xmlutil.h>

int psus_sig_check = 1;

static void set_instance_fields(struct xcl_context *xc, struct ML *mlp);

static char *
psu_inst(const char *sig)
{
  char *tmp = strstr(sig,"}::");
  char *ret = NULL;

  if (tmp)
    {
      ret = malloc(tmp - sig);
      strncpy(ret,sig+1,(tmp-sig)-1);
      ret[(tmp-sig)-1] = '\0';
    }
  return ret;
}

void
links_psu(struct xcl_context *xc, struct ML *mlp)
{
  struct linkset *lsp;
  int i, title_len = 0;
  const char *role = NULL;

  if (mlp && mlp->matches && mlp->matches[0].tt)
    role = mlp->matches[0].tt->owner->owner->owner->role;

  lsp = new_linkset(xc->linkbase,
		    role ? role : "psu",
		    mlp->matches[0].psu);
  if (mlp->matches[0].psu_form)
    {
      struct xcl_l*lp = calloc(1,sizeof(struct xcl_l));
      struct f2 *parsed_psu = mb_new(xc->sigs->mb_f2s);
      set_instance_fields(xc,mlp);
      /* PSU's don't use the && COF notation, so NULL final arg
	 is safe here */
      f2_parse((unsigned char*)mlp->matches[0].lp->xc->file,
	       mlp->matches[0].lp->f->lnum,
	       npool_copy((unsigned char*)mlp->matches[0].psu,xc->pool), 
	       parsed_psu,
	       NULL, NULL);
      
      mlp->matches[0].psu_form->file = (unsigned char *)mlp->matches[0].lp->xc->file;
      mlp->matches[0].psu_form->lnum = mlp->matches[0].lp->f->lnum;
      mlp->matches[0].psu_form->cf = parsed_psu->cf;
      mlp->matches[0].psu_form->gw = parsed_psu->gw;
      if (mlp->matches[0].lp->f->psu_sense)
	mlp->matches[0].psu_form->sense = (unsigned char*)mlp->matches[0].lp->f->psu_sense;
      else if (parsed_psu->sense)
	mlp->matches[0].psu_form->sense = parsed_psu->sense;
      if (parsed_psu->pos)
	mlp->matches[0].psu_form->pos = parsed_psu->pos;
      if (parsed_psu->epos)
	mlp->matches[0].psu_form->epos = parsed_psu->epos;

      lp->parent = xc->root; /* fake this */
      lsp->form = *mlp->matches[0].psu_form;
      lsp->form.file = (unsigned char*)xc->file;
      lsp->form.lnum = mlp->matches[0].lp->lnum;

      lsp->form.sig = f2_psu_sig(xc, mlp->matches[0].psu_form);

      lp->inst = psu_inst((char*)lsp->form.sig);
      lp->f = calloc(1,sizeof(struct ilem_form));
      lp->f->file = (char*)mlp->matches[0].psu_form->file;
      lp->f->lnum = mlp->matches[0].psu_form->lnum;
      lp->f->f2 = *mlp->matches[0].psu_form;
      if (psus_sig_check)
	sigs_l_check(xc, lp);
      mlp->matches[0].psu_nfinds = lp->f->fcount;

 #if 0
      /* Removed 2019-05-19 because it gets set correctly via f2_psu_sig then
	 this rubbish breaks it */
      /* WATCHME: should I be using psu_finds and reporting ambig here? 
       * For now, just use the first sig.
       */
      if (lp->f->fcount > 0)
        lsp->form->sig = lp->f->finds[0]->f2.sig;
#endif

      /* can't free this now because it may be referenced via the cache */
      /* free(lp->f); */
      free(lp);
      /* clear the newflag so it doesn't carry over to further occurrences of
	 this psu_form */
      /*mlp->matches[0].psu_form->newflag = 0;*/
      if (psus_sig_check)
	{
	  if (!mlp->matches[0].psu_nfinds)  /*NB: NO AMBIGUITY YET*/
	    {
	      struct f2 *e = mlp->matches[0].psu_form;
	      vwarning2((const char *)e->file, e->lnum, 
			"psu: %s[%s]%s: compound not found",
			e->cf,e->gw,e->pos);
	    }
	  else if (verbose)
	    {
	      struct f2 *e = mlp->matches[0].psu_form;
	      vwarning2((const char *)e->file, e->lnum, 
			"psu: %s[%s]%s found OK",
			e->cf,e->gw,e->pos);
	    }
	}
    }
  preallocate_links(lsp,mlp->matches_used);
  lsp->used = mlp->matches_used;
  for (i = 0; i < mlp->matches_used; ++i)
    {
      lsp->links[i].role = "elt";
      lsp->links[i].title = (const char *)(mlp->matches[i].lp->f->f2.cf
					   ? mlp->matches[i].lp->f->f2.cf
					   : mlp->matches[i].lp->f->f2.pos);
      title_len += strlen(lsp->links[i].title);
      lsp->links[i].lp = mlp->matches[i].lp;
      lsp->links[i].lref = mlp->matches[i].lp->xml_id;

      /* WATCHME: this is a bit lazy; but at initial implementation it
	 is not possible for an lp to be part of more than one PSU */
      mlp->matches[i].lp->psurefs = lsp->xml_id;
      mlp->matches[i].lp->f->is_part = 1;

      /* Delete finds which are not PSU matches */
      if (mlp->matches[i].nmatches < mlp->matches[i].lp->f->fcount)
	{
	  memcpy(mlp->matches[i].lp->f->finds, mlp->matches[i].matching_f2s, mlp->matches[i].nmatches * sizeof(struct f2*));
	  mlp->matches[i].lp->f->finds[mlp->matches[i].nmatches] = NULL;
	  mlp->matches[i].lp->f->fcount = mlp->matches[i].nmatches;
	}
    }
  if (!lsp->title)
    {
      char *tmp = malloc(title_len + mlp->matches_used + 1);
      *tmp = '\0';
      for (i = 0; i < mlp->matches_used; ++i)
	{
	  strcat(tmp, lsp->links[i].title);
	  if (i < mlp->matches_used)
	    strcat(tmp, " ");
	}
      tmp[strlen(tmp)-1] = '\0';
      lsp->title = tmp;
    }
}

static void
set_instance_fields(struct xcl_context *xc, struct ML *mlp)
{
  const char *lastw = "";
  int i;
  static char formbuf[128], normbuf[128];
  List *parts = list_create(LIST_SINGLE);

  *formbuf = *normbuf = '\0';
  for (i = 0; i < mlp->matches_used; ++i)
    {
      /* Should we be discriminating about which match
	 of matches[i].matches[] we are using for this? */
      struct f2 *lform = mlp->matches[i].matching_f2s[0];
      struct f2 *clone = mb_new(xc->sigs->mb_f2s);
      /* This is a shallow clone; we only need it so we can
	 set the flags locally */
      *clone = *lform;
      list_add(parts, lform);
      if (strcmp(lastw,mlp->matches[i].lp->ref))
	{
	  if (*formbuf)
	    strcat(formbuf, " ");
	  strcat(formbuf,(char*)lform->form);
	}
      else
	BIT_SET(lform->flags,F2_FLAGS_SAME_REF);
      lastw = mlp->matches[i].lp->ref;
      if (lform->norm)
	{
	  if (*normbuf)
	    strcat(normbuf, " ");
	  strcat(normbuf,(char*)lform->norm);
	}
    }
  mlp->matches[0].psu_form->form = (unsigned char*)formbuf;
  mlp->matches[0].psu_form->norm = (unsigned char*)normbuf;
  mlp->matches[0].psu_form->file = (unsigned char*)mlp->matches[0].lp->f->file;
  mlp->matches[0].psu_form->lnum = mlp->matches[0].lp->f->lnum;
  mlp->matches[0].psu_form->parts = (struct f2**)list2array(parts);
  list_free(parts, NULL);
}
