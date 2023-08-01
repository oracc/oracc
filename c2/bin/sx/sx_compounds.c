#include <oraccsys.h>
#include <unidef.h>
#include <signlist.h>
#include <sx.h>

int ctrace = 0;

const char *last_g;

enum sxc_type { sxc_initial, sxc_medial, sxc_final, sxc_container, sxc_contained };

/* g:d text=× sets this to 1; it is reset to 0 by g:s unless a g:gp
   sets it to 2 in which case g:gp sets it to 0 at the end of the
   (...) */
int sxc_container_active = 0;
static int sxc_nth = 0;
static void sx_compound(struct sl_signlist *sl, Node *gdl, const char *oid);
static void sx_compound_node(Node *np, struct sl_signlist *sl, const char *oid);

/* The list of compounds has been made while reading the input.
   Compounds are always registered under an sl_sign*: signs which only
   occur as forms are processed using the sl_sign* which is the host
   for the form-as-sign.

   For each sign A we create a hash of the form:
   A--|A.A| -> struct sl_compound *
   |--|A×A| -> struct sl_compound *

   This is further processed in sx_digests.c.
 */
void
sx_compounds(struct sl_signlist *sl)
{
  struct sl_inst *ip;

  /* We can't process these in sorted order because we can't guarantee
     that we have all the sortable tokens until after processing the
     compounds */
  for (ip = list_first(sl->compounds); ip; ip = list_next(sl->compounds))
    {
      struct sl_sign *s = (ip->type == 's' ? ip->u.s : ip->u.f->sign);
      struct sl_token *tokp;
      sxc_nth = 0;
      sl->curr_inst = ip;
      tokp = hash_find(sl->htoken, s->name);
      sx_compound(sl, tokp->gdl, (ccp)s->name);
    }
}

static struct sl_compound *
sx_compound_init(struct sl_signlist *sl, Hash *h, const char *c)
{
  struct sl_compound *cdp = memo_new(sl->m_compounds);
  hash_add(h, (uccp)c, cdp);
  return cdp;
}

/* An earlier version of sx_compounds registered signs that were not
   themselves in OGSL but this caused problems so now we require any
   compound element to be registered as a @sign or @comp and also
   require the use of @aka with sign-names that have non-standard name
   components. */
static void /* struct sl_sign* */
sx_compound_new_sign(struct sl_signlist *sl, const char *sgnname, const char *cpdname)
{
  const unsigned char *lc = NULL;
  struct sl_value *vp = NULL;
  struct sl_sign *sp = NULL;

  if (!sl->hcompoundnew)
    sl->hcompoundnew = hash_create(32);
  else if (hash_find(sl->hcompoundnew, (uccp)sgnname))
    return;  

  hash_add(sl->hcompoundnew, (uccp)sgnname, "");
	   
  lc = utf_lcase((uccp)sgnname);
  vp = hash_find(sl->hventry, (uccp)lc);

  if (!(sp = hash_find(sl->hsentry, (uccp)cpdname)))
    mesg_verr(&sl->curr_inst->mloc, "sx: no hsentry for form %s\n", cpdname);

  if (vp)
    {
      const unsigned char *sn = NULL;
      struct sl_inst *ip = NULL;
      
      /* Traverse the list of sl_inst* for the value looking for a valid one */
      for (ip = list_first(vp->insts); ip; ip = list_next(vp->insts))
	if (ip->valid)
	  break;

      if (ip)
	sn = (ip->parent_f ? ip->parent_f->u.f->name : ip->parent_s->u.s->name);
      else
	mesg_verr(&sl->curr_inst->mloc, "compound element %s in %s does not correspond to a valid value", sgnname, cpdname);

      if (sn)
	{
	  if (!hash_find(oids, sn))
	    mesg_verr(&sl->curr_inst->mloc, "compound element %s should have @sign entry (also tried %s=>%s)", sgnname, vp->name, sn);
	  else if (!sl->curr_inst->literal)
	    mesg_verr(&sl->curr_inst->mloc, "%s in %s should be %s; use @aka if necessary\n",
		      sgnname, cpdname, sn);
	}
      else
	mesg_verr(&sl->curr_inst->mloc, "compound element %s has no valid corresponding value %s\n", sgnname, vp->name);
    }
  else
    {
      mesg_verr(&sl->curr_inst->mloc, "compound element %s should have @sign entry\n", sgnname);
    }
}

