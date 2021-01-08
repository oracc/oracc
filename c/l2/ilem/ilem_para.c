#include <string.h>
#include <ctype128.h>
#include "warning.h"
#include "xcl.h"
#include "ilem_para.h"
#include "pool.h"
#include "xmlutil.h"

static int bracketing_level = 0;

#undef C
#define C(x) #x,
static const char *const LPC_names[] = { LPC };
static const char *const LPT_names[] = { LPT };

const unsigned char *
ilem_para_head_label(struct xcl_c *c, int depth)
{
  if (c->children)
    {
      int i;
      for (i = 0; i < c->nchildren; ++i)
	{
	  switch (c->children[i].c->node_type)
	    {
	    case xcl_node_l:
	      {
		struct ilem_para *p;
		for (p = c->children[i].l->ante_para; p; p = p->next)
		  if (p->type == LPT_label)
		    return p->text;
	      }
	      break;
	    case xcl_node_c:
	      /* This will return NULL if the chunk has no lemmata--is
		 that right? */
	      if (--depth > 0)
		return ilem_para_head_label(c->children[i].c, depth);
	      break;
	    default:
	      break;
	    }
	}
    }
  return NULL;
}

static void
add_lp(struct ilem_para **lpp, enum ilem_para_class c, enum ilem_para_type t,
       unsigned const char *text, int level)
{
  struct ilem_para *lp = *lpp;
  if (lp)
    {
      while (lp->next)
	lp = lp->next;
      lp->next = calloc(1, sizeof(struct ilem_para));
      lp = lp->next;
    }
  else
    {
      lp = calloc(1, sizeof(struct ilem_para));
      *lpp = lp;
    }

  lp->class = c;
  lp->type = t;
  lp->text = text;
  lp->level = level;
}

static unsigned char *
next_feature(unsigned char *c)
{
  while (*c && !isspace(*c))
    ++c;
  if (*c)
    *c++ = '\0';
  return c;
}

static unsigned char *
add_lp_label(struct ilem_para **lp, unsigned const char *c)
{
  unsigned const char *label = (unsigned const char *)"#none";
  if (c[1] && !isspace(c[1]))
    {
      label = ++c;
      while (*c && !isspace(*c))
	++c;
    }
  add_lp(lp, LPC_syntax, LPT_label, label, bracketing_level);
  return (unsigned char *)c;
}

void
ilem_reset_bracketing_level(void)
{
  bracketing_level = 0;
}

