#include <stddef.h>
#include <gutil.h>
#include <signlist.h>
#include <sx.h>


int
g_index_of(const unsigned char *g, const unsigned char *b)
{
  int i = 0;
  if (!b)
    b = g_base_of(g);
  g += strlen((ccp)b);
  if (strlen((ccp)g) == 6)
    {
      i = ((g[2] - 0x80)) * 10;
      g += 3;
    }
  i += (g[2] - 0x80);
  if (i == 13) /* subscript x */
    i = 1000;
  return i;
}

/* No need to store split_value data in value because for
 * sl->values[i] the split_value is in sl->splitv[i]
 */
void
sx_homophones(struct sl_signlist *sl)
{
  int i;
  
  sl->homophones = hash_create(1024);
  sl->splitv = memo_new_array(sl->m_split_v, sl->nvalues);
  for (i = 0; i < sl->nvalues; ++i)
    {
      List *lp = NULL;
      sl->splitv[i].v = sl->values[i];
      if (sl->values[i]->xvalue)
	{
	  sl->splitv[i].b = sl->values[i]->name;
	  sl->splitv[i].i = 0; /* 1 = no index (i.e., index 1); 0 = ₓ-index */
	}
      else
	{
	  sl->splitv[i].b = g_base_of(sl->values[i]->name);
	  if (strlen((ccp)sl->splitv[i].b) == strlen((ccp)sl->values[i]->name))
	    sl->splitv[i].i = 1;
	  else
	    sl->splitv[i].i = g_index_of(sl->values[i]->name, sl->splitv[i].b);
	}

      if (!(lp = hash_find(sl->homophones, sl->splitv[i].b)))
	hash_add(sl->homophones, sl->splitv[i].b, (lp = list_create(LIST_SINGLE)));

      list_add(lp, &sl->splitv[i]);
    }
}
