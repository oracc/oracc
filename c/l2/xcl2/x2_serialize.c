#include <stdio.h>
#include "hash.h"
#include "list.h"
#include "xmlutil.h"
#include "xcl.h"
#include "links.h"
#include "f2.h"
#include "warning.h"
#include "../prop/props.h"
#include "../ilem/ilem_form.h"
#include "../ilem/ilem_para.h"

#define XML_DECL "<?xml version=\"1.0\" encoding=\"utf-8\"?>"

/* Set this to 1 to get an XML dump of the XCL tree structure with no phrase skipping */
static int xcl_debug = 0;

static FILE *static_f_xcl;

extern int pretty;

static const char *psu_list_lang = NULL;

const char *xcl_c_type_str[] =
  {
    "text","discourse","chunk","sentence","clause","phrase","line-var","word-var"
  };
const char *xcl_d_type_str[] =
  {
    "line-start","break","cell-start","cell-end",
    "field-start","field-end",
    "gloss-start","gloss-end",
    "punct",
    "object","surface","column",
    "nonw" , "nonx"
  };

/*
 * NOTE: NO F2 SERIALIZATION CODE SHOULD BE USED HERE EXCEPT f2_serialize
 * ON struct f2.
 */

void
x2_attr(FILE *f_xcl, const char *name, const char *value)
{
  if (value && *value)
    fprintf(f_xcl, " %s=\"%s\"", name, xmlify((unsigned char *)value));
}

void
x2_attr_i(FILE *f_xcl, const char *name, int value)
{
  if (value)
    fprintf(f_xcl, " %s=\"%d\"", name, value);
}

static void
serialize_one_l_sub(FILE *f_xcl, struct xcl_l*lp, struct ilem_form *fp)
{
  fputs("<l", f_xcl);
  if (!fp)
    x2_attr(f_xcl,"xml:id",lp->xml_id);
  x2_attr_i(f_xcl,"lnum",fp ? fp->lnum : lp->lnum);
  x2_attr(f_xcl,"ref",lp->ref);
  x2_attr(f_xcl,"ftype",lp->subtype);

  if (lp->cof_head)
    {
      x2_attr(f_xcl, "cof-head", lp->cof_head->xml_id);
      if (fp)
	fp->f2.sig = NULL;
      else if (lp->f)
	lp->f->f2.sig = NULL;
    }
  else if (lp->cof_tails)
    {
      struct xcl_l *tailp;
      fputs(" cof-tails=\"", f_xcl);
      for (tailp = list_first(lp->cof_tails); tailp; tailp = list_next(lp->cof_tails))
	{
	  fputs(tailp->xml_id, f_xcl);
	  if (tailp != list_last(lp->cof_tails))
	    fputc(' ', f_xcl);
	}
      fputs("\"", f_xcl);
    }

  if (fp)
    {
      x2_attr(f_xcl,"inst",fp->sublem);
      if (BIT_ISSET(fp->f2.flags,F2_FLAGS_INVALID)
	  || BIT_ISSET(fp->f2.flags,F2_FLAGS_PARTIAL)
	  || BIT_ISSET(fp->f2.flags,F2_FLAGS_NO_FORM))
	x2_attr(f_xcl, "bad", "yes");
      else
	{
	  x2_attr(f_xcl,"sig",((const char*)fp->f2.sig));
	  x2_attr(f_xcl,"tail-sig",((const char*)fp->f2.tail_sig));
	}
    }
  else
    {
      x2_attr(f_xcl,"inst",lp->inst);
      if (lp->f)
	{
	  if (BIT_ISSET(lp->f->f2.flags,F2_FLAGS_INVALID))
	    x2_attr(f_xcl, "bad", "yes");
	  else if (BIT_ISSET(lp->f->f2.flags, F2_FLAGS_NOT_IN_SIGS))
	    {
	      extern const char *phase;
	      extern int cbd_lem_sigs;
	      const char *ophase = phase;
	      phase = "sig";
	      if (lp->f->f2.sig)
		{
		  x2_attr(f_xcl,"newsig",(char *)lp->f->f2.sig);
		  if (cbd_lem_sigs)
		    vnotice2((char*)lp->f->file,lp->f->lnum,"\t%s", lp->f->f2.sig);
		}
	      else if (lp->f->f2.tail_sig)
		{
 #if 0
		  /* You can't generate newsig for tail sigs; if the tail is new, make the whole
		     COF new so that harvest creates the right members */
		  x2_attr(f_xcl,"newsig",(char *)lp->f->f2.tail_sig);
		  if (cbd_lem_sigs)
		    vnotice2((char*)lp->f->file,lp->f->lnum,"\t%s", lp->f->f2.tail_sig);
#endif
		}
	      phase = ophase;
	    }
	  else if (BIT_ISSET(lp->f->f2.flags,F2_FLAGS_PARTIAL)
	      || BIT_ISSET(lp->f->f2.flags,F2_FLAGS_NO_FORM))
	    x2_attr(f_xcl, "bad", "yes");
	  else
	    {
	      if (BIT_ISSET(lp->f->f2.flags, F2_FLAGS_NEW_BY_PROJ)
		       || BIT_ISSET(lp->f->f2.flags, F2_FLAGS_NEW_BY_LANG))
		{
		  x2_attr(f_xcl,"exosig",(char *)lp->f->f2.sig);
		  x2_attr(f_xcl,"exoprj",(char *)lp->f->f2.exo_project);
		  x2_attr(f_xcl,"exolng",(char *)lp->f->f2.exo_lang);
		}
	      else
		x2_attr(f_xcl,"sig",(char *)lp->f->f2.sig);
	      x2_attr(f_xcl,"tail-sig",((const char*)lp->f->f2.tail_sig));
	    }
	}
    }

  fputs(">",f_xcl);

  if (lp->f)
    {
      f2_serialize_form(f_xcl, &lp->f->f2);
      if (lp->f->props)
	props_dump_props(lp->f,f_xcl);
    }

  ilem_para_dump(f_xcl, lp);
  fputs("</l>",f_xcl);
}

