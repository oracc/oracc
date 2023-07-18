#include <signlist.h>
#include <sx.h>

static sl_signlist_f sx_a_signlist;
static sl_letter_f sx_a_letter;
static sl_group_f sx_a_group;
static sl_sign_f sx_a_sign;
static sl_form_f sx_a_form;
static sl_list_f sx_a_list;
static sl_value_f sx_a_value;

struct sl_functions sx_asl_fncs;

struct sl_functions *
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
sx_a_signlist(struct sl_functions *f, struct sl_signlist *s)
{
  fprintf(f->fp, "@signlist %s\n", s->project);
  if (s->nletters)
    {
      int i;
      for (i = 0; i < s->nletters; ++i)
	f->let(f, &s->letters[i]);
    }
}

static void
sx_a_letter(struct sl_functions *f, struct sl_letter *l)
{
  fprintf(f->fp, "@letter\t%s\n", l->name);
  if (l->ngroups)
    {
      int i;
      for (i = 0; i < l->ngroups; ++i)
	f->grp(f, &l->groups[i]);
    }
}

static void
sx_a_group(struct sl_functions *f, struct sl_group *g)
{
  fprintf(f->fp, "@group\t%s\n", g->name);
  if (g->nsigns)
    {
      int i;
      for (i = 0; i < g->nsigns; ++i)
	f->sgn(f, g->signs[i]);
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
    fprintf(fp, "@uchar\t%s\n", up->uname);
  if (up->uphase)
    fprintf(fp, "@uchar\t%s\n", up->uphase);
  if (up->ucode)
    fprintf(fp, "@uchar\t%s\n", up->ucode);
  if (up->uchar)
    fprintf(fp, "@uchar\t%s\n", up->uchar);
  if (up->unotes)
    sx_a_str_list(fp, "unote", up->unotes);
}

static void
sx_a_notes(FILE *fp, struct sl_inst *i)
{
  if (i->n.inotes)
    sx_a_str_list(fp, "inote", i->n.inotes);
}

static void
sx_a_sign(struct sl_functions *f, struct sl_sign *s)
{
  if (s->xref)
    fprintf(f->fp, "@xsign\t%s\n", s->name);
  else
    fprintf(f->fp, "@sign\t%s\n", s->name);
  sx_a_unicode(f->fp, &s->U);
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
#if 0
  fprintf(f->fp, "@end sign\n");
#endif
}

static void
sx_a_form(struct sl_functions *f, struct sl_inst *s)
{
  fprintf(f->fp, "@form\t%s %s\n", s->u.f->var, s->u.f->name);
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
sx_a_list(struct sl_functions *f, struct sl_inst *l)
{
  fprintf(f->fp, "@list\t%s%s\n", l->u.l->name, l->query ? "?" : "");
  sx_a_notes(f->fp, l);
}

static void
sx_a_value(struct sl_functions *f, struct sl_inst *v)
{
  const char *queryminus = "";
  if (v->removed)
    queryminus = "-";
  else if (v->query)
    queryminus = "?";
  fprintf(f->fp, "@v%s\t%s\n", queryminus, v->u.v->name);
  sx_a_notes(f->fp, v);
}
