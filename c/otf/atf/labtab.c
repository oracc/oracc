#include <stdlib.h>
#include <string.h>
#include "cdf.h"
#include "labtab.h"

extern int saa_mode;

struct labtab *
create_labtab(void)
{
  struct labtab* lt = calloc(1, sizeof(struct labtab));
  lt->alloced = 32;
  lt->used = 0;
  lt->table = calloc(lt->alloced, sizeof(struct labtab_node));
  return lt;
}
void
destroy_labtab(struct labtab *lt)
{
  free(lt->table);
  free(lt);
}
void
register_label(struct labtab*lt,unsigned char *xid,unsigned char*l)
{
  struct labtab_node *ltp;
  if (lt->used == lt->alloced)
    {
      lt->alloced *= 2;
      lt->table = realloc(lt->table,lt->alloced*sizeof(struct labtab_node));
    }
  ltp = &lt->table[lt->used++];
  ltp->xmlid = xid;
  ltp->label = l;
}
int
find_label(struct labtab*lt,int start,unsigned char *l)
{
  int i;
  for (i = start; i < lt->used; ++i)
    if (!xstrcmp(lt->table[i].label,l))
      return i;
  if (saa_mode)
    {
      static unsigned char buf[128];
      if (*l >= '0' && *l <= '9')
	xstrcpy(buf,"o ");
      else
	*buf = '\0';
      xstrcat(buf,l);
      /* try with a 'o ' prefix */
      for (i = start; i < lt->used; ++i)
	if (!xstrcmp(lt->table[i].label,buf))
	  return i;
      /* try again with a ' suffix */
      xstrcat(buf,"'");
      for (i = start; i < lt->used; ++i)
	if (!xstrcmp(lt->table[i].label,buf))
	  return i;
    }
  return -1;
}
