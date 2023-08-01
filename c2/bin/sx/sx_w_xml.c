#include <xml.h>
#include <ns-asl.h>
#include <rnvif.h>
#include <rnvxml.h>
#include <signlist.h>
#include <sx.h>

extern Mloc *xo_loc;
static struct rnvval_atts *ratts;

static sx_signlist_f sx_w_x_signlist;
static sx_letter_f sx_w_x_letter;
static sx_group_f sx_w_x_group;
static sx_sign_f sx_w_x_sign;
static sx_form_f sx_w_x_form;
static sx_list_f sx_w_x_list;
static sx_value_f sx_w_x_ivalue;
static sx_value_f sx_w_x_qvs;
static sx_value_f sx_w_x_value;
static sx_notes_f sx_w_x_notes;
static sx_unicode_f sx_w_x_unicode;

struct sx_functions sx_w_xml_fncs;

struct sx_functions *
sx_w_xml_init(FILE *fp, const char *fname)
{
  rnvxml_init_err();
  rnvif_init();
  rnvxml_init(&asl_data, "asl");

  sx_w_xml_fncs.sll = sx_w_x_signlist;
  sx_w_xml_fncs.let = sx_w_x_letter;
  sx_w_xml_fncs.grp = sx_w_x_group;
  sx_w_xml_fncs.sgn = sx_w_x_sign;
  sx_w_xml_fncs.lst = sx_w_x_list;
  sx_w_xml_fncs.frm = sx_w_x_form;
  sx_w_xml_fncs.val = sx_w_x_value;
  sx_w_xml_fncs.inh = sx_w_x_ivalue;
  sx_w_xml_fncs.not = sx_w_x_notes;
  sx_w_xml_fncs.uni = sx_w_x_unicode;
  sx_w_xml_fncs.qvs = sx_w_x_qvs;
  sx_w_xml_fncs.fp = fp;
  sx_w_xml_fncs.fname = fname;
  return &sx_w_xml_fncs;
}

#if 0
static void
sx_w_x_str_list(FILE *fp, const char *tag, List *lp)
{
}
#endif

/* This is the entry point for xml output */
static void
sx_w_x_signlist(struct sx_functions *f, struct sl_signlist *sl, enum sx_pos_e p)
{
  xo_loc->file = "stdin"; xo_loc->line = 1;
  if (p == sx_pos_init)
    {
      ratts = rnvval_aa("x", "project", "ogsl", NULL);
      rnvxml_ea("sl:signlist", ratts);
    }
  else if (p == sx_pos_term)
    {
      rnvxml_ee("sl:signlist");
    }
}

static void
sx_w_x_group(struct sx_functions *f, struct sl_signlist *sl, struct sl_group *g, enum sx_pos_e p)
{
}

static void
sx_w_x_letter(struct sx_functions *f, struct sl_signlist *sl, struct sl_letter *l, enum sx_pos_e p)
{
  static int in_letter = 0;
  
  xo_loc->file = "stdin"; xo_loc->line = 1;
  if (p == sx_pos_inst)
    {
      char id[6];
      sprintf(id, "l%04d", l->code);
      if (in_letter)
	rnvxml_ee("sl:letter");
	
      ratts = rnvval_aa("x", "name", l->name, "title", l->name, "xml:id", id, NULL);
      rnvxml_ea("sl:letter", ratts);
      in_letter = 1;
    }
  else if (p == sx_pos_term)
    {
      if (in_letter)
	rnvxml_ee("sl:letter");
      in_letter = 0;
    }
}

static void
sx_w_x_form(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *s, enum sx_pos_e p)
{
}

static void
sx_w_x_ivalue(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *v, enum sx_pos_e p)
{
}

static void
sx_w_x_list(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *l, enum sx_pos_e p)
{
}

static void
sx_w_x_notes(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *ip)
{
}

static void
sx_w_x_qvs(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *vi, enum sx_pos_e p)
{
}

/** Because this is called when walking groups->signs \c sl_inst*s here can be a sign or form inst
 */
static void
sx_w_x_sign(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *s, enum sx_pos_e p)
{
}

static void
sx_w_x_value(struct sx_functions *f, struct sl_signlist *s, struct sl_inst *v, enum sx_pos_e p)
{
}

static void
sx_w_x_unicode(struct sx_functions *f, struct sl_signlist *sl, struct sl_unicode *up)
{
}
