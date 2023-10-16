#include <signlist.h>
#include <sx.h>

static sx_signlist_f sx_w_X_signlist;
static sx_letter_f sx_w_X_letter;
static sx_group_f sx_w_X_group;
static sx_sign_f sx_w_X_sign;
static sx_form_f sx_w_X_form;
static sx_list_f sx_w_X_list;
static sx_value_f sx_w_X_ivalue;
static sx_value_f sx_w_X_qvs;
static sx_value_f sx_w_X_value;
static sx_notes_f sx_w_X_notes;
static sx_unicode_f sx_w_X_unicode;

struct sx_functions sx_w_XXX_fncs;

struct sx_functions *
sx_w_XXX_init(FILE *fp, const char *fname)
{
  sx_w_XXX_fncs.sll = sx_w_X_signlist;
  sx_w_XXX_fncs.let = sx_w_X_letter;
  sx_w_XXX_fncs.grp = sx_w_X_group;
  sx_w_XXX_fncs.sgn = sx_w_X_sign;
  sx_w_XXX_fncs.lst = sx_w_X_list;
  sx_w_XXX_fncs.frm = sx_w_X_form;
  sx_w_XXX_fncs.val = sx_w_X_value;
  sx_w_XXX_fncs.inh = sx_w_X_ivalue;
  sx_w_XXX_fncs.not = sx_w_X_notes;
  sx_w_XXX_fncs.uni = sx_w_X_unicode;
  sx_w_XXX_fncs.qvs = sx_w_X_qvs;
  sx_w_XXX_fncs.fp = fp;
  sx_w_XXX_fncs.fname = fname;
  return &sx_w_XXX_fncs;
}

static void
sx_w_X_str_list(FILE *fp, const char *tag, List *lp)
{
}

/* This is the entry point for XXX output */
static void
sx_w_X_signlist(struct sx_functions *f, struct sl_signlist *sl, enum sx_pos_e p)
{
}

static void
sx_w_X_group(struct sx_functions *f, struct sl_signlist *sl, struct sl_group *g, enum sx_pos_e p)
{
}

static void
sx_w_X_letter(struct sx_functions *f, struct sl_signlist *sl, struct sl_letter *l, enum sx_pos_e p)
{
}

static void
sx_w_X_form(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *s, enum sx_pos_e p)
{
}

static void
sx_w_X_ivalue(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *v, enum sx_pos_e p)
{
}

static void
sx_w_X_list(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *l, enum sx_pos_e p)
{
}

static void
sx_w_X_notes(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *ip)
{
}

static void
sx_w_X_qvs(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *vi, enum sx_pos_e p)
{
}

/** Because this is called when walking groups->signs \c sl_inst*s here can be a sign or form inst
 */
static void
sx_w_X_sign(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *s, enum sx_pos_e p)
{
}

static void
sx_w_X_value(struct sx_functions *f, struct sl_signlist *s, struct sl_inst *v, enum sx_pos_e p)
{
}

static void
sx_w_X_unicode(struct sx_functions *f, struct sl_signlist *sl, struct sl_unicode *up)
{
}
