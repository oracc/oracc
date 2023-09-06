#include <xml.h>
#include <ns-asl.h>
#include <rnvif.h>
#include <rnvxml.h>
#include <gdl.h>
#include <signlist.h>
#include <sx.h>

extern Mloc *xo_loc;
static struct rnvval_atts *ratts;
Hash *xidseen = NULL;

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
static sx_notes_f sx_w_x_syss;
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
  sx_w_xml_fncs.sys = sx_w_x_syss;
  sx_w_xml_fncs.uni = sx_w_x_unicode;
  sx_w_xml_fncs.qvs = sx_w_x_qvs;
  sx_w_xml_fncs.fp = fp;
  sx_w_xml_fncs.fname = fname;
  return &sx_w_xml_fncs;
}

#if 0
static void
sx_w_x_et_list(FILE *fp, const char *tag, List *lp)
{
  const char *s;
  for (s = list_first(lp); s; s = list_next(lp))
    rnvxml_et(tag, NULL, (ccp)xmlify((uccp)s));
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
      xidseen = hash_create(1024);
      
      sx_w_x_notes(f, sl, sl->notes);
      int nn, i;
      const char **n = hash_keys2(sl->listdefs, &nn);
      qsort(n, nn, sizeof(const char *), cmpstringp);
      for (i = 0; i < nn; ++i)
	{
	  struct sl_listdef *ldp = hash_find(sl->listdefs, (uccp)n[i]);
	  ratts = rnvval_aa("x", "name", n[i], NULL);
	  rnvxml_ea("sl:listdef", ratts);
	  rnvxml_et("sl:info", NULL, (ccp)xmlify((uccp)ldp->str));
	  sx_w_x_notes(f, sl, &ldp->inst);
	  rnvxml_ee("sl:listdef");
	}

      n = hash_keys2(sl->sysdefs, &nn);
      qsort(n, nn, sizeof(const char *), cmpstringp);
      for (i = 0; i < nn; ++i)
	{
	  struct sl_sysdef *sdp = hash_find(sl->sysdefs, (uccp)n[i]);
	  ratts = rnvval_aa("x", "name", n[i], NULL);
	  rnvxml_ea("sl:sysdef", ratts);
	  if (sdp->comment)
	    rnvxml_et("sl:info", NULL, (ccp)xmlify((uccp)sdp->comment));
	  sx_w_x_notes(f, sl, &sdp->inst);
	  rnvxml_ee("sl:sysdef");
	}

    }
  else if (p == sx_pos_term)
    {
      rnvxml_ee("sl:signlist");
      hash_free(xidseen, NULL);
    }
}

static void
sx_w_x_group(struct sx_functions *f, struct sl_signlist *sl, struct sl_group *g, enum sx_pos_e p)
{
  static int in_group = 0;
  xo_loc->file = "stdin"; xo_loc->line = 1;
  if (p == sx_pos_inst)
    {
      if (in_group)
	rnvxml_ee("sl:signs");
	
      ratts = rnvval_aa("x", "name", g->name, "title", g->name, NULL);
      rnvxml_ea("sl:signs", ratts);
      in_group = 1;
    }
  else if (p == sx_pos_term)
    {
      if (in_group)
	rnvxml_ee("sl:signs");
      in_group = 0;
    }
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
  static int in_form = 0;

  if (p == sx_pos_inst)
    {
      const char *id_or_ref = "xml:id";
      
      if (in_form)
	{
	  rnvxml_ee("sl:form");
	  in_form = 0;
	}

      xo_loc->file = s->mloc.file; xo_loc->line = s->mloc.line;
  
      if (s->type == 'f')
	{
	  char scode[32];
	  struct sl_token *tp = NULL;
	  (void)sprintf(scode, "%d", s->u.f->sort);
	  tp = hash_find(sl->htoken, s->u.f->name);

	  if (!s->u.f->sign->xref)
	    id_or_ref = "ref";
	  else
	    {
	      if (hash_find(xidseen, (uccp)s->u.f->oid))
		id_or_ref = "ref";
	      else
		hash_add(xidseen, (uccp)s->u.f->oid, "");
	    }
	    
	  ratts = rnvval_aa("x", "n", s->u.f->name, id_or_ref, s->u.f->oid, "sort", scode, NULL);
	  rnvxml_ea("sl:form", ratts);
	  rnvxml_ea("sl:name", NULL);
	  grx_xml(tp->gdl, "g:w");
	  rnvxml_ee("sl:name");
	  in_form = 1;
	}
    }
  else if (p == sx_pos_term)
    {
      if (in_form)
	rnvxml_ee("sl:form");
      in_form = 0;
    }  
}

