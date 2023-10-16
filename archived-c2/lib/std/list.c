/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: list.c,v 0.4 1997/09/08 14:50:05 sjt Exp $
*/

#include <list.h>

const char *list_vec_sep_str = "\t\n ";

unsigned char *
list_concat(List *l)
{
  unsigned char *s = NULL;
  int len = 0;
  unsigned const char *n;
  for (n = list_first(l); n; n = list_next(l))
    len += strlen((const char *)n);
  s = malloc(len+1);
  *s = '\0';
  for (n = list_first(l); n; n = list_next(l))
    {
      strcat((char*)s,(char*)n);
    }
  return s;
}

unsigned char *
list_join(List *l, const char *j)
{
  unsigned char *s = NULL;
  int len = 0;
  unsigned const char *n;
  for (n = list_first(l); n; n = list_next(l))
    len += (strlen((const char *)n) + strlen(j));
  s = malloc(len+1);
  *s = '\0';
  for (n = list_first(l); n; n = list_next(l))
    {
      strcat((char*)s,(char*)n);
      if (list_more(l))
	strcat((char*)s, j);
    }
  
  return s;
}

void
list_add (List *lp, void *data)
{
  List_node *tmp = malloc (sizeof (List_node));
  if (!tmp)
    {
      fputs("out of core\n",stderr);
      exit(2);
    }
  tmp->data = data;
  if (NULL == lp)
    abort();
  if (NULL == lp->first)
    {
      lp->first = tmp;
      lp->last = tmp;
      if (lp->type == LIST_CIRCULAR)
        tmp->next = tmp->prev = tmp;
      else
        tmp->next = tmp->prev = NULL;
    }
  else
    {
      tmp->prev = lp->last;
      if (lp->type == LIST_CIRCULAR)
        tmp->next = lp->first;
      else
	tmp->next = NULL;
      lp->last->next = tmp;
      lp->last = tmp;
      if (lp->type == LIST_CIRCULAR)
        lp->first->prev = lp->last;
    }
  ++lp->count;
}

List *
list_append(List *lp1,List*lp2)
{
  lp1->count += lp2->count;
  lp1->last->next = lp2->first;
  free(lp2);
  return lp1;
}

List *
list_reset(List *lp)
{
  lp->count = 0;
  lp->first = lp->last = NULL;
  return lp;
}

List *
list_create (List_types_e type)
{
  List *tmp = calloc (sizeof(List), 1);
  if (!tmp)
    {
      fputs("out of core\n",stderr);
      exit(2);
    }
  tmp->type = type;
  tmp->first = tmp->last = NULL;
  tmp->count = 0;
  return tmp;
}

void
list_delete (List *lp, List_node * to_delete, void (*del)(void *))
{
  int keep_rover = (lp->rover && lp->rover == to_delete);

  if (lp == NULL || to_delete == NULL)
    return;

  if (NULL != del)
    del (to_delete->data);

  if (keep_rover)
    lp->rover = to_delete->prev;

  /* if lp->count is > 1, to_delete cannot == lp->first and lp->last */
  if (lp->count > 1 && lp->first == to_delete)
    lp->first = to_delete->next;
  else if (lp->count > 1 && lp->last == to_delete)
    lp->last = to_delete->prev;

  if (to_delete->prev != NULL)
    to_delete->prev->next = to_delete->next;

  if (to_delete->next != NULL)
    to_delete->next->prev = to_delete->prev;

  free (to_delete);
  --lp->count;

  if (0 == lp->count)
    {
      lp->rover = lp->first = lp->last = NULL;
    }
  else
    if (keep_rover && !lp->rover)
      lp->rover = lp->first;
}

void *
list_detach (List *lp, List_node * to_delete)
{
  void *data;

  if (lp == NULL || to_delete == NULL)
    return NULL;

  data = to_delete->data;

  list_delete (lp, to_delete, NULL);
  
  return data;  
}

void
list_exec (List *lp, void (*fnc)(void*))
{
  size_t i;
  List_node *lrover;
  if (lp == NULL)
    return;
  if (fnc == NULL)
    abort();
  for (i = 0, lrover = lp->first; i < lp->count; ++i, lrover = lrover->next)
    fnc (lrover->data);
}

void *
list_find (List *lp, const void *eltp, int(*cmp)(const void*lelt, const void*celt))
{
  void *vp;

  if (lp && list_len(lp))
    {
      for (vp = list_first (lp); vp != NULL; vp = list_next (lp))
	if (!cmp (vp, eltp))
	  return vp;
    }

  return NULL;
}

void
list_free (List *lp, void (*del)(void *))
{
  size_t i;
  List_node *lrover, *tmp;
  if (lp == NULL)
    return;
  if (lp->first != NULL)
    {
      if (del != NULL)
        list_exec (lp, del);
      for (i = 0, lrover = lp->first; i < lp->count; ++i)
        {
          tmp = lrover;
          lrover = lrover->next;
          free (tmp);
        }
    }
  free (lp);
}

void
list_xfree (void *vp)
{
  free (vp);
}