static void
sx_compound_data(struct sl_signlist *sl, const char *sgnname, const char *cpdname, enum sxc_type t)
{
  struct sl_sign *sp = NULL;
  struct sl_compound *cdp = NULL;
  int new_sign = 0;

  sp = hash_find(sl->hsentry, (uccp)sgnname);

  if (!sp)
    {
      sx_compound_new_sign(sl, sgnname, cpdname);
    }
  else
    {
      if (!sp->hcompounds)
	{
	  sp->hcompounds = hash_create(32);
	  if (ctrace)
	    fprintf(stderr, "ctrace: adding data for compound %s under %ssign %s\n",
		    cpdname, new_sign ? "new ": "", sgnname);
	  cdp = sx_compound_init(sl, sp->hcompounds, cpdname);
	}
      else if (!(cdp = hash_find(sp->hcompounds, (uccp)cpdname)))
	{
	  if (ctrace)
	    fprintf(stderr, "ctrace: adding more data for compound %s under sign %s\n", cpdname, sgnname);
	  cdp = sx_compound_init(sl, sp->hcompounds, cpdname);
	}
      
      if (ctrace)
	{
	  const char *st = NULL;
	  switch (t)
	    {
	    case sxc_initial:
	      st = "INITIAL";
	      break;
	    case sxc_final:
	      if (cdp->medial == 1)
		{
		  st = "FINAL (m=1 reset to 0)";
		  cdp->medial = 0;
		}
	      else if (cdp->medial == 2)
		{
		  st = "FINAL (m=2 reset to 1)";
		  cdp->medial = 1;
		}
	      else
		st = "FINAL (m=0)";
	      break;
	    case sxc_container:
	      st = "CONTAINER";
	      break;
	    case sxc_contained:
	      st = "CONTAINED";
	      break;
	    case sxc_medial:
	      if (cdp->medial)
		st = "MEDIAL/m";
	      else
		st = "MEDIAL/s";
	      break;
	    default:
	      abort();
	      break;
	    }
	  fprintf(stderr, "ctrace: add %s to sign %s in compound %s\n", st, sgnname, cpdname);
	}

      switch (t)
	{
	case sxc_initial:
	  cdp->initial_or_final = -1;
	  break;
	case sxc_final:
	  cdp->initial_or_final = 1;
	  break;
	case sxc_container:
	  cdp->container = 1;
	  break;
	case sxc_contained:
	  cdp->contained = 1;
	  break;
	case sxc_medial:
	  if (cdp->medial)
	    cdp->medial = 2;
	  else
	    cdp->medial = 1;
	  break;
	default:
	  abort();
	  break;
	}
    }
}

static void
sx_compound(struct sl_signlist *sl, Node *gdl, const char *sname)
{
  if (gdl && !strcmp(gdl->kids->name, "g:c"))
    {      
      if (ctrace)
	fprintf(stderr, "ctrace: start %s\n", gdl->kids->text);

      last_g = NULL;

      sx_compound_node(gdl->kids, sl, sname);

      if (last_g)
	sx_compound_data(sl, last_g, sname, sxc_final);
    }
}

static void
sx_compound_node(Node *np, struct sl_signlist *sl, const char *sname)
{
  if (np)
    {
      if (!strcmp(np->name, "g:s") || !strcmp(np->name, "g:l") || !strcmp(np->name, "g:n"))
	{

	  last_g = np->text;
	  
	  if (ctrace)
	    fprintf(stderr, "ctrace: g:s %s nth=%d contained=%d\n", np->text, sxc_nth, sxc_container_active);

	  if (!sxc_nth)
	    sx_compound_data(sl, last_g, sname, sxc_initial);
	  else
	    sx_compound_data(sl, last_g, sname, sxc_medial);
	  /*list_add(cdp->i, (void*)sname);*/

	  if (sxc_container_active)
	    {
	      sx_compound_data(sl, last_g, sname, sxc_contained);
	      if (sxc_container_active == 1)
		sxc_container_active = 0;
	    }
	  
	  ++sxc_nth;

	  /* don't process g:l/g:s kids because we have mods from s->text
	     and we don't currently index @g etc (might one day) */
	}
      else if (!strcmp(np->name, "g:d"))
	{
	  if (!strcmp(np->text, U_X_u8str))
	    {
	      if (ctrace)
		fprintf(stderr, "ctrace: g:d %s\n", np->text);
	      if (last_g)
		{
		  sx_compound_data(sl, last_g, sname, sxc_container);
		}
	      sxc_container_active = 1;
	    }
	}
      else if (!strcmp(np->name, "g:gp"))
	{
	  if (sxc_container_active)
	    sxc_container_active = 2;
	  for (np = np->kids; np; np = np->next)
	    sx_compound_node(np, sl, sname);
	  sxc_container_active = 0;
	}
      else if (!strcmp(np->name, "g:c"))
	{
	  for (np = np->kids; np; np = np->next)
	    sx_compound_node(np, sl, sname);
	}
      else if (!strcmp(np->name, "g:m"))
	; /* ignore @g on |(LAK079&LAK079)@g| */
      else if (!strcmp(np->name, "g:r"))
	; /* ignore repetition node on numbers */
      else
	{
	  fprintf(stderr, "sx: internal error: gvl node type %s not handled\n", np->name);
	}
    }
}

