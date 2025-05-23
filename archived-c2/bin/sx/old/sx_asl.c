#include <signlist.h>
#include <sx.h>

static sx_signlist_f sx_a_signlist;
static sx_letter_f sx_a_letter;
static sx_group_f sx_a_group;
static sx_sign_f sx_a_sign;
static sx_form_f sx_a_form;
static sx_list_f sx_a_list;
static sx_value_f sx_a_value;

struct sx_functions sx_asl_fncs;

struct sx_functions *
sx_asl_init(FILE *fp, const char *fname)
{
  sx_asl_fncs.sll = sx_a_signlist;
  sx_asl_fncs.let = sx_a_letter;
  sx_asl_fncs.grp = sx_a_group;
  sx_asl_fncs.sgn = sx_a_sign;
  sx_asl_fncs.lst = sx_a_list;
  sx_asl_fncs.frm = sx_a_form;
  sx_asl_fncs.val = sx_a_value;
  sx_asl_fncs.fp = fp;
  sx_asl_fncs.fname = fname;
  return &sx_asl_fncs;
}

/* This is the entry point for asl output */
static void
sx_a_signlist(struct sx_functions *f, struct sl_signlist *s)
{
  fprintf(f->fp, "@signlist %s\n\n", s->project);
  if (s->nletters)
    {
      int i;
      for (i = 0; i < s->nletters; ++i)
	f->let(f, &s->letters[i]);
    }
}

static void
sx_a_letter(struct sx_functions *f, struct sl_letter *l)
{
  if (!identity_mode)
    fprintf(f->fp, "@letter\t%s\n", l->name);

  if (l->ngroups)
    {
      int i;
      for (i = 0; i < l->ngroups; ++i)
	f->grp(f, &l->groups[i]);
    }
}

static void
sx_a_group(struct sx_functions *f, struct sl_group *g)
{
  if (!identity_mode)
    fprintf(f->fp, "@group\t%s\n", g->name);

  if (g->nsigns)
    {
      int i;
      for (i = 0; i < g->nsigns; ++i)
	{
	  if ('s' == g->signs[i]->type)
	    {
	      if ((!g->signs[i]->u.s->xref && !g->signs[i]->u.s->compound_only)
		  || !identity_mode)
		f->sgn(f, g->signs[i]->u.s);
	    }
	}
    }
}

static void
sx_a_str_list(FILE *fp, const char *tag, List *lp)
{
  unsigned const char *t;
  for (t = list_first(lp); t; t = list_next(lp))
    fprintf(fp, "@%s\t%s\n", tag, t);
}

static void
sx_a_unicode(FILE *fp, struct sl_unicode_info *up)
{
  if (up->uname)
    fprintf(fp, "@uname\t%s\n", up->uname);
  if (up->uphase)
    fprintf(fp, "@uphase\t%s\n", up->uphase);
  if (up->ucode)
    fprintf(fp, "@ucode\t%s\n", up->ucode);
  if (up->uchar)
    fprintf(fp, "@uchar\t%s\n", up->uchar);
  if (up->unotes)
    sx_a_str_list(fp, "unote", up->unotes);
}

static void
sx_a_notes(FILE *fp, struct sl_inst *i)
{
  if (i->n.lit)
    sx_a_str_list(fp, "lit", i->n.lit);
  if (i->n.notes)
    sx_a_str_list(fp, "note", i->n.notes);
  if (i->n.inotes)
    sx_a_str_list(fp, "inote", i->n.inotes);
}

/* In identity_mode this routine is not called when sl_sign is an xref
   to a form or only occurs in compounds */
static void
sx_a_sign(struct sx_functions *f, struct sl_sign *s)
{
  const char *minus = "", *query = "";

  if (!s->inst->valid)
    minus = "-";
  if (s->inst->query)
    query = "?";
  if (s->xref)
    {
      struct sl_sign *sowner;
      fprintf(f->fp, "@xref%s\t%s%s\n", minus, s->name, query);
      /* SORTME ? */
      for (sowner = list_first(s->xref->owners); sowner; sowner = list_next(s->xref->owners))
	fprintf(f->fp, "@see %s\n", sowner->name);
      fprintf(f->fp, "@end xref\n");
    }
  else if (s->compound_only)
    {
      fprintf(f->fp, "@comp\t%s\n", s->name);
    }
  else
    {
      fprintf(f->fp, "@sign%s\t%s%s\n", minus, s->name, query);
      if (s->fake)
	fprintf(f->fp, "@fake 1\n");
      if (s->pname)
	fprintf(f->fp, "@pname %s\n", s->pname);
      sx_a_unicode(f->fp, &s->U);
      sx_a_notes(f->fp, s->inst);
      if (s->nlists)
	{
	  int i;
	  for (i = 0; i < s->nlists; ++i)
	    f->lst(f, s->lists[i]);
	}
      if (s->nvalues)
	{
	  int i;
	  for (i = 0; i < s->nvalues; ++i)
	    f->val(f, s->values[i]);
	}
      if (s->nforms)
	{
	  int i;
	  for (i = 0; i < s->nforms; ++i)
	    {
	      f->frm(f, s->forms[i]);
	    }
	}
      fprintf(f->fp, "@end sign\n\n");
    }
}

static void
sx_a_form(struct sx_functions *f, struct sl_inst *s)
{
  const char *minus = "", *query = "", *ref = "", *refspace = "";
  if (!s->valid)
    minus = "-";
  if (s->query)
    query = "?";
  if (s->ref)
    {
      refspace = " ";
      ref = (ccp)s->ref;
    }
  fprintf(f->fp, "@form%s\t%s%s%s%s\n", minus, s->u.f->name, query, refspace, ref);
  if (s->u.f->pname)
    fprintf(f->fp, "@pname %s\n", s->u.f->pname);
  sx_a_unicode(f->fp, &s->u.f->U);
  sx_a_notes(f->fp, s);
  if (s->lv)
    {
      if (s->lv->nlists)
	{
	  int i;
	  for (i = 0; i < s->lv->nlists; ++i)
	    f->lst(f, s->lv->lists[i]);
	}
      if (s->lv->nvalues)
	{
	  int i;
	  for (i = 0; i < s->lv->nvalues; ++i)
	    f->val(f, s->lv->values[i]);
	}
    }
#if 0
  /*sx_a_signform_info();*/
  fprintf(f->fp, "@end form\n");
#endif
}

static void
sx_a_list(struct sx_functions *f, struct sl_inst *l)
{
  fprintf(f->fp, "@list\t%s%s\n", l->u.l->name, l->query ? "?" : "");
  sx_a_notes(f->fp, l);
}

static void
sx_a_value(struct sx_functions *f, struct sl_inst *v)
{
  const char *minus = "", *query = "", *refspace = "", *ref = "";
  if (!v->valid)
    minus = "-";
  if (v->query)
    query = "?";
  if (v->ref)
    {
      refspace = " ";
      ref = (ccp)v->ref;
    }
  fprintf(f->fp, "@v%s\t%s%s%s%s\n", minus, v->u.v->name, query, refspace, ref);
  sx_a_notes(f->fp, v);
}
