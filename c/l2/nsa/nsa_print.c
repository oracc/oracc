#include <string.h>
#include "nsa.h"
static void render_refs(struct nsa_text_ref **refs, FILE *fp);
static void render_amount(struct nsa_amount *a, FILE *fp);

int indent = 0;

void
nsa_print(struct nsa_result *resp, FILE *fp)
{
  int n;
  for (n=0; resp->results[n]; ++n)
    {
      if (resp->label)
	{
	  extern char *PQ;
	  if (PQ)
	    fprintf(fp,"%s.",PQ);
	  fputs(resp->label,fp);
	}
      fprintf(fp, "#%d) ", n+1);
      if (resp->results[n]->type == NSA_R_UNPARSED)
	{
	  fputc('[',fp);
	  render_refs(resp->results[n]->refs, fp);
	  fputs("]\n",fp);
	}
      else
	{
	  render_refs(resp->results[n]->refs, fp);
	  fputc('\n',fp);
	  render_amount(resp->results[n]->amount, fp);
	}
    }
}

const char *
show_char_list_str(List *lp)
{
  static char buf[1024];
  const char *s;
  *buf = '\0';
  for (s = list_first(lp); s; s = list_next(lp))
    {
      strcat(buf,s);
      if (lp->rover->next)
	strcat(buf,"; ");
    }
  return buf;
}

const char *
show_sys_list_str(List *steps)
{
  static char buf[1024];
  struct nsa_step *s;
  *buf = '\0';
  for (s = list_first(steps); s; s = list_next(steps))
    {
      strcat(buf,s->sys->n);
      if (steps->rover->next)
	strcat(buf,"; ");
    }
  return buf;
}

static void
render_refs(struct nsa_text_ref **refs, FILE *fp)
{
  int i;
  for (i = 0; refs[i]; ++i)
    {
      fprintf(fp,"%s",ref_text(refs[i]));
      if (refs[i+1])
	fputc(' ', fp);
    }
}
#if 0
static void
render_commodity(struct nsa_token**com, FILE *fp)
{
  if (com)
    {
      List *c = list_create(LIST_SINGLE);
      int i;
      struct nsa_text_ref **refs, *r;
      for (i = 0; com[i]; ++i)
	find_refs(com[i], c);
      refs = malloc((list_len(c)+1) * sizeof(struct nsa_text_ref*));
      for (i = 0, r = list_first(c); r; r = list_next(c), ++i)
	refs[i] = r;
      refs[i] = NULL;
      render_refs(refs,fp);
      list_free(c,NULL);
    }
}
#endif

static void
render_amount(struct nsa_amount *a, FILE *fp)
{
  if (a->measure->type == NSA_T_MEASURE && a->measure->d.m->system)
    {
      fputs("== [",fp);
      fprintf(fp,"%s] [",a->measure->d.m->system->n);
      fprintf(fp,"%s] [",a->measure->d.m->rendered_mev);
    }
  else if (a->measure->type == NSA_T_COUNT /* && a->measure->d.c->system */) /* count's need better system handling */
    {
      fprintf(fp, "== [base %d", a->measure->d.c->base);
    }
  else
    {
      fprintf(fp,"~~ [%s] [", show_sys_list_str(a->measure->d.m->cands));
    }
  if (a->commodity && commodity_head(a->commodity))
    fprintf(fp,"] [%s",commodity_head(a->commodity));
  fputs("]\n", fp);
}

unsigned char *
ref_text(struct nsa_text_ref*text)
{
  switch (text->ptype)
    {
    case NSA_P_LITERAL:
      return (unsigned char *)text->t.literal;
    case NSA_P_LEMM:
      return (unsigned char *)text->t.lemmptr->f->f2.form;
    case NSA_P_LINK:
      return NULL;
    default:
      return NULL;
    }
}

void
render_children(struct nsa_token *t,FILE *fp)
{
  int i;
  indent += 2;
  for (i = 0; t->children[i]; ++i)
    render_token(t->children[i], fp);
  indent -= 2;
}

void
render_count(struct nsa_count *c, FILE *fp)
{
  fprintf(fp, "%lld/%d",c->num,c->den);
}