static void
sx_w_x_syss(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *ip)
{
  if (ip && !ip->inherited && ip->sys)
    {
      struct sl_sys *sp;
      for (sp = list_first(ip->sys); sp; sp = list_next(ip->sys))
	{
	  ratts = rnvval_aa("x", "name", sp->name, "token", sp->v, NULL);
	  rnvxml_ea("sl:sys", ratts);
	  if (sp->vv)
	    rnvxml_ch((ccp)xmlify((uccp)sp->vv));
	  rnvxml_ee("sl:sys");
	}
    }
}

static void
sx_w_x_ivalue(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *v, enum sx_pos_e p)
{
  static int in_value = 0;
  
  if (p == sx_pos_init)
    {
      rnvxml_ea("sl:inherited", NULL);
    }
  else if (p == sx_pos_inst)
    {
      if (v->inherited)
	{
	  char scode[32];
	  (void)sprintf(scode, "%d", v->u.v->sort);
	  
	  if (in_value)
	    rnvxml_ee("sl:iv");
	  else
	    in_value = 1;
	  
	  ratts = rnvval_aa("x", "n", v->u.v->name, "sort", scode, NULL);
	  rnvxml_ea("sl:iv", ratts);
	}
    }
  if (p == sx_pos_term)
    {
      if (in_value)
	rnvxml_ee("sl:iv");
      in_value = 0;
      rnvxml_ee("sl:inherited");
    }      
}

static void
sx_w_x_list(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *l, enum sx_pos_e p)
{
  if (p == sx_pos_inst)
    {
      char scode[32];
      (void)sprintf(scode, "%d", l->u.l->sort);
      ratts = rnvval_aa("x", "n", l->u.l->name, "sort", scode, NULL);
      rnvxml_ec("sl:list", ratts);
    }
}

static void
sx_w_x_notes(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *ip)
{
  struct sl_note *np;
  if (ip && ip->notes)
    for (np = list_first(ip->notes); np; np = list_next(ip->notes))
      {
	const char *xtag = "sl:inote";
	if (*np->tag == 'n')
	  xtag = "sl:note";
	else if (*np->tag == 'l')
	  xtag = "sl:lit";
	rnvxml_et(xtag, NULL, (ccp)xmlify((uccp)np->txt));
      }
}

static void
sx_w_x_qvs(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *vi, enum sx_pos_e p)
{
  static int in_qs = 0;
  if (p == sx_pos_init)
    {
      /* don't open qs until we find a q to avoid <qs/> */
    }
  else if (p == sx_pos_inst)
    {
      char *qv = NULL;
      const char *o = NULL;
      unsigned const char *p = NULL;
      int n = 0;

      p = vi->parent_s ? vi->parent_s->u.s->name : vi->parent_f->u.f->name;
      n += strlen((ccp)vi->u.v->name) + strlen((ccp)p) + 3;
      qv = malloc(n);
      sprintf(qv, "%s(%s)", vi->u.v->name, p);
      o = vi->parent_s ? vi->parent_s->u.s->oid : vi->parent_f->u.f->oid;
      if (!o)
	o = "";
      
      ratts = NULL;
      if (vi->u.v->qvmust)
	ratts = rnvval_aa("x", "qv", qv, "o", o, NULL);
      else if (vi->u.v->qvsign && vi->parent_s)
	ratts = rnvval_aa("x", "qv", qv, "o", o, NULL);
      else if (vi->u.v->qvform && vi->parent_f)
	ratts = rnvval_aa("x", "qv", qv, "o", o, NULL);
      if (ratts)
	{
	  if (!in_qs++)
	    rnvxml_ea("sl:qs", NULL);

	  rnvxml_ec("sl:q", ratts);
	}
    }
  if (p == sx_pos_term)
    {
      if (in_qs)
	rnvxml_ee("sl:qs");
      in_qs = 0;
    }
}

