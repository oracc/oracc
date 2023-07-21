#include <signlist.h>
#include <sx.h>

extern Hash *oids;

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
static void sx_s_LIST(struct sl_functions *f, struct sl_list *s);
static void sx_s_VALUE(struct sl_functions *f, struct sl_value *v);
static void sx_s_homophones(FILE *fp, struct sl_signlist *sl);

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
  for (i = 0; i < sl->nlists; ++i)
    sx_s_LIST(f, sl->lists[i]);
  for (i = 0; i < sl->nvalues; ++i)
    sx_s_VALUE(f, sl->values[i]);
  sx_s_homophones(f->fp, sl);
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
  if (!s->xref)
    {
      curr_oid = s->oid;
      fprintf(f->fp, "%s\t%s\n", s->name, curr_oid);
      sx_s_unicode(f->fp, &s->U);
    }
}

static void
sx_s_form(struct sl_functions *f, struct sl_inst *s)
{
  /* NOT USED */
}

static void
sx_s_FORM(struct sl_functions *f, struct sl_form *s)
{
  if (s->sign->xref)
    {
      curr_oid = s->oid;
      fprintf(f->fp, "%s\t%s\n", s->name, curr_oid);
      sx_s_unicode(f->fp, &s->U);
    }
}

static void
sx_s_list(struct sl_functions *f, struct sl_inst *l)
{
}

static void
sx_s_LIST(struct sl_functions *f, struct sl_list *s)
{
  int i;
  fprintf(f->fp, "%s;l\t", s->name);
  for (i = 0; s->oids[i]; ++i)
    {
      if (i)
	fputc(' ', f->fp);
      fputs(s->oids[i], f->fp);
    }
  fputc('\n', f->fp);
}

static void
sx_s_value(struct sl_functions *f, struct sl_inst *v)
{
}

static void
sx_s_VALUE(struct sl_functions *f, struct sl_value *v)
{
  if (v->sowner)
    {
      fprintf(f->fp, "%s\t%s\n", v->name, v->sowner->oid);
      if (v->nfowners)
	{
	  int i;
	  for (i = 0; i < v->nfowners; ++i)
	    {
	      const char *sname = NULL, *oid = NULL;
	      if ('s' == v->fowners_i_sort[i]->type)
		{
		  sname = (ccp)v->fowners_i_sort[i]->u.s->name;
		  oid = (ccp)v->fowners_i_sort[i]->u.s->oid;
		}
	      else
		{
		  sname = (ccp)v->fowners_i_sort[i]->u.f->name;
		  oid = (ccp)v->fowners_i_sort[i]->u.f->oid;
		}
	      fprintf(f->fp, "%s(%s);qv\t%s\n", v->name, sname, oid);
	    }
	}
    }
  else
    {
      int i;
      fprintf(f->fp, "%s;q\t", v->name);
      for (i = 0; v->oids[i]; ++i)
	{
	  if (i)
	    fputc(' ', f->fp);
	  fputs(hash_find(oids, (uccp)v->oids[i]), f->fp);
	}
      fputs("\n", f->fp);
      for (i = 0; v->oids[i]; ++i)
	fprintf(f->fp, "%s(%s);qv\t%s\n", v->name, (ccp)hash_find(oids, (uccp)v->oids[i]), v->oids[i]);
    }
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

static void
sx_s_homophones(FILE *fp, struct sl_signlist *sl)
{
  const char **keys;
  int nkeys, i;
  keys = hash_keys2(sl->homophones, &nkeys);
  for (i = 0; i < nkeys; ++i)
    {
      struct sl_split_value **spv = NULL;
      List *lp = NULL;
      int j;
      
      fprintf(fp, "%s;h\t", keys[i]);
      lp = hash_find(sl->homophones, (uccp)keys[i]);
      spv = (struct sl_split_value**)list2array(lp);
      for (j = 0; j < list_len(lp); ++j)
	{
	  if (j)
	    fputc(' ', fp);
	  if (spv[j]->i == 1)
	    fputs(spv[j]->oid, fp);
	  else	    
	    fprintf(fp, "%s/%d", spv[j]->oid, spv[j]->i);
	}
      fputc('\n', fp);
    }
}
