#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <npool.h>
#include <memblock.h>

#include "cat.h"
#include "cat.tab.h"

struct npool *catpool;
struct mb *catchunk_mem;
struct mb *catnode_mem;

int cat_initted = 0;

static struct catchunk *head = NULL;
static struct catchunk *tail = NULL;

void
cat_init(void)
{
  if (cat_initted++)
    {
      head = tail = NULL;
      return;
    }
  catchunk_mem = mb_init(sizeof(struct catchunk),1024);
  catnode_mem = mb_init(sizeof(struct catnode),1024);
  catpool = npool_init();
  head = tail;
}

void
cat_term(void)
{
  if (cat_initted)
    {
      mb_term(catchunk_mem);
      npool_term(catpool);
      catchunk_mem = NULL;
      catpool = NULL;
      head = tail = NULL;
      cat_initted = 0;
    }
}

void
cat_chunk(int l, char *t)
{
  struct catchunk *c = mb_new(catchunk_mem);
  if (!head)
    head = tail = c;
  else
    {
      tail->next = c;
      tail = c;
    }
  tail->line = l;
  tail->text = t;
}

void
cat_cont(int l, char *t)
{
  fprintf(stderr, "cat_cont: tail=%s; t=%s\n", tail->text, t);
  while (isspace(t[1]))
    ++t;
  *t = ' ';
  if (tail)
    {
      unsigned char *newtext = npool_alloc(strlen(tail->text) + strlen(t) + 1, catpool);
      sprintf((char*)newtext, "%s%s", tail->text, t);
      fprintf(stderr, "cat_cont: newtext=%s\n", newtext);
      tail->text = (char*)newtext;
      tail->last = l;
    }
}

void
cat_dump(struct catchunk *ccp)
{
  struct catchunk *cp;
  for (cp = ccp; cp; cp = cp->next)
    printf("[%d]\t%s\n", cp->line, cp->text);
}

struct catchunk *
cat_read(const char *file)
{
  cat_init();
  catparse();
  cat_term();
  return head;
}

char *
cat_name(struct catchunk *cp)
{
  if ('@' == *cp)
    {
      char *end = ++cp;
      /* name is terminated by space or NULL */
      while (*end && (*end > 127 || !isspace(*end)))
	++end;
      if (isspace(*end))
	*end = '\0';
      return cp;
    }
  else
    return NULL;
}

struct catnode *
cat_herd(struct catchunk *cp, struct catconfig *cfg)
{
  struct catnode *head = NULL;
  List *cip_stack = NULL;
  struct catinfo *cip = NULL;
  
  head = mb_new(catnode_mem);
  head->n = cfg->head;
  cip = cfg->chkname(cn->n);

  cips = list_create(LIST_DOUBLE);
  push(cip_stack, cip);
  
  for (cp = ccp; cp; cp = cp->next)
    {
      if (*cp->text)
	{
	  struct catnode *cn = mb_new(catnode_mem);
	  static char *data;
	  
	  cn->n = cfg->getname(cp, &data);
	  if (cn->n && (cip = cfg->chkname(cn->n)))
	    {
	      /* link cn into the herd using the rules in the info struct */
	      switch (cip->rel)
		{
		case CI_PARENT:
		  /* this node acts a parent to all successive nodes
		     until another node with the same level; can be @end
		     XXX or @XXX, i.e., @entry is terminated by @end
		     entry or @entry. If cip->end == 1 it is an
		     error if there is no @end XXX.

		     Value is an integer; 1 is top level; 2 is second
		     level, etc.
		  */
		  break;
		case CI_CHILD:
		  /* attach as a child of the current parent node */
		  break;
		default:
		  /* no other cases */
		  break;
		}
	      cn->d = cfg->parse(data);
	    }
	  else
	    {
	      /* unknown name error */
	    }
	}
      else
	{
	  /* This is a paragraph break; can do validation of missing @end ... here */
	}      
    }
  return head;
}
