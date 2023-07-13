#include <stdlib.h>
#include <tree.h>
#include <list.h>

#include "sll.h"
#include "gvl.h"

extern int gdl_corrq;
extern int gvl_strict;

static unsigned char *gvl_c_form(Node *ynp, void (*fnc)(Node *np, void *user));
static void gvl_c_node_orig(Node * np, void *user);
static void gvl_c_node_c10e(Node * np, void *user);

/* This routine should be called after cp->orig and cp->c10e have been set */
void
gvl_c(gvl_g *cp)
{
  unsigned const char *l = NULL;
  if ((l=gvl_lookup(cp->orig)))
    {
      /* The original form trumps the c10e form because there are some
	 challenges to building the c10e form with absolute
	 accuracy. So, if cp->orig is a known compound we reset
	 cp->c10e as well */
      cp->oid = (ccp)l;
      cp->sign = gvl_lookup(sll_tmp_key(l,""));
      cp->c10e = cp->orig;
    }
  else if (cp->c10e && (l=gvl_lookup(cp->c10e)))
    {
      cp->oid = (ccp)l;
      cp->sign = gvl_lookup(sll_tmp_key(l,""));
      if (strcmp((ccp)cp->orig, (ccp)cp->c10e) && gvl_strict)
	cp->mess = gvl_vmess("compound %s should be %s", cp->orig, cp->c10e);
    }
  else
    {
      unsigned char *c10e_no_p = sll_strip_pp(cp->c10e);
      if (c10e_no_p && (l=gvl_lookup(c10e_no_p)))
	{
	  cp->oid = (ccp)l;
	  cp->sign = gvl_lookup(sll_tmp_key(l,""));
	  if (strcmp((ccp)cp->orig, (ccp)cp->c10e) && gvl_strict)
	    {
	      cp->mess = gvl_vmess("compound %s should be %s", cp->orig, c10e_no_p);
	      cp->c10e = pool_copy(c10e_no_p, curr_sl->p);
	    }
	}
      else
	{
	   /* don't error when errors are voided, on corrections or
	      compounds with X--the latter should be configurable */
	  if (!gvl_void_messages && !gdl_corrq && !strchr((ccp)cp->orig,'X'))
	    {
	      if (strcmp((ccp)cp->orig, (ccp)cp->c10e) && c10e_no_p && strcmp((ccp)cp->c10e, (ccp)c10e_no_p))
		cp->mess = gvl_vmess("unknown compound: %s (also tried %s/%s)", cp->orig, cp->c10e, c10e_no_p);
	      else if (strcmp((ccp)cp->orig, (ccp)cp->c10e))
		cp->mess = gvl_vmess("unknown compound: %s (also tried %s)", cp->orig, cp->c10e);
	      else if (c10e_no_p && strcmp((ccp)cp->orig, (ccp)c10e_no_p))
		cp->mess = gvl_vmess("unknown compound: %s (also tried %s)", cp->orig, c10e_no_p);
	      else
		cp->mess = gvl_vmess("unknown compound: %s", cp->orig);
	    }
	}
      /*free(c10e_no_p);*/
    }
}

unsigned char *
gvl_c_orig(Node *ynp)
{
  return gvl_c_form(ynp, gvl_c_node_orig);
}

unsigned char *
gvl_c_c10e(Node *ynp)
{
  return gvl_c_form(ynp, gvl_c_node_c10e);
}

/* lc should be 0 on first pass and 1 on final pass */

static void
gvl_c_node_text(List *lp, Node *np, int lc)
{
  switch (np->name[2])
    {
    case 'M':
      if (lc)
	np->name = "g:m";
    case 'm':
      list_add(lp, "@");
      list_add(lp, (void*)np->text);
      break;
    case 'A':
      if (lc)
	np->name = "g:a";
    case 'a':
      list_add(lp, "~");
      list_add(lp, (void*)np->text);
      break;
    case 'F':
      if (lc)
	np->name = "g:f";
    case 'f':
      /* graphetic forms are not added here */
      break;
    default:
      list_add(lp, (void*)np->text);
      break;
    }
}

static void
gvl_c_node_orig(Node *np, void *user)
{
  if ((strlen(np->name) == 3 || strlen(np->name) == 4)
      && !strchr("abcfmz", np->name[2])
      && 'n' != np->rent->name[2]
      && 'q' != np->rent->name[2])
    {
      if (np->user)
	list_add((List*)user, (void*)((gvl_g*)(np->user))->orig);
      else if (np->text)
	gvl_c_node_text(user, np, 0);
      else if (!strcmp(np->name, "g:gp"))
	list_add((List*)user, "(");
    }
}

static void
gvl_c_node_c10e(Node *np, void *user)
{
  if ((strlen(np->name) == 3 || strlen(np->name) == 4)
      && !strchr("abcfmz", np->name[2])
      && 'n' != np->rent->name[2]
      && 'q' != np->rent->name[2])
    {
      if (np->user)
	{
	  gvl_g *gp = np->user;
	  if (gp->sign)
	    list_add((List*)user, (void*)gp->sign);
	  else if (gp->c10e)
	    list_add((List*)user, (void*)gp->c10e);
	  else
	    list_add((List*)user, (void*)gp->orig);
	}
      else if (np->text)
	{
	  if (':' == *np->text)
	    list_add((List*)user, ".");
	  else
	    gvl_c_node_text(user, np, 1);
	}
      else if (!strcmp(np->name, "g:gp"))
	list_add((List*)user, "(");
    }
}

static void
gvl_c_node_gp_c(Node *np, void *user)
{
  if (!strcmp(np->name, "g:gp"))
    list_add((List*)user, ")");
}

static unsigned char *
gvl_c_form(Node *ynp, void (*fnc)(Node *np, void *user))
{
  List *lp = list_create(LIST_SINGLE);
  unsigned char *p = NULL, *s = NULL, *ret = NULL, *t = NULL;
  node_iterator(ynp, lp, fnc, gvl_c_node_gp_c);
  p = s = list_concat(lp);
  ret = t = pool_alloc(strlen((ccp)s)+3, curr_sl->p);
  *t++ = '|';
  while (*s)
    if ('|' == *s)
      ++s;
    else
      *t++ = *s++;
  *t++ = '|';
  *t = '\0';
  free(p);
  list_free(lp, NULL);
  return ret;
}