struct ilem_para *
ilem_para_parse(struct xcl_context *xc, unsigned const char *s, unsigned char **end, int err_lnum, enum ilem_para_pos pos)
{
#if 1
  unsigned char *c = pool_copy(s);
#else
  unsigned char *c = npool_copy(pool,s);
#endif
  struct ilem_para *lp = NULL;

  while (*c)
    {
      while (isspace(*c))
	++c;
      switch (*c)
	{
	case '#':
	  if (c[1] == '#')
	    add_lp(&lp, LPC_linkset, LPT_linkset_def, c+2, bracketing_level);
	  else
	    add_lp(&lp, LPC_linkset, LPT_linkset_member, ++c, bracketing_level);
	  break;
	case '@':
	  add_lp(&lp, LPC_pointer, LPT_pointer_anchor, ++c, bracketing_level);
	  break;
	case '=':
	  add_lp(&lp, LPC_pointer, LPT_pointer_ref, ++c, bracketing_level);
	  break;
	case '$':
	  if (strchr((char*)c,'='))
	    add_lp(&lp, LPC_property, LPT_long_prop, ++c, bracketing_level);
	  else
	    add_lp(&lp, LPC_property, LPT_short_prop, ++c, bracketing_level);
	  break;
	case ',':
	  if (pos == ilem_para_pos_post)
	    {
	      add_lp(&lp, LPC_boundary, LPT_phrase, (unsigned char *)",", bracketing_level);
	      c = add_lp_label(&lp, c);
	    }
	  else
	    vwarning2(file, err_lnum, "syntax boundaries must come after lemma and before ';'");
	  break;
	case '.':
	  if (bracketing_level != 0)
	    {
	      vwarning2(file, err_lnum, "mismatched brackets detected at end of sentence");
	      bracketing_level = 0;
	    }
	  if (pos == ilem_para_pos_post)
	    {
	      add_lp(&lp, LPC_boundary, LPT_sentence, (unsigned char *)",", bracketing_level);
	      c = add_lp_label(&lp, c);
	    }
	  else
	    vwarning2(file, err_lnum, "syntax boundaries must come after lemma and before ';'");
	  break;
	case '+':
	  switch (c[1])
	    {
	    case ':':
	      if (pos == ilem_para_pos_post)
		{
		  add_lp(&lp, LPC_boundary, LPT_discourse, (unsigned char*)"+:", bracketing_level);
		  c = add_lp_label(&lp, c);
		}
	      else
		vwarning2(file, err_lnum, "syntax boundaries must come after lemma and before ';'");	      
	      break;
	    case '.':
	      if (bracketing_level != 0)
		{
		  vwarning2(file, err_lnum, "mismatched brackets detected at end of sentence");
		  bracketing_level = 0;
		}
	      if (pos == ilem_para_pos_post)
		{
		  add_lp(&lp, LPC_boundary, LPT_sentence, (unsigned char*)"+.", bracketing_level);
		  c = add_lp_label(&lp, c);
		}
	      else
		vwarning2(file, err_lnum, "syntax boundaries must come after lemma and before ';'");
	      break;
	      /*
		case ';':
		add_lp(&lp, LPC_boundary, LPT_clause, (unsigned char*)"+;", bracketing_level);
		break;
		case ',':
		add_lp(&lp, LPC_boundary, LPT_phrase, (unsigned char*)"+,", bracketing_level);
		break;
	      */
	    case '/':
	      add_lp(&lp, LPC_syntax, LPT_prn, (unsigned char*)"+/", bracketing_level);
	      break;
	    case '|':
	      add_lp(&lp, LPC_syntax, LPT_or, (unsigned char*)"+|", bracketing_level);
	      break;
	    case '&':
	      add_lp(&lp, LPC_syntax, LPT_and, (unsigned char*)"+&", bracketing_level);
	      break;
	    case '<':
	      add_lp(&lp, LPC_syntax, LPT_mod, (unsigned char*)"+<", bracketing_level);
	      break;
	    case '>':
	      add_lp(&lp, LPC_syntax, LPT_premod, (unsigned char*)"+>", bracketing_level);
	      break;
	    default:
	      goto ret;
	    }
	  break;
	case '-':
	  switch (c[1])
	    {
	    case ':':
	      add_lp(&lp, LPC_boundary, LPT_no_discourse, (unsigned char*)"-:", bracketing_level);
	      break;
	    case '.':
	      add_lp(&lp, LPC_boundary, LPT_no_sentence, (unsigned char*)"-.", bracketing_level);
	      break;
	    case ';':
	      add_lp(&lp, LPC_boundary, LPT_no_clause, (unsigned char*)"-;", bracketing_level);
	      break;
	    case ',':
	      add_lp(&lp, LPC_boundary, LPT_no_phrase, (unsigned char*)"-,", bracketing_level);
	      break;
	    case '/':
	      add_lp(&lp, LPC_syntax, LPT_no_prn, (unsigned char*)"-/", bracketing_level);
	      break;
	    case '|':
	      add_lp(&lp, LPC_syntax, LPT_no_or, (unsigned char*)"-|", bracketing_level);
	      break;
	    case '&':
	      add_lp(&lp, LPC_syntax, LPT_no_and, (unsigned char*)"-&", bracketing_level);
	      break;
	    case '<':
	      add_lp(&lp, LPC_syntax, LPT_no_mod, (unsigned char*)"-<", bracketing_level);
	      break;
	    case '>':
	      add_lp(&lp, LPC_syntax, LPT_no_premod, (unsigned char*)"->", bracketing_level);
	      break;
	    default:
	      goto ret;
	    }
	  break;
	case '(':
	  add_lp(&lp, LPC_syntax, LPT_brack_o, (unsigned char*)"(", bracketing_level++);
	  c = add_lp_label(&lp, c);
	  break;
	case ')':
	  add_lp(&lp, LPC_syntax, LPT_brack_c, (unsigned char*)")", --bracketing_level);
	  break;
	case '%':
	  if (c[1] == '%')
	    {
	      unsigned char *nsa_start = c+2, *nsa_end = c+2;
	      char *nsa_tok = NULL; /* should use a string pool from somewhere */
	      while (*nsa_end && ';' != *nsa_end && !isspace(*nsa_end))
		++nsa_end;
	      if (pos == ilem_para_pos_post)
		{
		  /* warn about only %% allowed in ante-lem */;
		}
	      else
		{
		  if (nsa_end == nsa_start)
		    add_lp(&lp, LPC_nsa, LPT_nsa_stop, (unsigned char*)"%%", bracketing_level);
		  else
		    {
		      nsa_tok = malloc((nsa_end - nsa_start) + 1);
		      strncpy((char *)nsa_tok, (char*)nsa_start, nsa_end - nsa_start);
		      nsa_tok[nsa_end-nsa_start] = '\0';
		      add_lp(&lp, LPC_nsa, LPT_nsa_system, (unsigned char*)nsa_tok, bracketing_level);
		    }
		}
	    }
	  break;
	default:
	  goto ret;
	  break;
	}
      c = next_feature(c);
    }
 ret:
  if (end)
    *end = c;
  return lp;
}

static enum xcl_c_types
map_boundary(enum ilem_para_type lpt_t)
{
  switch (lpt_t)
    {
    case LPT_discourse:
      return xcl_c_discourse;
    case LPT_sentence:
      return xcl_c_sentence;
    case LPT_clause:
      return xcl_c_clause;
    case LPT_phrase:
      return xcl_c_phrase;
    default:
      return xcl_c_top;
    }
}

