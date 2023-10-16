#include <stdlib.h>
#include <mesg.h>
#include "gdl.tab.h"

static int o_c_map[] = 
  { '<',	'>',
    L_ang_par,	R_ang_par,
    L_cur_par,	R_cur_par,
    L_dbl_ang,	R_dbl_ang,
    L_dbl_cur,	R_dbl_cur,
    '[',       	']',
    L_uhs,     	R_uhs,
    L_lhs,     	R_lhs,
    '(',	')',
    CLP, 	CRP,
    '{',	'}',
    -1
  };

static struct s_o_c { int tok; const char *str; } s_o_c_map[] =
  {
    { '<', "<" },
    { '>', ">" },
    { '{', "{" },
    { '}', "}" },
    { L_ang_par, "<(" },
    { R_ang_par, ")>" },
    { L_cur_par, "{(" },
    { R_cur_par, ")}" },
    { L_dbl_ang, "<<" },
    { R_dbl_ang, ">>" },
    { L_dbl_cur, "{{" },
    { R_dbl_cur, "}}" },
    { '[', "[" },
    { ']', "]" },
    { L_uhs, "[#" },
    { R_uhs, "#]" },
    { L_lhs, "#[" },
    { R_lhs, "]#" },
    { '(', "(" },
    { ')', ")" },
    { CLP , "(" },
    { CRP , ")" },
    { -1 , NULL }
  };

static int c_of_o[END];
static int o_of_c[END];
static const char *s_of_oc[END];

int *state_stack = NULL, *break_stack = NULL;
static int break_alloced = 0, state_alloced = 0;
static int break_top = -1, state_top = -1;
#define OC_ALLOC	4

void
gdl_balance_init(void)
{
  int i;

  if (state_stack)
    {
      break_top = state_top = -1;
      return;
    }
    
  for (i = 0; o_c_map[i] > 0; ++i)
    {
      if (i%2)
	o_of_c[o_c_map[i]] = o_c_map[i-1];
      else
	c_of_o[o_c_map[i]] = o_c_map[i+1];      
    }
  
  for (i = 0; s_o_c_map[i].tok != -1; ++i)    
    s_of_oc[s_o_c_map[i].tok] = s_o_c_map[i].str;

  break_stack = calloc((break_alloced = OC_ALLOC), sizeof(int));
  state_stack = calloc((state_alloced = OC_ALLOC), sizeof(int));
}

void
gdl_balance_term(void)
{
  free(break_stack);
  free(state_stack);
  break_stack = state_stack = NULL;
  break_top = state_top = -1;
}

static void
gdl_break_extend(void)
{
  int t;
  break_alloced *= 2;
  break_stack = realloc(break_stack, break_alloced * sizeof(int));
  for (t = break_top; t < break_alloced; ++t)
    break_stack[t] = 0;
}

static int
gdl_break_peek(void)
{
  if (break_top >= 0)
    return break_stack[break_top];
  else
    return -1;
}

static int
gdl_break_pop(void)
{
  if (break_top >= 0)
    return break_stack[break_top--];
  else
    return -1;
}

static void
gdl_break_push(int tok)
{
  if (++break_top == break_alloced)
    gdl_break_extend();
  break_stack[break_top] = tok;
}

/* return 0 on OK; 1 on error */
int
gdl_balance_break(Mloc mlp, int tok)
{
  int ret = 0;
  /* if it's a closer, check the stack for a match */
  if (o_of_c[tok])
    {
      int p = gdl_break_peek();
      if (-1 == p)
	{
	  /* nothing on the stack, superfluous closer */
	  mesg_verr(&mlp, "unopened closer '%s'", s_of_oc[tok]);
	  ret = 1;
	}
      else if (p != o_of_c[tok])
	{
	  /* mismatched opener/closer */
	  mesg_verr(&mlp, "mismatched brackets: found closer '%s' but expected '%s'",
		    s_of_oc[tok], s_of_oc[c_of_o[p]]);
	  ret = 1;
	}
      else
	(void)gdl_break_pop();
    }
  else
    {
      /* for openers push the new opener on the stack */
      gdl_break_push(tok);
    }
  return ret;
}

static void
gdl_state_extend(void)
{
  int t;
  state_alloced *= 2;
  state_stack = realloc(state_stack, state_alloced * sizeof(int));
  for (t = state_top; t < state_alloced; ++t)
    state_stack[t] = 0;
}

static int
gdl_state_peek(void)
{
  if (state_top >= 0)
    return state_stack[state_top];
  else
    return -1;
}

static int
gdl_state_pop(void)
{
  if (state_top >= 0)
    return state_stack[state_top--];
  else
    return -1;
}

static void
gdl_state_push(int tok)
{
  if (++state_top == state_alloced)
    gdl_state_extend();
  state_stack[state_top] = tok;
}

/* return 0 on OK; 1 on error */
int
gdl_balance_state(Mloc mlp, int tok)
{
  int ret = 0;
  /* if it's a closer, check the stack for a match */
  if (o_of_c[tok])
    {
      int p = gdl_state_peek();
      if (-1 == p)
	{
	  /* nothing on the stack, superfluous closer */
	  mesg_verr(&mlp, "unopened closer '%s'", s_of_oc[tok]);
	  ret = 1;
	}
      else if (p != o_of_c[tok])
	{
	  /* mismatched opener/closer */
	  mesg_verr(&mlp, "mismatched brackets: found closer '%s' but expected '%s'",
		    s_of_oc[tok], s_of_oc[c_of_o[p]]);
	  ret = 1;
	}
      else
	(void)gdl_state_pop();
    }
  else
    {
      /* for openers push the new opener on the stack */
      gdl_state_push(tok);
    }
  return ret;
}

void
gdl_balance_flush(Mloc mlp)
{
  int tok;
  while ((tok = gdl_break_pop()) != -1)
    mesg_verr(&mlp, "unclosed opener '%s' [tok=%d]", s_of_oc[tok], tok);
  while ((tok = gdl_state_pop()) != -1)
    mesg_verr(&mlp, "unclosed opener '%s' [tok=%d]", s_of_oc[tok], tok);
  break_top = state_top = -1;
}
