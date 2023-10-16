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
int mesg_no_loc = 0;
static const char *mesg_prefix_string = NULL;
static Pool *msgpool;
static List *mesg_list;
static int msg_cmp(const void *pa, const void *pb);
static char *nl(char *e);

/* Incremented for each warning message; notice and vnotice do not
   increment the mesg_status */
static int mesg__status = 0;

int
mesg_last_line(void)
{
  if (mesg_list && list_len(mesg_list))
    {
      const char *m = list_last(mesg_list);
      if ((m = strchr(m, ':')))
	return (int)strtol(++m, NULL, 10);
    }
  return 0;
}

int
mesg_status(void)
{
  return mesg__status;
}

void
mesg_status_ignore_one(void)
{
  --mesg__status;
}

void
mesg_status_reset(void)
{
  mesg__status = 0;
}

void
mesg_init(void)
{
  if (!mesg_list)
    mesg_list = list_create(LIST_DOUBLE);
  if (!msgpool)
    msgpool = pool_init();
  mloc_init();
  /*warning_mesg();*/
}

void
mesg_term(void)
{
  if (mesg_list)
    {
      list_free(mesg_list, NULL);
      mesg_list = NULL;
    }
  if (msgpool)
    {
      pool_term(msgpool);
      msgpool = NULL;
    }
  mloc_term();
}

int
mesg_remove_error(const char *file, int line, const char *str)
{
  List_node *lnp;
  for (lnp = mesg_list->last; lnp; lnp = lnp->prev)
    {
      const char *err = lnp->data;
      if (!strncmp(err, file, strlen(file)) && ':' == err[strlen(file)])
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
      else if (mesg_no_loc && strstr(err, str))
	{
	  list_delete(mesg_list, lnp, NULL);
	  return 1;
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
  ++mesg__status;
}

Mloc *
mesg_mloc(const char *file, size_t line)
{
  static Mloc m;
  m.file = file;
  m.line = line;
  return &m;
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
      if (!mesg_no_loc)
	sprintf(e, "%s: %s\n", loc, s);
      else
	sprintf(e, "%s\n", s);
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
  if (!mesg_no_loc)
    sprintf(e, "%s: ", loc);
  else
    *e = '\0';
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
mesg_notice(const char *file, int ln, const char *str)
{
  static Mloc l;
  l.file = (char*)file;
  l.line = ln;
  mesg_err(&l,(char*)str);
  mesg_status_ignore_one();
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
mesg_vnotice(const char *file, int ln, const char *s, ...)
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
      mesg_status_ignore_one();
    }
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

List *
mesg_retrieve(void)
{
  List *ret = mesg_list;
  mesg_list = NULL;
  return ret;
}

void
mesg_prefix(const char *p)
{
  mesg_prefix_string = p;
}

void
mesg_print(FILE *fp)
{
  mesg_print2(fp, mesg_list);
  mesg_list = NULL;
}

void
mesg_print2(FILE *fp, List *mlist)
{
  if (mlist && list_len(mlist))
    {
      if (0) /* unsorted messages */
	{
	  List_node *lp;
	  for (lp = mlist->first; lp; lp = lp->next)
	    {
	      fputs((char*)lp->data, fp);
	    }
	}
      else
	{
	  char **mp = NULL;
	  int i;
	  mp = (char**)list2array(mlist);
	  qsort(mp, list_len(mlist), sizeof(char*), msg_cmp);
	  for (i = 0; mp[i]; ++i)
	    {
	      if (mesg_prefix_string)
		fputs(mesg_prefix_string, fp);
	      fputs(mp[i], fp);
	    }
	}
      list_free(mlist, NULL);
      mlist = NULL;
    }
}

static int
msg_cmp(const void *pa, const void *pb)
{
  const char *af = *(const char **)pa, *bf = *(const char **)pb;
  const char *al = strchr(af,':');
  const char *bl = strchr(bf,':');
  if (al && bl)
    {
      int ret = 0;
      ++al;
      ++bl;
      ret = strncmp(af,bf,al-af);
      if (!ret)
	ret = atoi(al) - atoi(bl);
      return ret;
    }
  else if (al)
    return -1;
  else if (bl)
    return 1;
  else
    return 0;
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
  if (!mloc_mem)
    mloc_mem = memo_init(1024, sizeof(Mloc));
}

void
mloc_term(void)
{
  if (mloc_mem)
    {
      memo_term(mloc_mem);
      mloc_mem = NULL;
    }
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