static void
process_boundaries(struct xcl_context*xc,struct ilem_para *p,int pos)
{
  struct ilem_para *pp;
  for (pp = p; pp; pp = pp->next)
    {
      if (pp->class == LPC_boundary)
	{
	  enum xcl_c_types xcl_t = map_boundary(pp->type);
	  if (xcl_t != xcl_c_top)
	    {
	      xcl_insert_ub(xc, pos, xcl_t,pp->level);
	      /* FIXME: if xc->curr->parent is NULL where are we supposed to put the annotation? */
	      if (pp->next && pp->next->type == LPT_label
		  && xc->curr->parent
		  && xc->curr->parent->nchildren > 1)
		xc->curr->parent->children[xc->curr->parent->nchildren-2].c->subtype = (const char*)pp->next->text;
	    }
	}
      else if (pp->class == LPC_syntax)
	{
	  if (pp->type == LPT_brack_o)
	    {
#if 1
	      struct xcl_l *carry_over = NULL;
	      if (pos && xc->curr->nchildren)
		{
		  carry_over = xc->curr->children[xc->curr->nchildren-1].l;
		  --xc->curr->nchildren;
		}
	      /* just step down from the current phrase; if it turns out
		 that this is , (NP ...) , we'll eliminate the unnecessary
		 level on output
	       */
	      xcl_chunk(xc, NULL, xcl_c_phrase);
	      xc->curr->bracketing_level = pp->level;
	      if (pp->next && pp->next->type == LPT_label)
		xc->curr->subtype = (const char *)pp->next->text;
	      xcl_chunk(xc, NULL, xcl_c_phrase);
	      if (carry_over)
		xcl_add_child(xc->curr, carry_over, carry_over->node_type);
#else
	      struct xcl_l *carry_over = NULL;
	      if (pos && xc->curr->nchildren)
		{
		  carry_over = xc->curr->children[xc->curr->nchildren-1].l;
		  --xc->curr->nchildren;
		}
	      xcl_fix_context(xc,NULL);
	      xcl_chunk(xc, NULL, xcl_c_phrase);
	      xc->curr->bracketing_level = pp->level;
	      if (pp->next && pp->next->type == LPT_label)
		xc->curr->subtype = pp->next->text;
	      if (carry_over)
		xcl_add_child(xc->curr, carry_over, carry_over->node_type);
#endif
	    }
	  else if (pp->type == LPT_brack_c)
	    {
	      xcl_chunk_end(xc); /* end the implicit , chunk */
	      xcl_chunk_end(xc); /* end the parenned chunk */
	    }
	}
    }
}

void
ilem_para_boundaries(struct xcl_l*lp, struct xcl_context*xc)
{
  if (xc && lp)
    {
      if (lp->ante_para)
	process_boundaries(xc,lp->ante_para,1);
      if (lp->post_para)
	process_boundaries(xc,lp->post_para,0);
    }
}

static void
ilem_para_dump_one(FILE*fp,struct ilem_para *p,const char *pos)
{
  struct ilem_para *pp;
  fprintf(fp,"<para pos=\"%s\">",pos);
  for (pp = p; pp; pp = pp->next)
    {
      fprintf(fp,"<p class=\"%s\" type=\"%s\" text=\"%s\" bracketing_level=\"%d\"/>",
	      LPC_names[pp->class],
	      LPT_names[pp->type],
	      xmlify(pp->text),
	      pp->level);
    }
  fputs("</para>",fp);
}

void
ilem_para_dump(FILE *fp, struct xcl_l *lp)
{
  if (fp && lp)
    {
      if (lp->ante_para)
	ilem_para_dump_one(fp,lp->ante_para,"ante");
      if (lp->post_para)
	ilem_para_dump_one(fp,lp->post_para,"post");
    }
}

static void
ilem_para_dump_text(FILE *fp, struct ilem_para *p)
{
  struct ilem_para *pp;
  for (pp = p; pp; pp = pp->next)
    fprintf(fp,"%s ", pp->text);
}
void
ilem_para_dump_text_ante(FILE *fp, struct xcl_l *lp)
{
  if (fp && lp)
    ilem_para_dump_text(fp,lp->ante_para);
}
void
ilem_para_dump_text_post(FILE *fp, struct xcl_l *lp)
{
  if (fp && lp)
    ilem_para_dump_text(fp,lp->post_para);
}

struct ilem_para *
ilem_para_find(struct xcl_l *lp, enum ilem_para_class class, enum ilem_para_type type)
{
  struct ilem_para *pp;
  for (pp = lp->ante_para; pp; pp = pp->next)
    {
      if (pp->class == class && pp->type == type)
	return pp;
    }  
  for (pp = lp->post_para; pp; pp = pp->next)
    {
      if (pp->class == class && pp->type == type)
	return pp;
    }
  return NULL;
}
