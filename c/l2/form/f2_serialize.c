#include <stdio.h>
#include <string.h>
#include <xmlutil.h>
#include "f2.h"
#include "ilem_form.h"
#include "props.h"

extern int lem_extended;

void
f2_attr(FILE *f_f2, const char *name, const char *value)
{
  if (value && *value)
    fprintf(f_f2, " %s=\"%s\"", name, xmlify((unsigned char *)value));
}

void
f2_attr_i(FILE *f_f2, const char *name, int value)
{
  if (value)
    fprintf(f_f2, " %s=\"%d\"", name, value);
}

static void
dump_norm(FILE *f_f2,struct f2 *fp)
{
  const unsigned char *tilde = (unsigned char *)strchr((char*)fp->morph,'~');
  if (tilde)
    {
      const unsigned char *m = fp->morph;
      fputs(" norm0=\"",f_f2);
      while (m != tilde)
	fputc(*m++,f_f2);
      if (fp->cf)
	fputs((const char*)xmlify((unsigned char *)fp->cf),f_f2);
      else
	fputs("X",f_f2);
      fputs((char*)++m,f_f2);
      fputc('"',f_f2);
    }
  else if (fp->norm)
    {
      f2_attr(f_f2,"norm",(char*)fp->norm);
    }
  else if (fp->cf)
    {
      f2_attr(f_f2,"norm",(char*)fp->cf);
    }
  else
    {
      /* no norm0 */
    }
}

void
f2_serialize_form(FILE *f_f2, struct f2 *f)
{
  fputs("<xff:f",f_f2);
  f2_attr(f_f2,"xml:lang",(char*)f->lang);
  f2_attr(f_f2,"form",(char*)f->form);
  f2_attr(f_f2,"oform",(char*)f->oform);
  f2_attr(f_f2,"cf",(char*)f->cf);
  if (f->cf)
    f2_attr(f_f2,"gw",(char*)f->gw);
  f2_attr(f_f2,"sense",(char*)f->sense);
  if (f->morph && lem_extended)
    dump_norm(f_f2,f);
  else
    f2_attr(f_f2,"norm",(char*)f->norm);
  f2_attr(f_f2,"pos",(char*)f->pos);
  f2_attr(f_f2,"epos",(char*)f->epos);
  f2_attr(f_f2,"base",(char*)f->base);
  f2_attr(f_f2,"cont",(char*)f->cont);
  f2_attr(f_f2,"stem",(char*)f->stem);
  f2_attr(f_f2,"morph",(char*)f->morph);
  f2_attr(f_f2,"morph2",(char*)f->morph2);
  fputs("/>",f_f2);
}

void
f2_serialize_form_2(FILE *f_f2, struct ilem_form *f)
{
  extern int lem_extended;
  int has_found = f->finds ? f->fcount : 0;
  int finds_index = 0;
  struct ilem_form**ffinds = f->finds;
  if (has_found < 0)
    has_found = 0;
  /* First print the principal form unconditionally */
  while (f)
    {
      fputs("<xff:f",f_f2);
#if 0
      f2_attr(f_f2,"rws",f->rws);
#endif
      f2_attr(f_f2,"rws_cf",f->rws_cf);
      f2_attr(f_f2,"aform",f->aform);
      f2_attr(f_f2,"type",f->type);
      if (f->result == -1 || f->invalid == 1)
	f2_attr(f_f2,"invalid","1");
      f2_attr(f_f2,"literal",f->literal);
      f2_attr(f_f2,"sublem",f->sublem);
      f2_attr(f_f2,"restrictor",f->restrictor);
      f2_attr(f_f2,"fdisamb",f->fdisamb);
      f2_attr(f_f2,"sdisamb",f->sdisamb);

#if 0
      f2_attr_i("freq", f->freq);
      f2_attr_i("pct", f->pct);
#endif

      if (f->newflag)
	fputs(" new=\"1\"",f_f2);
      if (f->explicit&M_NORM0)
	fputs(" ex_norm0=\"1\"",f_f2);
      if (f->explicit&M_SENSE)
	fputs(" ex_sense=\"1\"",f_f2);
      if (f->is_part == 1)
	fputs(" is_part=\"1\"",f_f2);
      if (f->props)
	{
	  fputc('>',f_f2);
	  props_dump_props(f,f_f2);
	  fputs("</xff:f>",f_f2);
	}
      else
	fputs("/>",f_f2);
      if (has_found)
	{
	  if (finds_index)
	    fputs("</l>", f_f2);
	  else
	    fprintf(f_f2,"<ll type=\"finds\">");
	  if (finds_index < has_found)
	    {
	      f = ffinds[finds_index++];
	      fputs("<l>",f_f2);
	    }
	  else
	    f = NULL;
	}
      else
	f = NULL;
    }
  if (has_found)
    fprintf(f_f2,"</ll>");
}
