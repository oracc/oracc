#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <pool.h>
#include <memo.h>

#include "cat.h"
#include "cat.tab.h"

Pool *catpool;
Memo *catchunk_mem;
Memo *catnode_mem;

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
  catchunk_mem = memo_init(sizeof(struct catchunk),1024);
  catnode_mem = memo_init(sizeof(struct catnode),1024);
  catpool = pool_init();
  head = tail;
}

void
cat_term(void)
{
  if (cat_initted)
    {
      memo_term(catchunk_mem);
      pool_term(catpool);
      catchunk_mem = NULL;
      catpool = NULL;
      head = tail = NULL;
      cat_initted = 0;
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
  return catyacc();
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
	  char *tmp = (char*)pool_alloc((end - n) + 1, catpool);
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

/* return 1 if end matches current parent; 0 otherwise */
struct catstate *
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

  while (1)
    {
      if (!strcmp(state->cn->name, ename))
	{
	  /* matched @end tag */
	  state->end = 0;
	  return catstack_pop();
	}
      else
	{
	  /* mismatched @end tag */
	  if (!(state = catstack_pop()))
	    break;
	}
    }
  return NULL;
}

struct catnode *
cat_herd(struct catchunk *ccp, struct catconfig *cfg)
{
  struct catnode *head = NULL;
  struct catinfo *cip = NULL, *head_cip = NULL;
  struct catstate *state = NULL;
  struct catchunk *cp = NULL;
  
  head = memo_new(catnode_mem);
  head->name = cfg->head;
  head_cip = cfg->chkname(head->name, strlen(head->name));
  state = catstack_push(head, head_cip);
  
  for (cp = ccp; cp; cp = cp->next)
    {
      if (*cp->text)
	{
	  struct catnode *cn = memo_new(catnode_mem);
	  static char *data;
	  
	  fprintf(stderr, "cat_herd: depth before processing node: %d\n", state->cip->depth);
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
		  fprintf(stderr, "cat_herd: curr=%s@%d: adding parent %s@%d; data=%s\n",
			  state->cn->name, state->cip->depth,
			  cn->name, cip->depth, data);
		  if (cip->depth > state->cip->depth)
		    state = catstack_push(cn, cip);
		  else if (cip->depth < state->cip->depth)
		    state = catstack_pop();
		  else
		    state->cn = cn;
		  /*cn->d = cfg->parse(data);*/
		  break;
		case CI_CHILD:
		  fprintf(stderr, "cat_herd: curr=%s@%d: adding child %s@; data=%s\n",
			  state->cn->name, state->cip->depth,
			  cn->name, data);
		  /*cn->d = cfg->parse(data);*/
		  break;
		case CI_END:
		  fprintf(stderr, "cat_herd: curr=%s@%d: adding end %s@; data=%s\n",
			  state->cn->name, state->cip->depth,
			  cn->name, data);
		  state = cat_end(state, data);
		  if (!state)
		    {
		      /* recover from mismatch by resetting to root */
		      fprintf(stderr, "mismatched @end\n");
		      catstack_reset();
		      state = catstack_push(head, head_cip);
		    }
		  break;
		}
	    }
	  else
	    {
	      /* unknown name error */
	    }
	  fprintf(stderr, "cat_herd: depth after processing node: %d\n", state->cip->depth);
	}
      else
	{
	  /* This is a paragraph break; can do validation of missing @end ... here */
	}      
    }
  return head;
}
