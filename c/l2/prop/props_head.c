#include <stdlib.h>
#include <stdio.h>
#include <ctype128.h>
#include <string.h>
#include <f2.h>
#include <ilem_form.h>
#include "xcl.h"
#include "npool.h"
#include "props.h"
#include "ngram.h"

void
props_head_cue(struct xcl_context *xc, 
	     struct props_context *pcp, struct xcl_l *lp, struct props_prop *pp)
{
  struct props_head *php = malloc(sizeof(struct props_head));
  List *todo = hash_find(pcp->todo, (unsigned char *)lp->xml_id);
  php->xc = xc;
  php->lp = lp;
  php->pp = pp;
  if (!todo)
    {
      todo = list_create(LIST_SINGLE);
      hash_add(pcp->todo, (unsigned char *)lp->xml_id, todo);
    }
  list_add(todo,php);
}

void
props_head_reg(struct props_context *pcp, struct xcl_l *lp)
{
  hash_add(pcp->heads, (unsigned char *)lp->xml_id, lp);
}

/* we can only exec this when a successful match has already
   been made, so we can safely give a large conventional
   value for maxlem in props_find_node calls */
void
props_head_sub(struct props_head *php)
{
  struct xcl_l *target = NULL, *ref = NULL, *lp = php->lp;
  int i = lp->nth;
  struct xcl_c *parent = lp->parent;
  struct xcl_l *hp = NULL;
  while (i >= 0)
    {
      if (parent->children[i].c->node_type == xcl_node_l
	  && (hp = hash_find(php->xc->props->heads, 
			     (unsigned char *)parent->children[i].l->xml_id)))
	break;
      else
	--i;
    }
  if (hp)
    {
      char buf[128];
      const unsigned char *idref = NULL, *value = NULL;
      if (php->pp->where == -100000)
	{
	  /* put value on head node */
	  target = hp;
	  /* get value from ref node */
	  if (*php->pp->value == '#')
	    {
	      int where = atoi((const char *)&php->pp->value[1]);
	      ref = props_find_node(NULL,php->lp,where, 1000);
	      if (ref)
		{
		  sprintf(buf,"#%s",ref->xml_id);
		  idref = npool_copy((unsigned char *)buf, php->xc->pool);
		  sprintf(buf,"%s[%s]",ref->f->f2.cf,ref->f->f2.gw);
		  value = npool_copy((unsigned char *)buf, php->xc->pool);
		}
	    }
	  else
	    {
	      value = php->pp->value;
	    }
	}
      else
	{
	  /* put value on this node */
	  target = props_find_node(NULL,php->lp,php->pp->where, 1000);
	  /* get value from head node */
	  ref = hp;
	  sprintf(buf,"#%s",ref->xml_id);
	  idref = npool_copy((unsigned char *)buf, php->xc->pool);
	  sprintf(buf,"%s[%s]",ref->f->f2.cf,ref->f->f2.gw);
	  value = npool_copy((unsigned char *)buf, php->xc->pool);
	}
      if (target && target->f && (value || idref))
	props_add_prop(target->f,php->pp->group,php->pp->name,
		       value,(const char*)idref,NULL,NULL,get_ngram_id());
    }
}

void
props_heads(struct xcl_context *xc,struct xcl_l *lp)
{
  List *todo = hash_find(xc->props->todo, (unsigned char *)lp->xml_id);
  if (todo)
    list_exec(todo, (void(*)(void*))props_head_sub);
}
