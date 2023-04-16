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
    -1
  };

static struct s_o_c { int tok; const char *str; } s_o_c_map[] =
  {
    { '<', "<" },
    { '>', ">" },
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

int *o_c_stack = NULL;
static int o_c_alloced = 0;
static int o_c_top = -1;
#define OC_ALLOC	4

void
gdl_balance_init(void)
{
  int i;

  if (o_c_stack)
    {
      o_c_top = -1;
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

  o_c_stack = calloc((o_c_alloced = OC_ALLOC), sizeof(int));
}

void
gdl_balance_term(void)
{
  free(o_c_stack);
  o_c_stack = NULL;
  o_c_top = -1;
}

static void
gdl_balance_extend(void)
{
  int t;
  o_c_alloced *= 2;
  o_c_stack = realloc(o_c_stack, o_c_alloced * sizeof(int));
  for (t = o_c_top; t < o_c_alloced; ++t)
    o_c_stack[t] = 0;
}

static int
gdl_balance_peek(void)
{
  if (o_c_top >= 0)
    return o_c_stack[o_c_top];
  else
    return -1;
}

static int
gdl_balance_pop(void)
{
  if (o_c_top >= 0)
    return o_c_stack[o_c_top--];
  else
    return -1;
}

static void
gdl_balance_push(int tok)
{
  if (++o_c_top == o_c_alloced)
    gdl_balance_extend();
  o_c_stack[o_c_top] = tok;
}

/* return 0 on OK; 1 on error */
int
gdl_balance(Mloc mlp, int tok)
{
  int ret = 0;
  /* if it's a closer, check the stack for a match */
  if (o_of_c[tok])
    {
      int p = gdl_balance_peek();
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
	(void)gdl_balance_pop();
    }
  else
    {
      /* for openers push the new opener on the stack */
      gdl_balance_push(tok);
    }
  return ret;
}

void
gdl_balance_flush(Mloc mlp)
{
  int tok;
  while ((tok = gdl_balance_pop()) != -1)
    mesg_verr(&mlp, "unclosed opener '%s'", s_of_oc[tok]);
  o_c_top = -1;
}
