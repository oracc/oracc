#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "list.h"
#include "npool.h"
#include "msglist.h"
#include "warning.h"

static struct npool *msgpool;
static List *msglist;
static int msg_cmp(const char **a, const char **b);

void
msglist_init(void)
{
  msglist = list_create(LIST_SINGLE);
  msgpool = npool_init();
  warning_msglist();
}

void
msglist_term(void)
{
  list_free(msglist, NULL);
  npool_term(msgpool);
}

static char *
nl(char *e)
{
  if ('\n' != e[strlen(e)-1])
    strcat(e, "\n");
  return e;
}

void
msglist_append(char *a)
{
  char *e = (char*)msglist->last->data, *n;
  n = malloc(strlen(e)+strlen(a)+1);
  strcpy(n,e);
  strcat(n,a);
  msglist->last->data = npool_copy((const unsigned char *)n,msgpool);
  free(n);
}

void
msglist_add(char *e)
{
  list_add(msglist, e);
}

char *
msglist_loc(msgloc *locp)
{
  int need = 0;
  char *e = NULL;

  /* re-implement with_textid here as well? */
  
  if (phase)
    {
      const char *fmt = "%s:%d: (%s)";
      need = snprintf(NULL, 0, fmt, locp->file, locp->first_line, phase);
      e = malloc(need+1);
      sprintf(e, fmt, locp->file, locp->first_line, phase);
    }
  else
    {
      const char *fmt = "%s:%d";
      need = snprintf(NULL, 0, fmt, locp->file, locp->first_line);
      e = malloc(need+1);
      sprintf(e, fmt, locp->file, locp->first_line);
    }
  return e;
}

void
msglist_err(msgloc *locp, char *s)
{
  if (s)
    {
      int need = 0;
      char *e = NULL, *loc;
      loc = msglist_loc(locp);
      need = snprintf(NULL, 0, "%s: %s\n", loc, s);
      e = malloc(need + 1 + 1); /* always allocate space for an extra \n */
      sprintf(e, "%s: %s\n", loc, s);
      msglist_add((char*)npool_copy((ucp)nl(e), msgpool));
      free(e);
      free(loc);
    }
}

void
msglist_averr(msgloc *locp, char *s, va_list ap)
{
  char *loc, *e;
  int need;
  va_list ap2;
  va_copy(ap2, ap);

  loc = msglist_loc(locp);
  need = vsnprintf(NULL, 0, s, ap);
  need += strlen(loc) + 3 + 1; /* always allocate space for an extra \n */
  e = malloc(need);
  sprintf(e, "%s: ", loc);
  free(loc);
  vsprintf(e+strlen(e), s, ap2);
  va_end(ap2);
  msglist_add((char*)npool_copy((ucp)nl(e), msgpool));
  free(e);
}
  
void
msglist_verr(msgloc *locp, char *s, ...)
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
msglist_warning(const char *file, int ln, const char *str)
{
  static msgloc l;
  l.file = (char*)file;
  l.first_line = ln;
  msglist_err(&l,(char*)str);
}

void
msglist_vwarning(const char *file, int ln, const char *str, va_list ap)
{
  static msgloc l;
  l.file = (char*)file;
  l.first_line = ln;
  msglist_averr(&l,(char*)str,ap);
}

void
msglist_print(FILE *fp)
{
  if (msglist && list_len(msglist))
    {
      if (0) /* unsorted messages */
	{
	  List_node *lp;
	  for (lp = msglist->first; lp; lp = lp->next)
	    {
	      fputs((char*)lp->data, fp);
	    }
	}
      else
	{
	  char **mp = NULL;
	  int i;
	  mp = (char**)list2array(msglist);
	  qsort(mp, list_len(msglist), sizeof(char*), (__compar_fn_t)msg_cmp);
	  for (i = 0; mp[i]; ++i)
	    fputs(mp[i], fp);
	}
      list_free(msglist, NULL);
      msglist = NULL;
    }
}

static int
msg_cmp(const char **a, const char **b)
{
  const char *af = *a, *bf = *b;
  const char *al = strchr(af,':')+1;
  const char *bl = strchr(bf,':')+1;
  int ret = strncmp(af,bf,al-af);
  if (!ret)
    ret = atoi(al) - atoi(bl);
  return ret;
}
