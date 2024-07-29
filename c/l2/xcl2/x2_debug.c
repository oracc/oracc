#include <stdio.h>
#include "hash.h"
#include "list.h"
#include "xmlutil.h"
#include "xcl.h"
#include "links.h"
#include "f2.h"
#include "../ilem/ilem_form.h"
#include "../ilem/ilem_para.h"

extern const char *xcl_c_type_str[];
extern const char *xcl_d_type_str[];
extern void x2_attr(FILE *f_dbg, const char *name, const char *value);
extern void x2_attr_i(FILE *f_dbg, const char *name, int value);

static FILE *static_f_dbg = NULL;

static void
x2d_serialize_one_l_sub(FILE *f_dbg, struct xcl_l*lp, struct ilem_form *fp)
{
  fputs("<l", f_dbg);
  if (!fp)
    x2_attr(f_dbg,"xml:id",lp->xml_id);
  x2_attr_i(f_dbg,"lnum",fp ? fp->lnum : lp->lnum);
  x2_attr(f_dbg,"ref",lp->ref);

  if (lp->cof_head)
    {
      x2_attr(f_dbg, "cof-head", lp->cof_head->xml_id);
      if (fp)
	fp->f2.sig = NULL;
      else if (lp->f)
	lp->f->f2.sig = NULL;
    }
  else if (lp->cof_tails)
    {
      struct xcl_l *tailp;
      fputs(" cof-tails=\"", f_dbg);
      for (tailp = list_first(lp->cof_tails); tailp; tailp = list_next(lp->cof_tails))
	{
	  fputs(tailp->xml_id, f_dbg);
	  if (tailp != list_last(lp->cof_tails))
	    fputc(' ', f_dbg);
	}
      fputs("\"", f_dbg);
    }

  if (fp)
    {
      if (BIT_ISSET(fp->f2.flags,F2_FLAGS_PARTIAL)
	  || BIT_ISSET(fp->f2.flags,F2_FLAGS_NO_FORM))
	x2_attr(f_dbg, "bad", "yes");

      x2_attr(f_dbg,"inst",fp->sublem);
      x2_attr(f_dbg,"sig",((const char*)fp->f2.sig));
      x2_attr(f_dbg,"tail-sig",((const char*)fp->f2.tail_sig));
    }
  else
    {
      x2_attr(f_dbg,"inst",lp->inst);
      if (lp->f)
	{
	  if (BIT_ISSET(lp->f->f2.flags, F2_FLAGS_NOT_IN_SIGS))
	    x2_attr(f_dbg,"newsig",(char *)lp->f->f2.sig);
	  else if (BIT_ISSET(lp->f->f2.flags, F2_FLAGS_NEW_BY_PROJ))
	    x2_attr(f_dbg,"exosig",(char *)lp->f->f2.sig);
	  else
	    x2_attr(f_dbg,"sig",(char *)lp->f->f2.sig);
	  x2_attr(f_dbg,"tail-sig",((const char*)lp->f->f2.tail_sig));
	  if (BIT_ISSET(lp->f->f2.flags,F2_FLAGS_PARTIAL)
	      || BIT_ISSET(lp->f->f2.flags,F2_FLAGS_NO_FORM))
	    x2_attr(f_dbg, "bad", "yes");
	}
    }

  fputs("/>",f_dbg);
#if 0
  if (lp->f)
    f2_serialize_form(f_dbg, &lp->f->f2);
  ilem_para_dump(f_dbg, lp);
  fputs("</l>",f_dbg);
#endif
}

static void
x2d_serialize_one_l(FILE *f_dbg, struct xcl_l*lp)
{
  struct ilem_form *fp = lp->f;

  if (fp && fp->ambig)
    {
      fputs("<ll", f_dbg);
      x2_attr(f_dbg,"xml:id",lp->xml_id);
      fputc('>',f_dbg);
      do
	{
	  x2d_serialize_one_l_sub(f_dbg, lp, fp);
	  fp = fp->ambig;
	}
      while (fp);
      fputs("</ll>", f_dbg);
    }
  else
    {
      x2d_serialize_one_l_sub(f_dbg, lp, NULL);
    }
}

static void
x2d_serialize_m(const unsigned char *key,void*val)
{
  if ('#' != *key)
    fprintf(static_f_dbg,"<m k=\"%s\">%s</m>",key,(char*)val);
}

static void
x2d_serialize_one_node(FILE*f_dbg,void *vp)
{
  enum xcl_node_types t = ((struct xcl_c *)vp)->node_type;
  /* int need_ll = 0; */
  int i;

#define cp ((struct xcl_c*)vp)
#define dp ((struct xcl_d*)vp)
#define lp ((struct xcl_l*)vp)

  switch (t)
    {
    case xcl_node_c:
      fprintf(f_dbg,"<c");
      x2_attr(f_dbg,"type",xcl_c_type_str[cp->type]);
      if (cp->subtype && *cp->subtype)
	{
	  if (cp->type == xcl_c_sentence
	      || cp->type == xcl_c_clause
	      || cp->type == xcl_c_phrase)
	    x2_attr(f_dbg,"tag",cp->subtype);
	  else
	    x2_attr(f_dbg,"subtype",cp->subtype);
	}
      if (cp->id)
	x2_attr(f_dbg,"xml:id",cp->id);
      x2_attr(f_dbg,"label",cp->label);
      x2_attr(f_dbg,"ref",cp->ref);
      x2_attr_i(f_dbg,"level",cp->level);
      fprintf(f_dbg," bracketing_level=\"%d\"",cp->bracketing_level);
      fprintf(f_dbg,">");
      if (cp->meta)
	{
	  fprintf(f_dbg,"<mds xml:id=\"%s\">",(char*)hash_find(cp->meta,
							       (unsigned char *)"#xml:id"));
	  static_f_dbg = f_dbg;
	  hash_exec2(cp->meta,x2d_serialize_m);
	  fputs("</mds>",f_dbg);
	}
      for (i = 0; i < cp->nchildren; ++i)
	{
	  switch (cp->children[i].c->node_type)
	    {
	    case xcl_node_c:
	      x2d_serialize_one_node(f_dbg,cp->children[i].c);
	      break;
	    case xcl_node_d:
	      x2d_serialize_one_node(f_dbg,cp->children[i].d);
	      break;
	    case xcl_node_l:
	      x2d_serialize_one_node(f_dbg,cp->children[i].l);
	      break;
	    }
	}
      fprintf(f_dbg,"</c>");
      break;
    case xcl_node_d:
      fprintf(f_dbg,"<d type=\"%s\" ref=\"%s\"/>",
	      xcl_d_type_str[dp->type],
	      dp->ref);
      break;
    case xcl_node_l:
      x2d_serialize_one_l(f_dbg,lp);
      break;
    }

#undef cp
#undef dp
#undef lp
}

void
xcl_debug_tree(FILE *f, void *vp)
{
  static_f_dbg = f;
  x2d_serialize_one_node(f,vp);
}
