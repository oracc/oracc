#include <json.h>
#include <signlist.h>
#include <sx.h>

static sx_signlist_f sx_w_j_signlist;
static sx_letter_f sx_w_j_letter;
static sx_group_f sx_w_j_group;
static sx_sign_f sx_w_j_sign;
static sx_form_f sx_w_j_form;
static sx_list_f sx_w_j_list;
static sx_value_f sx_w_j_ivalue;
static sx_value_f sx_w_j_qvs;
static sx_value_f sx_w_j_value;
static sx_notes_f sx_w_j_notes;
static sx_unicode_f sx_w_j_unicode;

struct sx_functions sx_w_jsn_fncs;

struct sx_functions *
sx_w_jsn_init(FILE *fp, const char *fname)
{
  jw_init(stdout);
  sx_w_jsn_fncs.sll = sx_w_j_signlist;
  sx_w_jsn_fncs.let = sx_w_j_letter;
  sx_w_jsn_fncs.grp = sx_w_j_group;
  sx_w_jsn_fncs.sgn = sx_w_j_sign;
  sx_w_jsn_fncs.lst = sx_w_j_list;
  sx_w_jsn_fncs.frm = sx_w_j_form;
  sx_w_jsn_fncs.val = sx_w_j_value;
  sx_w_jsn_fncs.inh = sx_w_j_ivalue;
  sx_w_jsn_fncs.not = sx_w_j_notes;
  sx_w_jsn_fncs.uni = sx_w_j_unicode;
  sx_w_jsn_fncs.qvs = sx_w_j_qvs;
  sx_w_jsn_fncs.fp = fp;
  sx_w_jsn_fncs.fname = fname;
  return &sx_w_jsn_fncs;
}

#if 0
static void
sx_w_j_str_list(FILE *fp, const char *tag, List *lp)
{
}
#endif

/* This is the entry point for XXX output */
static void
sx_w_j_signlist(struct sx_functions *f, struct sl_signlist *sl, enum sx_pos_e p)
{
  if (p == sx_pos_init)
    {
      jw_object_o();
      jw_member("type");
      jw_string("signlist");
      jw_member("project");
      jw_string(sl->project);
      jw_member("signs");
      jw_object_o();
    }
  else if (p == sx_pos_term)
    {
      jw_object_c(); /* close signs object */
      jw_object_c(); /* close signlist object */
    }
}

static void
sx_w_j_group(struct sx_functions *f, struct sl_signlist *sl, struct sl_group *g, enum sx_pos_e p)
{
}

static void
sx_w_j_letter(struct sx_functions *f, struct sl_signlist *sl, struct sl_letter *l, enum sx_pos_e p)
{
}

static void
sx_w_j_form(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *s, enum sx_pos_e p)
{
  static int in_form = 0;

  if (p == sx_pos_inst)
    {
      if (in_form)
	jw_object_c();

      jw_member((ccp)s->u.f->name);
      jw_object_o();
      jw_strmem("type","form");
      in_form = 1;
    }
  if (p == sx_pos_term)
    {
      jw_object_c();
      in_form = 0;
    }
}

static void
sx_w_j_ivalue(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *v, enum sx_pos_e p)
{
}

static void
sx_w_j_list(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *l, enum sx_pos_e p)
{
}

static void
sx_w_j_notes(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *ip)
{
}

static void
sx_w_j_qvs(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *vi, enum sx_pos_e p)
{
}

/** Because this is called when walking groups->signs \c sl_inst*s here can be a sign or form inst
 */
static void
sx_w_j_sign(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *s, enum sx_pos_e p)
{
  static int in_sign = 0;

  if (p == sx_pos_inst)
    {
      unsigned const char *name;

      if (in_sign)
	jw_object_c();

      if (s->type == 's')
	name = s->u.s->name;
      else
	name = s->u.f->name;

      jw_member((ccp)name);
      jw_object_o();
      jw_strmem("type","sign");

      in_sign = 1;
    }
  if (p == sx_pos_term)
    {
      jw_object_c();
      in_sign = 0;
    }
}

static void
sx_w_j_value(struct sx_functions *f, struct sl_signlist *s, struct sl_inst *v, enum sx_pos_e p)
{
  if (p == sx_pos_init)
    {
      jw_member("values");
      jw_array_o();
    }
  else if (p == sx_pos_term)
    jw_array_c();
  else
    jw_string((ccp)v->u.v->name);
}

static void
sx_w_j_unicode(struct sx_functions *f, struct sl_signlist *sl, struct sl_unicode *up)
{
}