static void
serialize_one_l(FILE *f_xcl, struct xcl_l*lp)
{
  struct ilem_form *fp = lp->f;

  if (fp && fp->ambig)
    {
      fputs("<ll", f_xcl);
      x2_attr(f_xcl,"xml:id",lp->xml_id);
      fputc('>',f_xcl);
      do
	{
	  serialize_one_l_sub(f_xcl, lp, fp);
	  if (pretty)
	    fputc('\n',f_xcl);
	  fp = fp->ambig;
	}
      while (fp);
      fputs("</ll>", f_xcl);
    }
  else
    {
      serialize_one_l_sub(f_xcl, lp, NULL);
      if (pretty)
	fputc('\n',f_xcl);
    }
}

static void
serialize_m(unsigned char *key,void*val)
{
  if ('#' != *key)
    fprintf(static_f_xcl,"<m k=\"%s\">%s</m>",key,xmlify((char*)val));
}

static int
no_words(struct xcl_c*cp)
{
  int i;
  for (i = 0; i < cp->nchildren; ++i)
    {
      switch (cp->children[i].c->node_type)
	{
	case xcl_node_c:
	case xcl_node_l:
	  return 0;
	default:
	  break;
	}
    }
  return 1;
}

static int 
singleton_phrase_children(struct xcl_c*cp, int *pi)
{
  int i;
  int nphrases = 0;
  *pi = 0;
  for (i = 0; i < cp->nchildren; ++i)
    {
      switch (cp->children[i].c->node_type)
	{
	case xcl_node_c:
	  if (cp->children[i].c->type == xcl_c_phrase)
	    {
	      if (++nphrases > 1)
		return 0;
	      else
		*pi = i;
	    }
	  break;
	case xcl_node_l:
	  return 0;
	default:
	  break;
	}
    }
  return 1;
}