/** Because this is called when walking groups->signs \c sl_inst*s here can be a sign or form inst
 */
static void
sx_w_x_sign(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *s, enum sx_pos_e p)
{
  static int in_sign = 0;

  if (p == sx_pos_inst)
    {
      if (in_sign)
	{
	  rnvxml_ee("sl:sign");
	  in_sign = 0;
	}

      xo_loc->file = s->mloc.file; xo_loc->line = s->mloc.line;
  
      if (s->type == 's')
	{
	  char scode[32];
	  struct sl_token *tp = NULL;
	  (void)sprintf(scode, "%d", s->u.s->sort);
	  tp = hash_find(sl->htoken, s->u.s->name);
	  if (s->u.s->oid)
	    ratts = rnvval_aa("x", "n", s->u.s->name, "xml:id", s->u.s->oid, "sort", scode, NULL);
	  else
	    ratts = rnvval_aa("x", "n", s->u.s->name, "sort", scode, NULL);
	  rnvxml_ea("sl:sign", ratts);
	  in_sign = 1;
	  if (tp && tp->gdl)
	    {
	      rnvxml_ea("sl:name", NULL);
	      grx_xml(tp->gdl, "g:w");
	      rnvxml_ee("sl:name");
	    }
	}
#if 0
      else if (s->type == 'f')
	{
	  /* xref */
	}
      /* also handle compound-only here */
#endif
    }
  else if (p == sx_pos_term)
    {
      if (in_sign)
	rnvxml_ee("sl:sign");
      in_sign = 0;
    }  
}

static void
sx_w_x_value(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *v, enum sx_pos_e p)
{
  static int in_value = 0;
  
  if (p == sx_pos_inst)
    {
      if (!v->inherited && !v->u.v->atf)
	{
	  char scode[32];
	  struct sl_token *tp = NULL;
	  (void)sprintf(scode, "%d", v->u.v->sort);
	  tp = hash_find(sl->htoken, v->u.v->name);
	  
	  if (in_value)
	    rnvxml_ee("sl:v");
	  else
	    in_value = 1;
	  if (v->lang)
	    ratts = rnvval_aa("x", "n", v->u.v->name, "xml:lang", v->lang, "sort", scode, NULL);
	  else
	    ratts = rnvval_aa("x", "n", v->u.v->name, "sort", scode, NULL);
	  rnvxml_ea("sl:v", ratts);
	  rnvxml_ea("sl:name", NULL);
	  grx_xml(tp->gdl, "g:w");
	  rnvxml_ee("sl:name");
	}
    }
  if (p == sx_pos_term)
    {
      if (in_value)
	rnvxml_ee("sl:v");
      in_value = 0;
    }      
}

static void
sx_w_x_unicode(struct sx_functions *f, struct sl_signlist *sl, struct sl_unicode *up)
{
  if (up->uname)
    rnvxml_et("sl:uname", NULL, up->uname);
  if (up->uhex || up->useq)
    {
      const char *u;
      ratts = rnvval_aa("x", "hex", up->uhex ? up->uhex : up->useq, NULL);
      if (up->utf8)
	u = (ccp)up->utf8;
      else
	u = "";
      rnvxml_et("sl:ucun", ratts, u);
    }
  if (up->urev)
    rnvxml_et("sl:uage", NULL, up->urev);
}
