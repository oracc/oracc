#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <pool.h>
#include <memblock.h>

#include "cat.h"
#include "cat.tab.h"

static struct catstate *catstack_push(struct catnode *n, struct catinfo *i);
static struct catstate *catstack_pop(void);

Pool *catpool;
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
  catpool = pool_init();
  head = tail;
}

void
cat_term(void)
{
  if (cat_initted)
    {
      mb_term(catchunk_mem);
      pool_term(catpool);
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
cat_name(struct catchunk *cp, char **data)
{
  char *n = NULL;
  if (cp && '@' == *cp->text)
    {
      char *end = NULL;
      end = n = cp->text + 1;
      /* name is terminated by space or NULL */
      while (*end && (*end > 127 || !isspace(*end)))
	++end;
      if (isspace(*end))
	{
	  *end++ = '\0';
	  *data = end;
	}
      else
	{
	  char *tmp = (char*)npool_alloc((end - n) + 1, catpool);
	  strncpy(tmp, n, n - end);
	  tmp[n-end] = '\0';
	  n = tmp;
	  *data = end;
	}
      return n;
    }
  else
    return NULL;
}

#define CS_MAX 	16
static struct catstate cstack[CS_MAX];
static int cs_depth = 0;

static struct catstate *
catstack_push(struct catnode *n, struct catinfo *i)
{
  struct catstate *csp = NULL;
  if (cs_depth < CS_MAX)
    {
      csp = &cstack[cs_depth++];
      csp->cn = n;
      csp->cip = i;
      csp->end = i->end; /* csp->end is an editable version of 'end';
			    the one in csp->cip belongs to the
			    reference structure for the name */
    }
  else
    {
      fprintf(stderr, "catstack_push: nesting too deep\n");
    }
  return csp;
}

static struct catstate *
catstack_pop(void)
{
  struct catstate *csp = NULL;
  if (cs_depth)
    csp = &cstack[--cs_depth];
  else
    fprintf(stderr, "catstack_pop: attempt to pop empty stack\n");
  return csp;
}

void
cat_end(struct catstate *state, char *data)
{
  char *ename = data;
  while (isalnum(*data))
    ++data;
  if (isspace(*data))
    {
      *data++ = '\0';
      while (isspace(*data))
	++data;
    }
  else if (*data)
    {
      /* junk after data */
      fprintf(stderr, "cat_end: junk after data %s\n", data);
      *data = '\0';
    }
  if (!strcmp(state->cn->name, ename))
    {
      /* matched @end tag */
      state->end = 0;
    }
  else
    {
      /* mismatched @end tag */
    }
}

struct catnode *
cat_herd(struct catchunk *ccp, struct catconfig *cfg)
{
  struct catnode *head = NULL;
  struct catinfo *cip = NULL;
  struct catstate *state = NULL;
  struct catchunk *cp = NULL;
  
  head = mb_new(catnode_mem);
  head->name = cfg->head;
  cip = cfg->chkname(head->name, strlen(head->name));
  state = catstack_push(head, cip);
  
  for (cp = ccp; cp; cp = cp->next)
    {
      if (*cp->text)
	{
	  struct catnode *cn = mb_new(catnode_mem);
	  static char *data;
	  
	  cn->name = cfg->getname(cp, &data);
	  if (cn->name && (cip = cfg->chkname(cn->name, strlen(cn->name))))
	    {
	      /* always make cn the last child of curr */
	      if (state->cn->k)
		{
		  state->cn->last->next = cn;
		  state->cn->last = cn;
		}
	      else
		state->cn->last = state->cn->k = cn;

	      switch (cip->rel)
		{
		case CI_PARENT:
		  if (cip->depth > state->cip->depth)
		    state = catstack_push(cn, cip);
		  else if (cip->depth < state->cip->depth)
		    state = catstack_pop();
		  else
		    state->cn = cn;
		  /*cn->d = cfg->parse(data);*/
		  break;
		case CI_CHILD:
		  /*cn->d = cfg->parse(data);*/
		  break;
		case CI_END:
		  cat_end(state, data);
		  break;
		}
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
