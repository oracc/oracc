#include <list.h>
#include <xml.h>
#include <ns-asl.h>
#include <rnvif.h>
#include <rnvxml.h>
#include <gdl.h>
#include <signlist.h>
#include <oraccsys.h>
#include <sx.h>

extern Mloc *xo_loc;
static struct rnvval_atts *ratts;
Hash *xidseen = NULL;
static unsigned char *x_cdp_refs(struct sl_signlist *sl, struct sl_inst *s);

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
static sx_notes_f sx_w_x_images;
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
  sx_w_xml_fncs.img = sx_w_x_images;
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
      ratts = rnvval_aa("x", "project", sl->project, NULL);
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
      if (sl->iheaders)
	{
	  for (i = 0; i < list_len(sl->images); ++i)
	    {
	      const char *o = itoa(i);
	      ratts = rnvval_aa("x", "xml:id", sl->iheaders[i].id, "order", o, "label", sl->iheaders[i].label, NULL);
	      rnvxml_ec("sl:iheader", ratts);
	    }
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
sx_w_x_images(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *ip)
{
  int skip_tle = 0;
  if (ip->type == 's')
    skip_tle = (ip->u.s->type == sx_tle_componly
		|| ip->u.s->type == sx_tle_lref
		|| ip->u.s->type == sx_tle_sref);
  else
    skip_tle = ip->u.f->compoundonly;
  if (ip && !ip->inherited && !skip_tle)
    {
      const char *oid = (ip->type == 's' ? ip->u.s->oid : ip->u.f->oid);
      if (oid)
	{
	  int index = (uintptr_t)hash_find(sl->oidindexes, (uccp)oid);
	  if (index)
	    {
	      --index; /* oidindexes stores index+1 */
	      if (sl->iarray->rows[index])
		{
		  int i;
		  int n = list_len(sl->images)+1;
		  rnvxml_ea("sl:images", NULL);
		  for (i = 1; i < n; ++i)
		    {
		      struct rnvval_atts *ratts = NULL;
		      if (sl->iarray->rows[index][i])
			ratts = rnvval_aa("x", "ref", sl->iheaders[i-1].id, "loc", sl->iarray->rows[index][i], NULL);
		      rnvxml_ec("sl:i", ratts);
		    }
		  rnvxml_ee("sl:images");
		}
	    }
	}
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

	  if (s->u.f->compoundonly)
	    {
	      List *a = list_create(LIST_SINGLE);
	      const char **atts = NULL;
	      static char scode[32];
	      struct rnvval_atts *ratts = NULL;

	      list_add(a, "n");
	      list_add(a, (void*)xmlify(s->u.f->name));
  
	      if (s->u.f->oid)
		{
		  list_add(a, "xml:id");
		  list_add(a, (void*)s->u.f->oid);
		}

	      if (s->u.f->sort > 0)
		(void)sprintf(scode, "%d", s->u.f->sort);
	      if (*scode)
		{
		  list_add(a, "sort");
		  list_add(a, scode);
		}

	      list_add(a, "compoundonly");
	      list_add(a, "yes");
	      
	      unsigned char *oids = x_cdp_refs(sl, s);
	      if (oids)
		{
		  list_add(a, "cpd-refs");
		  list_add(a, oids);
		}
	      else
		fprintf(stderr, "%s:#digest_by_oid=%s\n", s->u.f->name, " [not found]");

	      atts = list2chars(a);
	      ratts = rnvval_aa_qatts((char**)atts, list_len(a)/2);
	      list_free(a, NULL);
	      rnvxml_ea("sl:form", ratts);
	      rnvxml_ea("sl:name", NULL);
	      grx_xml(tp->gdl, "g:w");
	      rnvxml_ee("sl:name");
	      rnvxml_ee("sl:form");
	      /* don't set in_form here */
	    }
	  else
	    {
	      if (!s->u.f->sign->xref)
		id_or_ref = "ref";
	      else
		{
		  if (s->u.f->oid)
		    {
		      if (hash_find(xidseen, (uccp)s->u.f->oid))
			id_or_ref = "ref";
		      else
			hash_add(xidseen, (uccp)s->u.f->oid, "");
		    }
		}
	      ratts = rnvval_aa("x", "n", s->u.f->name, id_or_ref, s->u.f->oid ? s->u.f->oid : "", "sort", scode, NULL);
	      rnvxml_ea("sl:form", ratts);
	      rnvxml_ea("sl:name", NULL);
	      grx_xml(tp->gdl, "g:w");
	      rnvxml_ee("sl:name");
	      in_form = 1;
	    }
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

static const char *
x_tle_tag(enum sx_tle t)
{
  switch (t)
    {
    case sx_tle_componly:
      return "sl:compoundonly";
    case sx_tle_formproxy:
      return "sl:formproxy";
    case sx_tle_lref:
      return "sl:lref";
    case sx_tle_sign:
      return "sl:sign";
    case sx_tle_sref:
      return "sl:sref";
    default:
      return NULL;
    }
}
static unsigned char *
x_cdp_refs(struct sl_signlist *sl, struct sl_inst *s)
{
  struct sl_compound_digest *cdp = NULL;

  if (s->type == 's')
    cdp = hash_find(s->u.s->hcompounds, (uccp)"#digest_by_oid");
  else if (s->type == 'f')
    cdp = hash_find(s->u.f->sign->hcompounds, (uccp)"#digest_by_oid");
  if (cdp)
    {
      unsigned char *oids;
      int len = 0;
      int i;
      for (i = 0; cdp->memb[i]; ++i)
	;
      len = (i * (strlen("o1234567")+1))+1;
      oids = pool_alloc(len, sl->p);
      *oids = '\0';
      for (i = 0; cdp->memb[i]; ++i)
	{
	  strcat((char*)oids, cdp->memb[i]);
	  strcat((char*)oids, " ");
	}
      oids[strlen((ccp)oids)-1] = '\0';
      /*fprintf(stderr, "%s:#digest_by_oid=%s\n", s->u.s->name, oids);*/
      if (oids && *oids)
	return oids;
      else
	mesg_verr(&s->mloc, "@compoundonly entry %s not found in any compounds",
		  s->type == 's' ? s->u.s->name : s->u.f->name);
    }
  return NULL;
}

static struct rnvval_atts *
x_tle_atts(struct sl_signlist *sl, struct sl_inst *s)
{
  List *a = list_create(LIST_SINGLE);
  const char **atts = NULL;
  static char scode[32];
  struct rnvval_atts *ratts = NULL;

  list_add(a, "n");
  list_add(a, (void*)xmlify(s->u.s->name));
  
  if (s->u.s->oid)
    {
      list_add(a, "xml:id");
      list_add(a, (void*)s->u.s->oid);
    }

  if (s->u.s->sort > 0)
    (void)sprintf(scode, "%d", s->u.s->sort);
  if (*scode)
    {
      list_add(a, "sort");
      list_add(a, scode);
    }

  if (s->u.s->type == sx_tle_componly)
    {
      unsigned char *oids = x_cdp_refs(sl, s);
      if (oids)
	{
	  list_add(a, "cpd-refs");
	  list_add(a, oids);
	}
      else
	fprintf(stderr, "%s:#digest_by_oid=%s\n", s->u.s->name, "[not found]");
    }
  atts = list2chars(a);
  ratts = rnvval_aa_qatts((char**)atts, list_len(a)/2);
  list_free(a, NULL);
  return ratts;
}

/** Because this is called when walking groups->signs \c sl_inst*s here can be a sign or form inst
 */
static void
sx_w_x_sign(struct sx_functions *f, struct sl_signlist *sl, struct sl_inst *s, enum sx_pos_e p)
{
  static const char *in_sign = 0;

  if (p == sx_pos_inst)
    {
      if (in_sign)
	{
	  rnvxml_ee(in_sign);
	  in_sign = NULL;
	}

      xo_loc->file = s->mloc.file; xo_loc->line = s->mloc.line;
  
      if (s->type == 's')
	{
	  struct sl_token *tp = NULL;
	  tp = hash_find(sl->htoken, s->u.s->name);
	  ratts = x_tle_atts(sl, s);
	  rnvxml_ea((in_sign = x_tle_tag(s->u.s->type)), ratts);	  
	  if (s->u.s->smap)
	    {
	      ratts = rnvval_aa("x", "oid", s->u.s->smoid, NULL);
	      rnvxml_et("sl:smap", ratts, s->u.s->smap);
	    }
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
	{
	  rnvxml_ee(in_sign);
	  in_sign = NULL;
	}
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
