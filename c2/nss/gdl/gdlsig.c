#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include "gdl.h"

int deep_sig = 0;

void gdlsig_descend(Node *np, List *lp);
const char *gdlsig_sep(const char *sep);

const char *
gdlsig_str(Mloc *mp, unsigned char *atf, int frag_ok, int deep)
{
  Tree *tp = NULL;
  int saved_deep = deep_sig;
  const char *sig = NULL;
  
  deep_sig = deep;
  gdlparse_reset();
  tp = gdlparse_string(mp, (char*)atf);
  if (tp)
    sig = gdlsig(tp);
  deep_sig = saved_deep; 
  return sig;
}

void
gdlsig_addoid(Node *np, List *lp)
{
  if (np->user)
    {
      gvl_g *gp = np->user;
      if (deep_sig && gp->deep)
	gdlsig_descend(gp->deep, lp);
      else if (deep_sig && !strcmp(np->name, "g:c"))
	gdlsig_descend(np, lp);
      else if (gp->oid)
	list_add(lp, (char*)gp->oid);
      else
	list_add(lp, "q99");
    }
  else
    list_add(lp, "q99");
}

Node *
gdlsig_oidnode(Node *np)
{
  if (strlen(np->name) == 3)
    {
      switch (np->name[2])
	{
	case 'v':
	case 's':
	case 'c':
	  return np;
	  break;
	case 'q':
	case 'n':
	  return np->kids->next;
	  break;
	default:
	  return NULL;
	}
    }
  else
    return NULL;
}

void
gdlsig_descend(Node *np, List *lp)
{
  Node *npp;
  for (npp = np->kids; npp; npp = npp->next)
    {
      Node *oidnode = gdlsig_oidnode(npp);
      if (oidnode)
	gdlsig_addoid(oidnode, lp);
      else if (!strcmp(npp->name, "g:d"))
	list_add(lp, (char*)gdlsig_sep((char*)npp->text));
      else
	gdlsig_descend(npp, lp);
    }
}

const char *
gdlsig_sep(const char *sep)
{
  switch (*sep)
    {
    case '.':
    case '-':
    case ':':
      return ".";
      break;
    default:
      return sep;
    }
}

const char *
gdlsig(Tree *tp)
{
  if (tp && tp->root && tp->root->kids)
    {
      List *lp = list_create(LIST_SINGLE);
      const char *sig = NULL;
      Node *np;
      for (np = tp->root->kids; np; np = np->next)
	{
	  Node *oidnode = gdlsig_oidnode(np);
	  if (oidnode)
	    gdlsig_addoid(oidnode, lp);
	  else if (!strcmp(np->name, "g:d"))
	    list_add(lp, (char*)gdlsig_sep((char*)np->text));
	  else if (!strcmp(np->name, "g:det"))
	    {
	      if (np->prev)
		list_add(lp, ".");
	      gdlsig_descend(np, lp);
	      if (np->next)
		list_add(lp, ".");
	    }
	  else if (np->kids)
	    gdlsig_descend(np, lp);
	}
      sig = (const char *)list_concat(lp);
      list_free(lp, NULL);
      return sig;
    }
  return NULL;
}
