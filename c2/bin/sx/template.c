#include <signlist.h>
#include <sx.h>

static sl_signlist_f sx_X_signlist;
static sl_letter_f sx_X_letter;
static sl_group_f sx_X_group;
static sl_sign_f sx_X_sign;
static sl_form_f sx_X_form;
static sl_list_f sx_X_list;
static sl_value_f sx_X_value;

static void sx_X_str(FILE *fp, const char *tag, unsigned char *s);
static void sx_X_str_list(FILE *fp, const char *tag, List *lp);
static void sx_X_unicode(FILE *fp, struct sl_unicode_info *up);
static void sx_X_notes(FILE *fp, struct sl_inst *i);

struct sl_functions sx_XXX_fncs;

struct sl_functions *
sx_XXX_init(FILE *fp, const char *fname)
{
  sx_XXX_fncs.sll = sx_X_signlist;
  sx_XXX_fncs.let = sx_X_letter;
  sx_XXX_fncs.grp = sx_X_group;
  sx_XXX_fncs.sgn = sx_X_sign;
  sx_XXX_fncs.lst = sx_X_list;
  sx_XXX_fncs.frm = sx_X_form;
  sx_XXX_fncs.val = sx_X_value;
  sx_XXX_fncs.fp = fp;
  sx_XXX_fncs.fname = fname;
  return &sx_XXX_fncs;
}

static void
sx_X_signlist(struct sl_functions *f, struct sl_signlist *sl)
{
}

static void
sx_X_letter(struct sl_functions *f, struct sl_letter *l)
{
}

static void
sx_X_group(struct sl_functions *f, struct sl_group *g)
{
}

static void
sx_X_sign(struct sl_functions *f, struct sl_sign *s)
{
}

static void
sx_X_form(struct sl_functions *f, struct sl_inst *s)
{
}

static void
sx_X_list(struct sl_functions *f, struct sl_inst *l)
{
}

static void
sx_X_value(struct sl_functions *f, struct sl_inst *v)
{
}

static void
sx_X_str(FILE *fp, const char *tag, unsigned char *s)
{
}

static void
sx_X_str_list(FILE *fp, const char *tag, List *lp)
{
}

static void
sx_X_unicode(FILE *fp, struct sl_unicode_info *up)
{
  if (up->uname)
    sx_X_str(fp, "uname", up->uname);
  if (up->uphase)
    sx_X_str(fp, "uphase", up->uphase);
  if (up->ucode)
    sx_X_str(fp, "ucode", up->ucode);
  if (up->uchar)
    sx_X_str(fp, "uchar", up->uchar);
  if (up->unotes)
    sx_X_str_list(fp, "unote", up->unotes);
}

static void
sx_X_notes(FILE *fp, struct sl_inst *i)
{
  if (i->n.lit)
    sx_a_str_list(fp, "lit", i->n.lit);
  if (i->n.notes)
    sx_a_str_list(fp, "note", i->n.notes);
  if (i->n.inotes)
    sx_a_str_list(fp, "inote", i->n.inotes);
}

