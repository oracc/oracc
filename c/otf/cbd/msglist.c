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
msglist_loc(YLLTYPE *locp)
{
  int need = 0;
  char *e = NULL, *ret;
  need = snprintf(NULL, 0, "%s:%d", loc.file, loc.first_line);
  e = malloc(need+1);
  sprintf(e, "%s:%d: error: %s\n", efile, loc.first_line, s);
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
      need = snprintf(NULL, len, "%s: %s\n", loc, s);
      e = malloc(need + 1);
      sprintf(e, "%s: %s\n", loc, s);
      msglist_add(npool_copy((ucp)e, curr_cbd->pool));
      free(e);
      free(loc);
      ++parser_status;
    }
}

void
msglist_verr(YYLTYPE *locp, char *s, ...)
{
  if (s)
    {
      int need = 0;
      char *e = NULL, *loc;
      loc = msglist_loc(locp);
      va_start(ap, s);
      need = vsnprintf(NULL, len, s, ap);
      va_end(ap);
      need += strlen(loc) + 3;
      e = malloc(need);
      sprintf(e, "%s: ", loc);
      free(loc);
      va_start(ap, s);
      vsprintf(e+strlen(e), s, ap);
      va_end(ap);
      msglist_add(npool_copy((ucp)e, curr_cbd->pool));
      free(e);
      ++parser_status;
    }
}
