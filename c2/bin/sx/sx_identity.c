#include <signlist.h>
#include <sx.h>

static sl_signlist_f sx_i_signlist;
static sl_letter_f sx_i_letter;
static sl_group_f sx_i_group;
static sl_sign_f sx_i_sign;
static sl_form_f sx_i_form;
static sl_value_f sx_i_value;

struct sl_functions sx_identity_fncs;

struct sl_functions *
sx_i_init(FILE *fp, const char *fname)
{
  sx_identity_fncs.sll = sx_i_signlist;
  sx_identity_fncs.let = sx_i_letter;
  sx_identity_fncs.grp = sx_i_group;
  sx_identity_fncs.sgn = sx_i_sign;
  sx_identity_fncs.frm = sx_i_form;
  sx_identity_fncs.val = sx_i_value;
  sx_identity_fncs.fp = fp;
  sx_identity_fncs.fname = fname;
  return &sx_identity_fncs;
}

/* This is the entry point for identity output */
static void
sx_i_signlist(struct sl_functions *f, struct sl_signlist *s)
{
  fprintf(f->fp, "@project %s\n", s->project);
  if (s->letters)
    {
      int i;
      for (i = 0; s->letters[i].name; ++i)
	f->let(f, &s->letters[i]);
    }
}

void
sx_i_letter(struct sl_functions *f, struct sl_letter *l)
{
  if (l->groups)
    {
      int i;
      for (i = 0; l->groups[i].name; ++i)
	f->grp(f, &l->groups[i]);
    }
}

void
sx_i_group(struct sl_functions *f, struct sl_group *g)
{
  if (g->signs)
    {
      int i;
      for (i = 0; g->signs[i].name; ++i)
	f->sgn(f, &g->signs[i]);
    }
}

void
sx_i_sign(struct sl_functions *f, struct sl_sign *s)
{
  fprintf(f->fp, "@sign\t%s\n", s->name);
  if (s->forms)
    {
      int i;
      for (i = 0; s->forms[i].name; ++i)
	f->frm(f,&s->forms[i]);
    }
  fprintf(f->fp, "@end sign\n");
}

void
sx_i_form(struct sl_functions *f, struct sl_form *s)
{
  fprintf(f->fp, "@form\t%s\n", s->name);
  /*sx_i_signform_info();*/
  fprintf(f->fp, "@end form\n");
}

void
sx_i_list(struct sl_functions *f, struct sl_list *l)
{
  fprintf(f->fp, "@list\t%s\n", l->name);
}

void
sx_i_value(struct sl_functions *f, struct sl_value *v)
{
  fprintf(f->fp, "@list\t%s\n", v->name);
}