static void
serialize_one_node(FILE*f_xcl,void *vp)
{
  enum xcl_node_types t = ((struct xcl_c *)vp)->node_type;
  /* int need_ll = 0; */
  int i, unwrapping = 0, phrase_index = 0;

#define cp ((struct xcl_c*)vp)
#define dp ((struct xcl_d*)vp)
#define lp ((struct xcl_l*)vp)

  switch (t)
    {
    case xcl_node_c:
      if (cp->type == xcl_c_sentence)
	{ 
	  if (no_words(cp))
	    unwrapping = 1;
	  else
	    {
	      int skipped_any_phrases = 0;
	      int set_cp_subtype = 0;
	      while (cp->nchildren && singleton_phrase_children(cp, &phrase_index))
		{
		  const char *phrase_subtype = cp->children[phrase_index].c->subtype;
		  int skip_phrase = 0;
		  if (!phrase_subtype)
		    phrase_subtype = (const char *)ilem_para_head_label(cp,1);
		  if (phrase_subtype)
		    {
		      if ((set_cp_subtype || !cp->subtype) 
			  && (!phrase_subtype || 'S' == *phrase_subtype))
			{
			  cp->subtype = phrase_subtype;
			  set_cp_subtype = 1;
			  skip_phrase = 1;
			  /* either this was already NULL or we've used it as
			     phrase_subtype */
			  cp->children[phrase_index].c->subtype = NULL;
			}
		    }
		  else
		    skip_phrase = 1;
		  if (skip_phrase)
		    {
		      union xcl_u tmp = cp->children[phrase_index];
		      union xcl_u *free_me = cp->children;
		      union xcl_u *new_c = NULL;
		      int n_new_c = 0, src, dest;

		      n_new_c = cp->nchildren + tmp.c->nchildren - 1;
		      new_c = malloc(n_new_c * sizeof(union xcl_u));
		      for (src = dest = 0; src < phrase_index; )
			new_c[dest++].c = cp->children[src++].c;
		      for (src = 0; src < tmp.c->nchildren; ++src)
			new_c[dest++].c = tmp.c->children[src].c;
		      for (src = phrase_index + 1; src < cp->nchildren; )
			new_c[dest++].c = cp->children[src++].c;
		      cp->nchildren = cp->children_alloced = n_new_c;
		      cp->children = new_c;
#if 0
		      cp->children = tmp.c->children;
		      cp->nchildren = tmp.c->nchildren;
		      cp->children_alloced = tmp.c->children_alloced;
#endif
		      free(free_me);
		      tmp.c->nchildren = tmp.c->children_alloced = 0;
		      tmp.c->children = NULL;
		      ++skipped_any_phrases;
		    }
		  else
		    break;
		}
	      /* if (skipped_any_phrases) */
		goto skipping_phrase;
	    }
	}
      else if (cp->type == xcl_c_phrase && singleton_phrase_children(cp, &phrase_index))
	{
	  unwrapping = 1;
	  if (cp->subtype && !cp->children[phrase_index].c->subtype)
	    cp->children[phrase_index].c->subtype = cp->subtype;
	}
      else
	{
	skipping_phrase:
	  fprintf(f_xcl,"<c");
	  x2_attr(f_xcl,"type",xcl_c_type_str[cp->type]);
	  if (cp->type == xcl_c_sentence)
	    x2_attr(f_xcl,"implicit","yes");
	  if (cp->subtype && *cp->subtype)
	    {
	      if (cp->type == xcl_c_sentence
		  || cp->type == xcl_c_clause
		  || cp->type == xcl_c_phrase)
		x2_attr(f_xcl,"tag",cp->subtype);
	      else
		x2_attr(f_xcl,"subtype",cp->subtype);
	    }
	  if (cp->id)
	    x2_attr(f_xcl,"xml:id",cp->id);
	  x2_attr(f_xcl,"label",cp->label);
	  x2_attr(f_xcl,"ref",cp->ref);
	  x2_attr_i(f_xcl,"level",cp->level);
	  fprintf(f_xcl," bracketing_level=\"%d\"",cp->bracketing_level);
	  fprintf(f_xcl,">");
	  if (cp->meta)
	    {
	      fprintf(f_xcl,"<mds xml:id=\"%s\">",(char*)hash_find(cp->meta,
								   (unsigned char *)"#xml:id"));
	      static_f_xcl = f_xcl;
	      hash_exec2(cp->meta,serialize_m);
	      fputs("</mds>",f_xcl);
	    }
	}
      for (i = 0; i < cp->nchildren; ++i)
	{
	  switch (cp->children[i].c->node_type)
	    {
	    case xcl_node_c:
	      serialize_one_node(f_xcl,cp->children[i].c);
	      break;
	    case xcl_node_d:
	      serialize_one_node(f_xcl,cp->children[i].d);
	      break;
	    case xcl_node_l:
	      serialize_one_node(f_xcl,cp->children[i].l);
	      break;
	    }
	}
      if (!unwrapping)
	fprintf(f_xcl,"</c>");
      break;
    case xcl_node_d:
      fprintf(f_xcl,"<d type=\"%s\"",xcl_d_type_str[dp->type]);
      if (dp->ref)
	fprintf(f_xcl, " ref=\"%s\"", dp->ref);
      if (dp->subtype)
	fprintf(f_xcl, " subtype=\"%s\"", dp->subtype);
      fputs("/>", f_xcl);
      break;
    case xcl_node_l:
#if 0
      if (lp->f && lp->f->acount)
	{
	  fprintf(f_xcl,"<ll type=\"ambig\" xml:id=\"%s\" ref=\"%s\">",lp->xml_id, lp->ref);
	  need_ll = 1;
	}
#endif
      serialize_one_l(f_xcl,lp);
#if 0
      while (need_ll)
	{
	  fprintf(f_xcl,"</ll>");
	  --need_ll;
	}
#endif
      break;
    }

#undef cp
#undef dp
#undef lp
}

