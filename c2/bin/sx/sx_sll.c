#include <signlist.h>
#include <sx.h>

/** This is the output for the signlist database; it does not use sx_walk
 */

extern Hash *oids;

static const char *curr_oid;

static void sx_s_compounds(FILE *fp, const unsigned char *name, const char *tag, const char **oids);
static void sx_s_form(FILE *f, struct sl_form *s);
static void sx_s_homophones(FILE *fp, struct sl_signlist *sl);
static void sx_s_letter(FILE *f, struct sl_letter *l);
static void sx_s_list(FILE *f, struct sl_list *s);
static void sx_s_qualified(FILE *fp, struct sl_signlist *sl);
static void sx_s_sign(FILE *f, struct sl_sign *s);
static void sx_s_str(FILE *fp, const char *tag, const unsigned char *s);
static void sx_s_unicode(FILE *fp, struct sl_unicode *up);
static void sx_s_value(FILE *f, struct sl_value *v);
static void sx_s_values_by_oid(FILE *fp, struct sl_signlist *sl);

/* This is the entry point for sll output */
void
sx_s_sll(FILE *f, struct sl_signlist *sl)
{
  int i;
  for (i = 0; i < sl->nsigns; ++i)
    sx_s_sign(f, sl->signs[i]);
  for (i = 0; i < sl->nforms; ++i)
    sx_s_form(f, sl->forms[i]);
  for (i = 0; i < sl->nlists; ++i)
    sx_s_list(f, sl->lists[i]);
  for (i = 0; i < sl->nvalues; ++i)
    sx_s_value(f, sl->values[i]);
  sx_s_values_by_oid(f, sl);
  sx_s_homophones(f, sl);
  sx_s_qualified(f, sl);
  fputs("#letters\n", f);
  for (i = 0; i < sl->nletters; ++i)
    sx_s_letter(f, &sl->letters[i]);
}

static void
sx_s_letter(FILE *f, struct sl_letter *l)
{
  int i = 0;
  for (i = 0; i < l->ngroups; ++i)
    {
      int j;
      for (j = 0; j < l->groups[i].nsigns; ++j)
	if ('s' == l->groups[i].signs[j]->type)
	  fprintf(f, "%s;let\tl%04d\n", l->groups[i].signs[j]->u.s->oid, l->code);
    }
}

static void
sx_s_aka(FILE *fp, const char *oid, List *aka)
{
  Memo_str *n;
  /* 2023-8-25 removed ';aka' -- these are now non-reciprocal entries
     and the status as an @aka can be determined by looking up the
     OID, then looking up the OID's name, and comparing the first name
     to the OID's name */
  for (n = list_first(aka); n; n = list_next(aka))
    fprintf(fp, "%s\t%s\n", n->s, oid);
}

static void
sx_s_compounds_driver(FILE *fp, const unsigned char *n, Hash *hcompounds)
{
  struct sl_compound_digest *dp = hash_find(hcompounds, (uccp)"#digest_by_oid");
  if (dp->memb)
    sx_s_compounds(fp, n, "cmemb", dp->memb);
  if (dp->initial)
    sx_s_compounds(fp, n, "cinit", dp->initial);
#if 0
  /* This is not used in sldb2.tsv; possibly a bug */
  if (dp->medial)
    sx_s_compounds(fp, n, "cmedial", dp->medial);
#endif
  if (dp->final)
    sx_s_compounds(fp, n, "clast", dp->final);
  if (dp->container)
    sx_s_compounds(fp, n, "contains", dp->container);
  if (dp->contained)
    sx_s_compounds(fp, n, "contained", dp->contained);
}

static void
sx_s_values_by_oid(FILE *fp, struct sl_signlist *sl)
{
  const char **keys;
  int nkeys, i;
  keys = hash_keys2(sl->values_by_oid, &nkeys);
  for (i = 0; i < nkeys; ++i)
    {
      int j;
      const char **vals = hash_find(sl->values_by_oid, (uccp)keys[i]);
      if (vals)
	{
	  fprintf(fp, "%s;values\t", keys[i]);
	  for (j = 0; vals[j]; ++j)
	    {
	      if (j)
		fputc(' ', fp);
	      fputs((ccp)vals[j], fp);
	    }
	  fputc('\n', fp);
	}
    }
}

static void
sx_s_sign(FILE *f, struct sl_sign *s)
{
  if (!s->xref && (s->oid || s->smoid))
    {
      if (s->smoid)
	curr_oid = s->smoid;
      else
	curr_oid = s->oid;

      if (s->smoid)
	fprintf(f, "%s\t%s\n", s->smap, curr_oid);
      fprintf(f, "%s\t%s\n", s->name, curr_oid);
      fprintf(f, "%s\t%s\n", curr_oid, s->name);

      sx_s_unicode(f, &s->U);

      if (s->nforms)
	{
	  int i;
	  fprintf(f, "%s;forms\t", s->oid);
	  for (i = 0; i < s->nforms; ++i)
	    {
	      if (s->forms[i]->u.f->oid)
		{
		  if (i)
		    fputc(' ', f);
		  fputs(s->forms[i]->u.f->oid, f);
		}
	    }
	  fputc('\n', f);
	}
      
      if (s->aka)
	sx_s_aka(f, s->oid, s->aka);
    }

  /* this is for both sign and form_as_sign where the sign wrapper
     carries the homophone data */
  if (s->hcompounds)
    sx_s_compounds_driver(f, s->name, s->hcompounds);
  
}

