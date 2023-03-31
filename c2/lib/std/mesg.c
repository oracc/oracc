/*
 * This file contains mesg_xxx routines and mloc_xxx routines
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "list.h"
#include "memo.h"
#include "mesg.h"
#include "pool.h"

/************************************************
 ***
 *** 		Mesg routines
 ***
 ***********************************************/

const char *phase = NULL;

static Pool *msgpool;
static List *mesg_list;
static int msg_cmp(const void *pa, const void *pb);
static char *nl(char *e);

void
mesg_init(void)
{
  mesg_list = list_create(LIST_DOUBLE);
  msgpool = pool_init();
  mloc_init();
  /*warning_mesg();*/
}

void
mesg_term(void)
{
  list_free(mesg_list, NULL);
  pool_term(msgpool);
  mloc_term();
}

int
mesg_remove_error(const char *file, int line, const char *str)
{
  List_node *lnp;
  for (lnp = mesg_list->last; lnp; lnp = lnp->prev)
    {
      const char *err = lnp->data;
      if (!strncmp(err, file, strlen(file) && ':' == err[strlen(file)]))
	{
	  int errnum = atoi(err + strlen(file) + 1);
	  if (errnum == line && strstr(err, str))
	    {
	      list_delete(mesg_list, lnp, NULL);
	      return 1;
	    }
	  else if (errnum < line)
	    return 0;
	}
      else
	return 0;
    }
  return 0;
}

void
mesg_append(const char *a)
{
  char *e = (char*)mesg_list->last->data, *n;
  n = malloc(strlen(e)+strlen(a)+1);
  strcpy(n,e);
  strcat(n,a);
  mesg_list->last->data = pool_copy((const unsigned char *)n,msgpool);
  free(n);
}

void
mesg_add(char *e)
{
  list_add(mesg_list, e);
}

char *
mesg_loc(Mloc *locp)
{
  int need = 0;
  char *e = NULL;

  /* re-implement with_textid here as well? */
  
  if (phase)
    {
      const char *fmt = "%s:%d: (%s)";
      need = snprintf(NULL, 0, fmt, locp->file, locp->line, phase);
      e = malloc(need+1);
      sprintf(e, fmt, locp->file, locp->line, phase);
    }
  else
    {
      const char *fmt = "%s:%d";
      need = snprintf(NULL, 0, fmt, locp->file, locp->line);
      e = malloc(need+1);
      sprintf(e, fmt, locp->file, locp->line);
    }
  return e;
}

void
mesg_err(Mloc *locp, const char *s)
{
  if (s)
    {
      int need = 0;
      char *e = NULL, *loc;
      loc = mesg_loc(locp);
      need = snprintf(NULL, 0, "%s: %s\n", loc, s);
      e = malloc(need + 1 + 1); /* always allocate space for an extra \n */
      sprintf(e, "%s: %s\n", loc, s);
      mesg_add((char*)pool_copy((ucp)nl(e), msgpool));
      free(e);
      free(loc);
    }
}

void
mesg_averr(Mloc *locp, const char *s, va_list ap)
{
  char *loc, *e;
  int need;
  va_list ap2;
  va_copy(ap2, ap);

  loc = mesg_loc(locp);
  need = vsnprintf(NULL, 0, s, ap);
  need += strlen(loc) + 3 + 1; /* always allocate space for an extra \n */
  e = malloc(need);
  sprintf(e, "%s: ", loc);
  free(loc);
  vsprintf(e+strlen(e), s, ap2);
  va_end(ap2);
  mesg_add((char*)pool_copy((ucp)nl(e), msgpool));
  free(e);
}
  
void
mesg_verr(Mloc *locp, const char *s, ...)
{
  if (s)
    {
      va_list ap;
      va_start(ap, s);
      mesg_averr(locp, s, ap);
      va_end(ap);
    }
}

void
mesg_warning(const char *file, int ln, const char *str)
{
  static Mloc l;
  l.file = (char*)file;
  l.line = ln;
  mesg_err(&l,(char*)str);
}

void
mesg_vwarning(const char *file, int ln, const char *s, ...)
{
  static Mloc l;
  l.file = (char*)file;
  l.line = ln;
  if (s)
    {
      va_list ap;
      va_start(ap, s);
      mesg_averr(&l, s, ap);
      va_end(ap);
    }
}

void
mesg_avwarning(const char *file, int ln, const char *str, va_list ap)
{
  static Mloc l;
  l.file = (char*)file;
  l.line = ln;
  mesg_averr(&l,(char*)str,ap);
}

void
mesg_print(FILE *fp)
{
  if (mesg_list && list_len(mesg_list))
    {
      if (0) /* unsorted messages */
	{
	  List_node *lp;
	  for (lp = mesg_list->first; lp; lp = lp->next)
	    {
	      fputs((char*)lp->data, fp);
	    }
	}
      else
	{
	  char **mp = NULL;
	  int i;
	  mp = (char**)list2array(mesg_list);
	  qsort(mp, list_len(mesg_list), sizeof(char*), msg_cmp);
	  for (i = 0; mp[i]; ++i)
	    fputs(mp[i], fp);
	}
      list_free(mesg_list, NULL);
      mesg_list = NULL;
    }
}

static int
msg_cmp(const void *pa, const void *pb)
{
  const char *af = *(const char **)pa, *bf = *(const char **)pb;
  const char *al = strchr(af,':')+1;
  const char *bl = strchr(bf,':')+1;
  int ret = strncmp(af,bf,al-af);
  if (!ret)
    ret = atoi(al) - atoi(bl);
  return ret;
}

static char *
nl(char *e)
{
  if ('\n' != e[strlen(e)-1])
    strcat(e, "\n");
  return e;
}

/************************************************
 ***
 *** 		Mloc routines
 ***
 ***********************************************/

static Memo *mloc_mem = NULL;

void
mloc_init(void)
{
  mloc_mem = memo_init(1024, sizeof(Mloc));
}

void
mloc_term(void)
{
  memo_term(mloc_mem);
  mloc_mem = NULL;
}

Mloc *
mloc_file_line(const char *file, int line)
{
  Mloc *ml = memo_new(mloc_mem);
  ml->file = file;
  ml->line = line;
  return ml;
}

Mloc *
mloc_mloc(Mloc *arg_ml)
{
  Mloc *ml = memo_new(mloc_mem);
  *ml = *arg_ml;
  return ml;
}
