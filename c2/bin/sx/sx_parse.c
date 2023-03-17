#include <cat.h>
#include "sx.h"

void
sx_p_sign(Node *np, char *data)
{
  np->parsed = data;
}

void
sx_p_form(Node *np, char *data)
{
  np->parsed = data;
}

void
sx_p_v(Node *np, char *data)
{
  np->parsed = data;
}