static void
sx_s_form(FILE *f, struct sl_form *s)
{
  if (s->sign->xref)
    {
      curr_oid = s->oid;
      fprintf(f, "%s\t%s\n", s->name, s->oid);
      fprintf(f, "%s\t%s\n", s->oid, s->name);

      sx_s_unicode(f, &s->U);

      if (s->aka)
	sx_s_aka(f, s->oid, s->aka);
    }
  if (s->nowners)
    {
      int i;
      fprintf(f, "%s;signs\t", s->oid);
      for (i = 0; i < s->nowners; ++i)
	{
	  if (i)
	    fputc(' ', f);
	  if (s->owners_sort[i]->oid)
	    fputs(s->owners_sort[i]->oid, f);
	  else if (s->owners_sort[i]->smoid)
	    fputs(s->owners_sort[i]->smoid, f);
	}
      fputc('\n', f);
    }
}

static void
sx_s_list(FILE *f, struct sl_list *s)
{
  int i;
  fprintf(f, "%s;l\t", s->name);
  for (i = 0; s->oids[i]; ++i)
    {
      if (i)
	fputc(' ', f);
      fputs(s->oids[i], f);
    }
  fputc('\n', f);
}

static void
sx_s_value(FILE *f, struct sl_value *v)
{
  if (!v->qvmust && !v->atf)
    {
      if (v->parents)
	{
	  struct sl_inst *ip;
	  if (v->parents->signs)
	    {
	      if ((ip = list_first(v->parents->signs)))
		fprintf(f, "%s\t%s\n", v->name, ip->u.s->oid);
	    }
	  else if (v->parents->forms)
	    {
	      if ((ip = list_first(v->parents->forms)))
		fprintf(f, "%s\t%s\n", v->name, ip->u.f->oid);
	    }
	}
    }
}

static void
sx_s_str(FILE *fp, const char *tag, const unsigned char *s)
{
  fprintf(fp, "%s;%s\t%s\n", curr_oid, tag, s);
}

static void
sx_s_unicode(FILE *fp, struct sl_unicode *up)
{
  if (up->uhex)
    sx_s_str(fp, "ucode", (uccp)up->uhex);
  else if (up->useq)
    sx_s_str(fp, "ucode", (uccp)up->useq);
  if (up->utf8)
    sx_s_str(fp, "uchar", (uccp)up->utf8);
}

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
	  if (spv[j]->oid)
	    {
	      if (j)
		fputc(' ', fp);
	      if (spv[j]->i == 1)
		fputs(spv[j]->oid, fp);
	      else if (spv[j]->i == 1000)
		fprintf(fp, "%s/0", spv[j]->oid);
	      else 
		fprintf(fp, "%s/%d", spv[j]->oid, spv[j]->i);
	    }
	}
      fputc('\n', fp);
    }
}

static void
sx_s_compounds(FILE *fp, const unsigned char *name, const char *tag, const char **oids)
{
  int i;
  fprintf(fp, "%s;%s\t", name, tag);
  for (i = 0; oids[i]; ++i)
    {
      if (i)
	fputc(' ', fp);
      fputs(oids[i], fp);
    }
  fputc('\n', fp);
}

static void
sx_s_qualified(FILE *fp, struct sl_signlist *sl)
{
  int i;
  for (i = 0; i < sl->nvalues; ++i)
    {
      if (sl->values[i]->parents)
	{
	  if (sl->values[i]->parents->qvoids)
	    {
	      int j;
	      if (sl->values[i]->qvmust)
		{
		  fprintf(fp, "%s;q\t", sl->values[i]->name);
		  for (j = 0; sl->values[i]->parents->qvoids[j]; ++j)
		    {
		      if (j)
			fputc(' ', fp);
#if 1
		      fputs(sl->values[i]->parents->qvoids[j], fp);
#else
		      fputs(hash_find(oids, (uccp)sl->values[i]->parents->qvoids[j]), fp);
#endif
		    }
		  fputc('\n', fp);
		}
	      for (j = 0; sl->values[i]->parents->qvoids[j]; ++j)
		fprintf(fp, "%s(%s);qv\t%s\n", sl->values[i]->name,
			(const char*)hash_find(oids, (uccp)sl->values[i]->parents->qvoids[j]),
			sl->values[i]->parents->qvoids[j]);
	    }
	}
    }
}
