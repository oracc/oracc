#include <signlist.h>
#include <sx.h>

static sl_signlist_f sx_a_signlist;
static sl_letter_f sx_a_letter;
static sl_group_f sx_a_group;
static sl_sign_f sx_a_sign;
static sl_form_f sx_a_form;
static sl_value_f sx_a_value;

struct sl_functions sx_asl_fncs;

struct sl_functions *
sx_asl_init(FILE *fp, const char *fname)
{
  sx_asl_fncs.sll = sx_a_signlist;
  sx_asl_fncs.let = sx_a_letter;
  sx_asl_fncs.grp = sx_a_group;
  sx_asl_fncs.sgn = sx_a_sign;
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
  if (s->letters)
    {
      int i;
      for (i = 0; s->letters[i].name; ++i)
	f->let(f, &s->letters[i]);
    }
}

static void
sx_a_letter(struct sl_functions *f, struct sl_letter *l)
{
  fprintf(f->fp, "@letter\t%s\n", l->name);
  if (l->groups)
    {
      int i;
      for (i = 0; l->groups[i].name; ++i)
	f->grp(f, &l->groups[i]);
    }
}

static void
sx_a_group(struct sl_functions *f, struct sl_group *g)
{
  fprintf(f->fp, "@group\t%s\n", g->name);
  if (g->signs)
    {
      int i;
      for (i = 0; g->signs[i]->name; ++i)
	f->sgn(f, g->signs[i]);
    }
}

static void
sx_a_sign(struct sl_functions *f, struct sl_sign *s)
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

static void
sx_a_form(struct sl_functions *f, struct sl_form *s)
{
  fprintf(f->fp, "@form\t%s\n", s->name);
  /*sx_a_signform_info();*/
  fprintf(f->fp, "@end form\n");
}

static void
sx_a_list(struct sl_functions *f, struct sl_list *l)
{
  fprintf(f->fp, "@list\t%s\n", l->name);
}

static void
sx_a_value(struct sl_functions *f, struct sl_value *v)
{
  fprintf(f->fp, "@list\t%s\n", v->name);
}