static void
serialize_one_psu(const unsigned char*psu)
{
  fprintf(static_f_xcl,"<psu lang=\"%s\">%s</psu>",psu_list_lang,xmlify(psu));
}

static void
serialize_psu_list(char *key,List *lp)
{
  psu_list_lang = key;
  list_exec(lp,(list_exec_func*)serialize_one_psu);
}

static void
serialize_psus(FILE *f_xcl,Hash_table*psus)
{
  static_f_xcl = f_xcl;
  fputs("<psus>",f_xcl);
  hash_exec2(psus, (hash_exec2_func*)serialize_psu_list);
  fputs("</psus>",f_xcl);
  static_f_xcl = NULL;
}

void
x2_serialize(struct xcl_context *xc, FILE *f_xcl, int with_xml_decl)
{
  if (!xc || !xc->root)
    return;

  if (with_xml_decl)
    fprintf(f_xcl,"%s\n",XML_DECL);
  fputs("<xcl xmlns=\"http://oracc.org/ns/xcl/1.0\" xmlns:xff=\"http://oracc.org/ns/xff/1.0\"",
	f_xcl);
  if (xc->project)
    fprintf(f_xcl," project=\"%s\"",xc->project);
  if (xc->textid)
    fprintf(f_xcl," textid=\"%s\"",xc->textid);
  if (xc->file)
    fprintf(f_xcl," file=\"%s\"",xc->file);
  if (xc->langs)
    fprintf(f_xcl," langs=\"%s\"",xc->langs);
  fputc('>',f_xcl);
  if (xc->psus->key_count > 0)
    serialize_psus(f_xcl,xc->psus);
  if (xcl_debug)
    xcl_debug_tree(f_log, xc->root);
  serialize_one_node(f_xcl,xc->root);
  if (xc->linkbase)
    links_dump(f_xcl, xc->linkbase);
  fputs("</xcl>",f_xcl);
}
