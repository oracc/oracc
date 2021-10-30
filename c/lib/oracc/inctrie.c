/* Incremental trie library.

   This library is designed for small, sparsely populated key sets;
   large key sets could use lots of memory. */

#include <stdlib.h>
#include <string.h>
#include "inctrie.h"

struct inctrie *
inctrie_create(void)
{
  return calloc(1,sizeof(struct inctrie));
}

struct inctrie *
inctrie_insert(struct inctrie *ip, const char *key, const void *val, short level)
{
  if (*key)
    {
      if (!ip->cells)
	if (!(ip->cells = calloc(128, sizeof(struct inctrie_cell *))))
	  return NULL;

      if (NULL == ip->cells[(int)*key])
	ip->cells[(int)*key] = inctrie_create();

      ip->cells[(int)*key]->level = ++level;

      if (key[1])
	{
	  struct inctrie *arg = ip->cells[(int)*key];
	  if (!inctrie_insert(arg, ++key, val, level))
	    return NULL;
	}
      else
	ip->cells[(int)*key]->val = val;
    }
  return ip;
}

const void *
inctrie_lookup(struct inctrie *ip, const char *key, int *length)
{
  while (*key)
    {
      if (*(unsigned char*)key < 0x80 && ip && ip->cells && ip->cells[(int)*key])
	ip = ip->cells[(int)*key++];
      else
	break;
    }
  if (ip)
    {
      *length = ip->level;
      return ip->val;
    }
  return NULL;
}

const unsigned char *
inctrie_map(struct inctrie *ip, const char *s, const char *end, int append,
	    void(*er)(const char*,const char*,void*,const char *,size_t),
	    void*user, const char *f, size_t l)
{
  static unsigned char *dest = NULL;
  static int dest_alloced = 0;
  int dest_used = 0;
  const char *s_orig = s;

  if (dest && append)
    dest_used = strlen((const char *)dest);
  
  if (!s)
    {
      if (dest)
	{
	  free(dest);
	  dest_alloced = 0;
	  dest = NULL;
	}
      return NULL;
    }

  if (!ip)
    return NULL;

  while (s < end && *s)
    {
      if (*(unsigned char *)s < 0x80)
	{
	  int len;
	  const unsigned char *ret = inctrie_lookup(ip,s,&len);
	  if (ret)
	    {
	      int retlen = strlen((const char *)ret);
	      if (dest_alloced <= (dest_used+retlen+1))
		{
		  while (dest_alloced <= (dest_used+retlen+1))
		    dest_alloced += 128;
		  dest = realloc(dest, dest_alloced * sizeof(unsigned char));
		}
	      memcpy(dest+dest_used,ret,retlen);
	      dest_used += retlen;
	      s += len;
	    }
	  else
	    {
	      if (er)
		er(s_orig,s,user,f,l);
	      if (dest_alloced <= (dest_used+1))
		{
		  while (dest_alloced <= (dest_used+2))
		    dest_alloced += 128;
		  dest = realloc(dest, dest_alloced * sizeof(unsigned char));
		}
	      dest[dest_used++] = *s++;
	    }
	}
      else
	{
	  if (dest_alloced <= (dest_used+1))
	    {
	      while (dest_alloced <= (dest_used+2))
		dest_alloced += 128;
	      dest = realloc(dest, dest_alloced * sizeof(unsigned char));
	    }
	  dest[dest_used++] = *s++;
	}
    }
  if (dest)
    dest[dest_used] = '\0';
  return dest;
}

void
ifree_sub(struct inctrie *ip)
{
  int i;
  if (ip->cells)
    {
      for (i = 0; i < 128; ++i)
	{
	  if (ip->cells[i])
	    ifree_sub(ip->cells[i]);
	  free(ip->cells[i]);
	}
      free(ip->cells);
    }
}

void
inctrie_free(struct inctrie *ip)
{
  if (ip)
    {
      ifree_sub(ip);
      inctrie_map(NULL,NULL,NULL,0,NULL,NULL,NULL,0);
      free(ip);
    }
}

#ifdef TEST
#include <stdio.h>
int
main(int argc, char **argv)
{
  struct inctrie *t = inctrie_create();
  int len;
  const char *rep = NULL;
  unsigned char *mapstr = "aszabaszab";

  inctrie_insert(t,"a","z", 0);
  inctrie_insert(t,"sz","sh", 0);
  rep = inctrie_lookup(t,"b",&len);
  if (rep)
    fprintf(stderr,"lookup of b succeeded when it shouldn't\n");
  rep = inctrie_lookup(t,"a",&len);
  if (rep)
    fprintf(stderr,"lookup of a succeeded with length %d and val %s\n",len,rep);
  rep = inctrie_lookup(t,"sza",&len);
  if (rep)
    fprintf(stderr,"lookup of sz succeeded with length %d and val %s\n",len,rep);
  rep = (const char *)inctrie_map(t,mapstr);
  fprintf(stderr, "mapped %s to %s\n", mapstr, rep);
}
#endif
