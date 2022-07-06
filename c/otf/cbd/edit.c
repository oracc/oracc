#include "gx.h"

int
edit_add(unsigned char **ll, struct entry *e)
{
  unsigned char *ea = ll[0];
  char ctxt = '\0';
  struct edit *ed = NULL;

  /* set the ctxt */
  if (!strncmp((ccp)&ll[0][1], "@entry", strlen("@entry")))
    ctxt = 'e';
  else if (!strncmp((ccp)&ll[0][1], "@sense", strlen("@sense")))
    ctxt = 's';
  else if (!strncmp((ccp)ll[-1], "@entry", strlen("@entry")))
    ctxt = 'e';
  else if (!strncmp((ccp)ll[-1], "@sense", strlen("@sense")))
    ctxt = 's';
  else
    {
      warning("edit_add unable to set ctxt");
      return -1;
    }

  ed = calloc(1, sizeof(struct edit));
  
  /* set the type */
  if (*ea == '+')
    {
      if (ctxt == 'e')
	{
	  ed->type = ADD_E;
	  ed->owner = e;
	}
      else if (ctxt == 's')
	{
	  ed->type = ADD_S;
	  ed->owner = list_last(e->senses);
	}
      else
	{
	  warning("edit_add + in unknown ctxt");
	  return -1;
	}
    }
  else if (*ea == '>')
    {      
      if (ea[1] == '>')
	{
	  if (ctxt == 'e')
	    {
	      ed->type = MRG_E;
	      ed->owner = e;
	    }
	  else
	    {
	      ed->type = MRG_S;
	      ed->owner = list_last(e->senses);
	    }
	}
      else
	{
	  if (ctxt == 'e')
	    {
	      ed->type = REN_E;
	      ed->owner = e;
	    }
	  else
	    {
	      ed->type = REN_S;
	      ed->owner = list_last(e->senses);
	    }
	}
	
    }
  else if (*ea == '-')
    {
      if (ctxt == 'e')
	{
	  ed->type = DEL_E;
	  ed->owner = e;
	}
      else
	{
	  ed->type = DEL_S;
	  ed->owner = list_last(e->senses);
	}
    }
  else
    {
      fprintf(stderr, "edit_add passed unparseable data %s\n", (ccp)ea);
      return -1;
    }
	  
  /* warn or set */
  return 0;
}

int
edit_make_script()
{
  return 0;
}