#if 0
/**loop over the list merging the two shortest each time. Add the
 * new array to the list and delete the two you just merged. Return
 * only the data that remains
 */
void *
list_reduce (Boolean invert, List * lp, int (*cmp)(const void*, const void*), void (*del)(void *))
{
  List_node *base, *short1, *short2, *end;
  void *ret;
  size_t ret_size;
  Array *ap;

  if (NULL == lp)
    return NULL;

  /* turn the list into a circular one if it isn't already */
  if (lp->type != LIST_CIRCULAR)
    {
      lp->last->next = lp->first;
      lp->first->prev = lp->last;
      lp->type = LIST_CIRCULAR;
    }
  base = lp->first;
  short1 = base;
 
  while (base != lp->last)
    {
      /* trap boundary case of list two elements long */
      if (base->next->next == base)
	short2 = base->next;
      else
	{
          for (end = base->next; end != base; end = end->next)
            if (((Array*)(short1->data))->in_use > ((Array*)(end->data))->in_use)
	      short1 = end;
          short2 = short1->next;
          for (end = short2->next; end != short1; end = end->next)
            if (((Array*)(short2->data))->in_use > ((Array*)(end->data))->in_use)
	       short2 = end;
	}
      if (short1 == short2)
        fatal ();

      ret = array_merge (invert, 
			 dra_data ((Array*)(short1->data)),
			 dra_data ((Array*)(short2->data)),
			 dra_elt_size ((Array*)(short1->data)),
			 dra_count ((Array*)(short1->data)),
			 dra_count ((Array*)(short2->data)),
			 &ret_size,
			 cmp);
      ap = dra_attach (ret, dra_elt_size((Array*)(base->data)), ret_size);
      list_add (lp, ap);
      list_delete (lp, short1, del);
      list_delete (lp, short2, del);
      short1 = base = lp->first;
    }
  ret = list_detach (lp, lp->first);
  list_free (lp, NULL);
  return ret;
}
#endif

void *
list_first (List *lp)
{
  if (lp == NULL || lp->first == NULL || lp->count == 0)
    return NULL;
  else
    {
      lp->rover = lp->first;
      return lp->rover->data;
    }
}

void*
list_next (List *lp)
{
  if (lp->rover == lp->last)
    return NULL;
  else if (lp->rover == NULL) /* should make it safe to delete lp->first while iterating */
    {
      lp->rover = lp->first;
      return lp->rover->data;
    }
  else
    {
      lp->rover = lp->rover->next;
      return lp->rover->data;
    }
}

void *
list_pop (List *lp)
{
  void *data;
  List_node *tmp;
  if (NULL == lp || NULL == lp->first)
    return NULL;
  else if (1 == lp->count)
    {
      data = lp->first->data;
      tmp = lp->first;
    }
  else if (lp->type == LIST_FIFO)
    {
      data = lp->first->data;
      tmp = lp->first;
      lp->first = lp->first->next;
      if (NULL != lp->first)
	lp->first->prev = NULL;
    }
  else
    {
      data = lp->last->data;
      tmp = lp->last;
      lp->last = lp->last->prev;
      if (NULL != lp->last)
	lp->last->next = NULL;
    }
  free (tmp);

  --lp->count;
  if (0 == lp->count)
    lp->first = lp->last = NULL;

  return data;
}

List *
list_from_str (char *str, char *(*tok)(char *), List_types_e type)
{
  List *tmp = list_create (type);
  char *s;

  while (1)
    {
      s = (NULL == tok) ? strtok (str, list_vec_sep_str) : tok (str);
      if (NULL == s)
	break;
      list_add(tmp, strdup(s));
      if (list_len (tmp) == 1)
        str = NULL;
    }

  return tmp;
}

unsigned char *
list_to_str(List *l)
{
  return list_to_str2(l, " ");
}

unsigned char *
list_to_str2(List *l, const char *sep)
{
  unsigned char *s = NULL;
  int len = 0;
  unsigned const char *n;
  for (n = list_first(l); n; n = list_next(l))
    len += strlen((const char *)n);
  len += (list_len(l) * strlen(sep));
  s = malloc(len+1);
  *s = '\0';
  for (n = list_first(l); n; n = list_next(l))
    sprintf((char*)(s+strlen((const char *)s)),"%s%s",n,sep);
  s[len-strlen(sep)] = '\0';
  return s;
}

const void **
list2array(List *l)
{
  const void **c = malloc((1+list_len(l))*sizeof(void*));
  int cindex = 0;
  List_node *lnp;

  if (!l || !list_len(l))
    return NULL;

  for (lnp = l->first; lnp; lnp = lnp->next)
    c[cindex++] = lnp->data;
  c[cindex] = NULL;
  return c;
}

const char **
list2chars(List *l)
{
  char **c = malloc((1+list_len(l))*sizeof(char*));
  int cindex = 0;
  List_node *lnp;

  if (!l || !list_len(l))
    return NULL;

  for (lnp = l->first; lnp; lnp = lnp->next)
    c[cindex++] = (char*)lnp->data;
  c[cindex] = NULL;
  return (const char **)c;
}
