#include <stdio.h>
#include "gx.h"

static List *msglist;

void
msglist_init(void)
{
  msglist = list_create(LIST_SINGLE);
}

void
msglist_term(void)
{
  list_free(msglist, NULL);
}

void
msglist_add(char *e)
{
  list_add(msglist, e);
}

char *
msglist_loc(YYLTYPE *locp)
{
  int need = 0;
  char *e = NULL;
  need = snprintf(NULL, 0, "%s:%d", locp->file, locp->first_line);
  e = malloc(need+1);
  sprintf(e, "%s:%d\n", locp->file, locp->first_line);
  return e;
}

void
msglist_err(YYLTYPE *locp, char *s)
{
  if (s)
    {
      int need = 0;
      char *e = NULL, *loc;
      loc = msglist_loc(locp);
      need = snprintf(NULL, 0, "%s: %s\n", loc, s);
      e = malloc(need + 1);
      sprintf(e, "%s: %s\n", loc, s);
      msglist_add((char*)npool_copy((ucp)e, curr_cbd->pool));
      free(e);
      free(loc);
    }
}

void
msglist_averr(YYLTYPE *locp, char *s, va_list ap)
{
  char *loc, *e;
  int need;
  va_list ap2;
  va_copy(ap2, ap);
  
  loc = msglist_loc(locp);
  need = vsnprintf(NULL, 0, s, ap);
  need += strlen(loc) + 3;
  e = malloc(need);
  sprintf(e, "%s: ", loc);
  free(loc);
  vsprintf(e+strlen(e), s, ap2);
  va_end(ap2);
  msglist_add((char*)npool_copy((ucp)e, curr_cbd->pool));
  free(e);
    
}
  
void
msglist_verr(YYLTYPE *locp, char *s, ...)
{
  if (s)
    {
      va_list ap;
      va_start(ap, s);
      msglist_averr(locp, s, ap);
      va_end(ap);
    }
}

void
msglist_print(FILE *fp)
{
  if (msglist && list_len(msglist))
    {
      if (1) /* unsorted messages */
	{
	  List_node *lp;
	  for (lp = list_first(msglist); lp; lp = lp->next)
	    {
	      fputs((char*)lp->data, fp);
	    }
	}
      else
	{
	  /*char **mp = NULL;*/
	}
    }
}
