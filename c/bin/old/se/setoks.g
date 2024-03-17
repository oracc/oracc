%{
#include <string.h>
#include "se.h"
%}
struct se_tok_tab
{
  const char *name;
  enum se_toks tok;
}
%%
and,	se_and
AND, 	se_and
or,	se_or
OR,	se_or
not,	se_not
NOT,	se_not
