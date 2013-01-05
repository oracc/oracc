/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: addgraph.c,v 0.4 1997/09/08 14:50:13 sjt Exp $
*/

#define inline
#undef xfwrite
#define xfwrite(_fn,_t,_buf,_siz,_cnt,_fp) \
			if (fwrite(_buf,_siz,_cnt,_fp)!=_cnt)\
			  error(NULL,"write failed on %s (disk full?)",_fn)

extern unsigned long _nkeys;

static inline Dbi_tnode *trie_find (unsigned char *key);
static inline void trie_add (Dbi_tnode *dp);
static inline void addgraph (Dbi_index *dp, unsigned char *key, void *data);
static void trie_init (void);
static void trie_term (void);

static unsigned char trie_chars[] = "abdeghjiklmnpqrsfctvuwz0123456789";
static int ntrie_chars = sizeof (trie_chars) - 1;
static char trie_map[256];
static char *trie_1, *trie_2, *trie_3;
static Dbi_tnode **t1_ptrs, **t2_ptrs, **t3_ptrs;

Dbi_tnode *last_find;

#define  dsize	(dp->h.data_size)
  
/*******************************************************************/
static void
trie_init ()
{
  unsigned char *t_map_rover;
  
  trie_1 = xcalloc (ntrie_chars, 1);
  trie_2 = xcalloc (ntrie_chars * ntrie_chars, 1);
  trie_3 = xcalloc (ntrie_chars * ntrie_chars * ntrie_chars, 1);
  t1_ptrs = xcalloc (ntrie_chars, sizeof (Dbi_tnode*));
  t2_ptrs = xcalloc (ntrie_chars * ntrie_chars, sizeof (Dbi_tnode*));
  t3_ptrs = xcalloc (ntrie_chars * ntrie_chars * ntrie_chars, sizeof (Dbi_tnode*));
  memset (trie_map, -1, 256);
  for (t_map_rover = trie_chars; *t_map_rover; ++t_map_rover)
    trie_map[*t_map_rover] = t_map_rover - trie_chars;
}

static void
trie_term ()
{
  free (trie_1);
  free (trie_2);
  free (trie_3);
  free (t1_ptrs);
  free (t2_ptrs);
  free (t3_ptrs);
}

/* we can assume that 'key' is 1, 2 or 3 characters in length */
static inline Dbi_tnode *
trie_find (unsigned char *key)
{
  int k1, k2, k3, k1n, k2n;

  if ((k1 = trie_map[key[0]]) >= 0 && trie_1[k1])
    {
      if ('\0' == key[1])
	return t1_ptrs[k1];
      k1n = k1*ntrie_chars;
      if ((k2 = trie_map[key[1]]) >= 0 && trie_2[k1n + k2])
	{
	  if ('\0' == key[2])
	    return t2_ptrs[k1n + k2];
	  k1n = k1*(ntrie_chars*ntrie_chars);
	  k2n = k2*ntrie_chars;
	  if ((k3 = trie_map[key[2]]) >= 0 && trie_3[k1n+k2n+k3])
	    return t3_ptrs[k1n+k2n+k3];
	}
    }
  return NULL;
}

/* we can assume that 'key' is 1, 2 or 3 characters in length */
static inline void
trie_add (Dbi_tnode *dp)
{
  int k1, k2, k3, k1n, k2n;
  if ((k1 = trie_map[dp->key[0]]) >= 0)
    {
      trie_1[k1] = 1;
      if (dp->key[1])
	{
	  k1n = k1*ntrie_chars;
	  if ((k2 = trie_map[dp->key[1]]) >= 0)
	    {
	      trie_2[k1n+k2] = 1;
	      if (dp->key[2])
		{
		  if ((k3 = trie_map[dp->key[2]]) >= 0)
		    {
		      k1n = k1*(ntrie_chars*ntrie_chars);
		      k2n = k2*ntrie_chars;
		      trie_3[k1n+k2n+k3] = 1;
		      t3_ptrs[k1n+k2n+k3] = dp;
		    }
		}
	      else
		t2_ptrs[k1n+k2] = dp;
	    }
	}
      else
	{
	  t1_ptrs[k1] = dp;
	}
    }
}

/**************************************************************/

static inline void
addgraph (Dbi_index *dp, unsigned char *key, void *data)
{
  Boolean found;
  Hash_element *q, **p;

  /* find match or insertion point */
  if (key[1] == '\0' || key[2] == '\0' || key[3] == '\0')
    {
      last_find = trie_find((unsigned char *)key);
      /* FIX HERE TO LOOK IN HASH TABLE IF NOT FOUND AND ANY CHAR IS NON TRIE CHAR */
    }
  else
    last_find = NULL;
  
  if (NULL == last_find)
    {
      q = _hash_lookup(dp->keys, (unsigned char *)key, &p);
      if (NULL == q)
	{
	  *p = xmalloc (sizeof (Hash_element));
	  (*p)->data = xmalloc (sizeof (Dbi_tnode));
	  (*p)->next = NULL;
	  last_find = (*p)->data;
	  (*p)->key = last_find->key = (unsigned char *)xstrdup((const char*)key);
	  last_find->kid = (size_t)dp->h.entry_count++;
	  last_find->dcount = 0;
	  last_find->kcount = 0;
	  if (dp->cache_size > 0)
	    {
	      last_find->cache = xmalloc (dp->cache_size * dsize);
	      last_find->cache_count = 0;
	    }
	  else
	    {
	      last_find->cache = NULL;
	      last_find->cache_count = 0;
	    }
	  found = FALSE;
	  dp->keys->key_count++;
	  HASH_EXPAND_TABLE(dp->keys);
	  if (last_find->key[1] == '\0'
	      || last_find->key[2] == '\0'
	      || last_find->key[3] == '\0')
	    trie_add (last_find);
	}
      else
	{
	  found = TRUE;
	  last_find = q->data;
	}
    }
  else
    found = TRUE;

  if (dp->cache_size)
    {
      if (last_find->cache_count + 1 == dp->cache_size)
	{
	  xfwrite (dp->_tmp_fn, TRUE, &last_find->kid, sizeof (Unsigned32), 
		   1, dp->_tmp_fp);
	  xfwrite (dp->_tmp_fn, TRUE, &last_find->cache_count, sizeof (Unsigned32), 
		   1, dp->_tmp_fp);
	  xfwrite (dp->_tmp_fn, TRUE, (char*)last_find->cache, dsize,
		   last_find->cache_count, dp->_tmp_fp);
	  ++last_find->kcount;
	  last_find->dcount += last_find->cache_count;
	  last_find->cache_count = 0;
	}
      memcpy (&((char*)last_find->cache)[last_find->cache_count*dsize], 
	      data, dsize);
      ++last_find->cache_count;
    }
  else
    {
      static size_t count = 1;
      xfwrite (dp->_tmp_fn, TRUE, &last_find->kid, sizeof (size_t), 
	       1, dp->_tmp_fp);
      xfwrite (dp->_tmp_fn, TRUE, &count, sizeof (size_t), 1, dp->_tmp_fp);
      xfwrite (dp->_tmp_fn, TRUE, data, dsize, 1, dp->_tmp_fp);
      ++last_find->kcount;
      ++last_find->dcount;
    }
}
