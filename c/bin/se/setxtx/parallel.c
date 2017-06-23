/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: parallel.c,v 0.3 1997/09/08 14:50:16 sjt Exp $
*/
#include <psd_base.h>
#include "index.h"

extern int swc_flag;

struct any_type *parallels_mm;
int parallels_depth;
struct parallel *curr_parallel = NULL;
static void push_parallel (void);
static void pop_parallel (void);

void
begin_parallels ()
{
  push_parallel ();
  parallels_depth = 0;
  if (curr_parallel)
    {
      curr_parallel->child = new (parallels_mm);
      curr_parallel->child->parent = curr_parallel;
      curr_parallel = curr_parallel->child;
      curr_parallel->child = NULL;
      curr_parallel->empty_branch_flag = 0;
    }
  else
    {
      curr_parallel = new (parallels_mm);
      curr_parallel->parent = NULL;
    }
  curr_parallel->start_column = start_column;
  curr_parallel->word_id = curr_word_id;
  curr_parallel->start_node = curr_node;
}

void
end_parallels ()
{
  if (!curr_parallel)
    return;
  if (!swc_flag)
    {
      if (curr_parallel->empty_branch_flag)
	start_column = curr_parallel->start_column;
      else
	start_column = curr_parallel->end_column+1;
    }
  curr_node = curr_parallel->start_node;
  pop_parallel ();
}

static union pparms_u 
{
  int i;
  struct parallel *p;
} *parms_stack = NULL;
static int parms_stack_len = 0;
static int parms_stack_top = 0;

static void
push_parallel ()
{
  if ((parms_stack_len+3) >= parms_stack_top)
    {
      parms_stack_len += 16;
      parms_stack = realloc (parms_stack, 
			     parms_stack_len * sizeof(union pparms_u));
    }
  parms_stack[parms_stack_top++].i = parallels_depth;
  parms_stack[parms_stack_top++].i = branch_id;
  parms_stack[parms_stack_top++].p = curr_parallel;
}

static void
pop_parallel ()
{
  if (parms_stack_top)
    {
      curr_parallel = parms_stack[--parms_stack_top].p;
      branch_id = parms_stack[--parms_stack_top].i;
      parallels_depth = parms_stack[--parms_stack_top].i;
    }
  else
    {
      fprintf(stderr,"parms stack underflow\n");
      curr_parallel = NULL;
    }
}
