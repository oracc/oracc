#include <signlist.h>
#include <sx.h>

static sl_signlist_f sx_s_signlist;
static sl_letter_f sx_s_letter;
static sl_group_f sx_s_group;
static sl_sign_f sx_s_sign;
static sl_form_f sx_s_form;
static sl_list_f sx_s_list;
static sl_value_f sx_s_value;

struct sl_functions sx_sll_fncs;

static const char *curr_oid;

#if 0
static void sx_s_str_list(FILE *fp, const char *tag, List *lp);
static void sx_s_notes(FILE *fp, struct sl_inst *i);
#endif

static void sx_s_str(FILE *fp, const char *tag, const unsigned char *s);
static void sx_s_unicode(FILE *fp, struct sl_unicode_info *up);
static void sx_s_FORM(struct sl_functions *f, struct sl_form *s);

struct sl_functions *
sx_sll_init(FILE *fp, const char *fname)
{
  sx_sll_fncs.sll = sx_s_signlist;
  sx_sll_fncs.let = sx_s_letter;
  sx_sll_fncs.grp = sx_s_group;
  sx_sll_fncs.sgn = sx_s_sign;
  sx_sll_fncs.lst = sx_s_list;
  sx_sll_fncs.frm = sx_s_form;
  sx_sll_fncs.val = sx_s_value;
  sx_sll_fncs.fp = fp;
  sx_sll_fncs.fname = fname;
  return &sx_sll_fncs;
}

/* This is the entry point for sll output */
static void
sx_s_signlist(struct sl_functions *f, struct sl_signlist *sl)
{
  int i;
  for (i = 0; i < sl->nsigns; ++i)
    f->sgn(f, sl->signs[i]);
  for (i = 0; i < sl->nforms; ++i)
    sx_s_FORM(f, sl->forms[i]);
}

static void
sx_s_letter(struct sl_functions *f, struct sl_letter *l)
{
}

static void
sx_s_group(struct sl_functions *f, struct sl_group *g)
{
}

static void
sx_s_sign(struct sl_functions *f, struct sl_sign *s)
{
  curr_oid = s->oid;
  fprintf(f->fp, "%s\t%s\n", s->name, curr_oid);
  sx_s_unicode(f->fp, &s->U);
}

static void
sx_s_form(struct sl_functions *f, struct sl_inst *s)
{
  /* NOT USED */
}

static void
sx_s_FORM(struct sl_functions *f, struct sl_form *s)
{
  curr_oid = s->oid;
  fprintf(f->fp, "%s\t%s\n", s->name, curr_oid);
  sx_s_unicode(f->fp, &s->U);
}

static void
sx_s_list(struct sl_functions *f, struct sl_inst *l)
{
}

static void
sx_s_value(struct sl_functions *f, struct sl_inst *v)
{
}

static void
sx_s_str(FILE *fp, const char *tag, const unsigned char *s)
{
  fprintf(fp, "%s;%s\t%s\n", curr_oid, tag, s);
}

#if 0
static void
sx_s_str_list(FILE *fp, const char *tag, List *lp)
{
}
#endif

static void
sx_s_unicode(FILE *fp, struct sl_unicode_info *up)
{
  if (up->uname)
    sx_s_str(fp, "uname", (uccp)up->uname);
  if (up->uphase)
    sx_s_str(fp, "uphase", (uccp)up->uphase);
  if (up->ucode)
    sx_s_str(fp, "ucode", (uccp)up->ucode);
  if (up->uchar)
    sx_s_str(fp, "uchar", (uccp)up->uchar);
#if 0
  if (up->unotes)
    sx_s_str_list(fp, "unote", up->unotes);
#endif
}

#if 0
  /* SLL output ignores notes */
static void
sx_s_notes(FILE *fp, struct sl_inst *i)
{
  if (i->n.lit)
    sx_s_str_list(fp, "lit", i->n.lit);
  if (i->n.notes)
    sx_s_str_list(fp, "note", i->n.notes);
  if (i->n.inotes)
    sx_s_str_list(fp, "inote", i->n.inotes);
}
#endif