void
render_token(struct nsa_token *t, FILE *fp)
{
  int i;
  for (i = 0; i < indent; ++i)
    fputc(' ',fp);
  switch (t->type)
    {
    case NSA_T_GRAPHEME:
      if (!grapheme_is_sysdet(t))
	{
	  fputs("[GRAPHEME]",fp);
	  if (grapheme_unit(t))
	    {
	      const unsigned char *gt = nsa_grapheme_text(t);
	      fprintf(fp, "\t%s%s%s[%s :: %s]", 
		      grapheme_overt(t) ? "" : "*",
		      gt,
		      (strlen((const char *)gt)<8)?"\t":" ",
		      grapheme_num(t) ? grapheme_num(t) : "",
		      grapheme_unit(t) ? grapheme_unit(t) : "");
	    }
	  else
	    fprintf(fp, "\t%s", nsa_grapheme_text(t));
	  fputc('\n',fp);
	}
      break;
    case NSA_T_COUNT:
      fputs("[COUNT]\t",fp);
      render_count(t->d.c,fp);
      fputc('\n',fp);
      break;
    case NSA_T_UNIT:
      fputs("[UNIT] ",fp);
      fprintf(fp, "%s%s [%s]\n",
	      unit_implicit(t)?"*":"",
	      unit_name(t),
	      unit_step(t) 
	      ? unit_step(t)->sys->n
	      : show_sys_list_str(unit_cands(t)));
      break;
    case NSA_T_QUANTITY:
      fputs("[QUANTITY] ",fp);
      if (quantity_aev(t))
	render_count(quantity_aev(t),fp);
      fputc('\n',fp);
      indent += 2;
      render_token(quantity_count(t),fp);
      render_token(quantity_unit(t),fp);
      indent -= 2;
      break;
    case NSA_T_MEASURE:
      fputs("[MEASURE] ",fp);
      if (measure_aev(t))
	render_count(measure_aev(t),fp);
      if (measure_rendered_mev(t))
	fprintf(fp, " = %s", measure_rendered_mev(t));
      fputc('\n',fp);
      break;
    case NSA_T_COMMODITY:
      fputs("[COMMODITY] ", fp);
      if (commodity_cands(t))
	{
	  fprintf(fp, " (head '%s' => %s)", 
		  commodity_head(t),
		  show_char_list_str(commodity_cands(t)));
	}
      fputc('\n',fp);
      /* drop down to rendering of children at end of subr */
      break;
    case NSA_T_AMOUNT:
      fputs("[AMOUNT]\n",fp);
      indent += 2;
      render_token(amount_measure(t),fp);
      if (amount_commodity(t))
	render_token(amount_commodity(t),fp);
      if (amount_measure(t)->type == NSA_T_MEASURE 
	  && measure_sysdet(amount_measure(t)))
	{
	  struct nsa_system *sysd = grapheme_is_sysdet(measure_sysdet(amount_measure(t)));
	  fprintf(fp,"  [SYSDET]\n    [GRAPHEME] %s => %s\n",
		  nsa_grapheme_text(measure_sysdet(amount_measure(t))),
		  sysd ? sysd->n : ""
		  );
	}
      indent -= 2;
      return;
    case NSA_T_UNKNOWN:
      fputs("[UNKNOWN]\n",fp);
      break;
    case NSA_T_NONE:
      fputs("[NONE]\n",fp);
      break;
    case NSA_T_STOP:
      fputs("[STOP]\n",fp);
      break;
    }
  if (t->children)
    render_children(t,fp);
}

void
nsa_divider(const char *mess, FILE *fp)
{
  fprintf(fp,"=======================%s=====================\n",mess);
}

void
nsa_show_tokens(struct nsa_parser *p, FILE *fp)
{
  struct nsa_token *t;
  for (t = list_first(p->toks); t; t = list_next(p->toks))
    render_token(t, fp);
}

const unsigned char *
nsa_grapheme_text(struct nsa_token *t)
{
  if (t && t->type == NSA_T_GRAPHEME && t->d.g)
    {
      struct nsa_text_ref *ref = grapheme_text_ref(t);
      switch (ref->ptype)
	{
	case NSA_P_LEMM:
	  return 
	    (unsigned char *)
	    (ref->t.lemmptr->f->f2.cf 
	     ? ref->t.lemmptr->f->f2.cf 
	     : ref->t.lemmptr->f->f2.form);
	case NSA_P_LITERAL:
	  return ref->t.literal;
	case NSA_P_LINK:
	  return (unsigned char *)ref->t.linkptr->title;
	default:
	  return NULL;
	}
    }
  return NULL;
}
