#include <stdio.h>
#include "gx.h"

#include "iterator_fncs.c"

extern void iterator(struct cbd *c, iterator_fnc fncs[]);

static void f_proplist(const char *p);

void
identity(struct cbd*cbd)
{
  iterator_fnc *fncs = ifnc_init();
  iterator(cbd,fncs);
  free(fncs);
}

static void
f_alias(struct alias *a)
{
  printf("@alias %s\n", a->cgp->tight);
}

static void
f_cbd(struct cbd *c)
{
  printf("@project %s\n@lang %s\n@name %s\n", c->project, c->lang, c->name);
  if (list_len(c->proplists))
    list_exec(c->proplists, (list_exec_func*)f_proplist);
  printf("\n");
}

static void
f_entry(struct entry *e)
{
  if (e->ed)
    {
      switch (e->ed->type)
	{
	case ADD_E:
	  putchar('+');
	  break;
	case DEL_E:
	  putchar('-');
	  break;
	default:
	  break;
	}
    }
  printf("@entry %s\n", e->cgp->loose);
  if (e->ed)
    {
      switch (e->ed->type)
	{
	case DEL_E:
	  printf("#why: %s\n", e->ed->why);
	  break;
	case REN_E:
	  printf("> %s\n", e->ed->target->tight);
	  break;
	case MRG_E:
	  printf(">> %s\n", e->ed->target->tight);
	  break;
	default:
	  break;
	}
    }
}

static void
f_end_cbd(struct cbd *c)
{
  /* nothing to do in identity mode */
}

static void
f_end_entry(struct entry *e)
{
  printf("@end entry\n\n");
}

static void
f_parts(struct entry *e)
{
  if (e->parts->cgps && list_len(e->parts->cgps))
    {
      printf("@parts");
      List_node *lp;
      for (lp = e->parts->cgps->first; lp; lp = lp->next)
	printf(" %s", ((struct cgp*)(lp->data))->tight);
      printf("\n");
    }
}

static void
f_proplist(const char *p)
{
  printf("@proplist %s\n", p);
}
