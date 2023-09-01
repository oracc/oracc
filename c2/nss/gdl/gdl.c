#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <pool.h>
#include <prop.h>
#include <memo.h>
#include <tree.h>
#include <atf2utf.h>

#include "gdl.h"
#include "gdl.tab.h"

Pool *gdlpool;

/* These only get set when reading ATF but they are used in GDL
   messages so they are declared here which means that programs using
   GDL but not ATF don't have to pull in the whole ATF library */
char *curr_pqx = NULL;
int curr_pqx_line = 0;

/* set to 1 when a vq has !, e.g., a!(BA) */
int gdl_corrq = 0;

int gdl_initted = 0;
int gdltrace = 0;

/* If this is non-zero we use orig graphemes in reconstituting
   compounds rather than c10e */
int gdl_orig_mode = 0;

void
gdl_init(void)
{
  if (gdl_initted++)
    return;

  atf2utf_init();
  
  gdl_balance_init();

  gdl_lexfld_init();

  gdl_modq_init();
  
  gdlpool = pool_init();
}

void
gdl_term(void)
{
  if (gdl_initted)
    {
      pool_term(gdlpool);
      gdlpool = NULL;
      gdl_initted = 0;
      gdl_balance_term();
      gdl_modq_term();
    }
}


static Node *
gdl_first_s_sub(Node *np)
{
  if (np)
    {
      if (strlen(np->name)==3 && (np->name[2] == 'l' || np->name[2] == 'n' || np->name[2] == 's'))
	return np;
      else
	return gdl_first_s_sub(np->kids);
    }
  return NULL;
}

const unsigned char *
gdl_first_s(Node *gdl)
{
  Node *np = NULL;

  /* If caller used gdl_literal this is sufficient; if gdl->kids is
     NULL through user error this may not be correct but the error
     needs to be fixed anyway */
  if (!gdl->kids)
    return (uccp)gdl->text;
  
  np = gdl_first_s_sub(gdl);

  if (!np)
    {
      if (gdl->kids->kids)
	np = gdl_first_s_sub(gdl->kids->kids->next);
      if (!np)
	{
	  if (gdl->kids->kids && gdl->kids->kids->kids)
	    np = gdl_first_s_sub(gdl->kids->kids->kids->next);
	  else if (gdl->kids->kids && gdl->kids->kids->next)
	    np = gdl_first_s_sub(gdl->kids->kids->next->next);
	}
    }

  if (np && np->user)
    return ((gvl_g*)(np->user))->orig;  
  else
    return NULL;
}
