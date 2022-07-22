/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: hash.c,v 0.3 1997/09/08 14:50:04 sjt Exp $
*/

#include <psd_base.h>
#include <stdarg.h>
#include <hash.h>

static int non_null_address = 1;

Hash_table *
hash_from_var_char_star(const char *required, ...)
{
  if (NULL != required)
    {
      Hash_table *h = hash_create(1);
      va_list va;
      const char *more;
      va_start(va, required);
      hash_add(h, (unsigned char *)required, &non_null_address);
      while ((more = va_arg(va, const char *)))
	hash_add(h, (unsigned char *)more, &non_null_address);
      va_end(va);
      return h;
    }
  return NULL;
}

void
hash_add (Hash_table *htab, const unsigned char *key, void *data)
{
  Hash_element *q, **p = NULL;

  q = _hash_lookup (htab, key, &p);
  if (q != NULL)
    {
#if HASH_DEBUG
      fprintf (hash_debug_fp, "[hash_add] Table %lx, %s found at %lx\n", htab, key, q);
#endif
      if (htab->freeable_data)
	free (q->data);
      q->data = data;
      return;
    }
#if HASH_DEBUG
  fprintf (hash_debug_fp, "[hash_add] Table %lx, adding %s at %lx\n", htab, key, *p);
#endif
  *p = malloc (sizeof (Hash_element));
  (*p)->key = key;
  (*p)->data = data;
  (*p)->next = NULL;

  if (++htab->key_count / MUL (htab->segment_count, SEGMENT_SIZE) > htab->max_load_factor)
    _hash_expand (htab);
}

#if HASH_STATISTICS
long HashAccesses, HashCollisions;
#endif

#if HASH_DEBUG
FILE *hash_debug_fp = NULL;
#endif
 
Hash_table *
hash_create (Unsigned32 count)
{
  Unsigned32 i;
  Hash_table *tmp;
 
  /* Adjust count to be nearest higher power of 2, minimum SEGMENT_SIZE, 
   * then convert into segments. */
  i = SEGMENT_SIZE;
  while (i < count)
    i <<= 1;
  count = DIV (i, SEGMENT_SIZE);
 
  tmp = malloc (sizeof (Hash_table));
  tmp->freeable_data = 0;
  for (tmp->segment_count = 0; tmp->segment_count < count; ++tmp->segment_count)
    {
      tmp->directory[tmp->segment_count] = malloc (sizeof (Hash_element*) * SEGMENT_SIZE);
      for (i = 0; i < SEGMENT_SIZE; ++i)
        tmp->directory[tmp->segment_count][i] = NULL;
    }

  /* set the unused directory entries to NULL */
  for (i = tmp->segment_count; i < DIRECTORY_SIZE; ++i)
    tmp->directory[i] = NULL;

  tmp->key_count = 0;
  tmp->p = 0;
  tmp->maxp = MUL (count, SEGMENT_SIZE);
  tmp->min_load_factor = 1;
  tmp->max_load_factor = DEFAULT_MAX_LOAD_FACTOR;

#if HASH_DEBUG
  if (NULL == hash_debug_fp)
    {
      hash_debug_fp = xfopen ("hash.dbg", "w");
      setvbuf (hash_debug_fp, NULL, _IONBF, 0);
    }
  fprintf (hash_debug_fp, "[hash_create] Table %lx Count %u maxp %d SegmentCount %d\n",
	    tmp, count, (int)tmp->maxp, (int)tmp->segment_count);
#endif
#if HASH_STATISTICS
  HashAccesses = HashCollisions = 0;
#endif

  return tmp;
}

void
hash_freeable_data(Hash_table *h,int i)
{
  h->freeable_data = i;
}

void *
hash_find (Hash_table *htab, const unsigned char *key)
{
  static Hash_element *ret = NULL;
  int ok = 0;
  ret = _hash_lookup(htab, key, NULL);
  ok = (ret != ((Hash_element*)0));
  if (!ok)
    return NULL;
  if (ok)
    return ret->data;
  return NULL;
}

void
hash_free (Hash_table *htab, void (*fnc)(void *))
{
  hash_free2(htab,NULL,fnc);
}

void
hash_free2 (Hash_table *htab, void (*keyfnc)(void *), void (*datfnc)(void *))
{
  Unsigned32 i, j;
  Hash_element *p, *q;
 
  if (htab != NULL)
    {
      for (i = 0; i < htab->segment_count; i++)
	{
	  if (htab->directory[i] != NULL)
	    {
	      for (j = 0; j < SEGMENT_SIZE; j++)
		{
		  p = htab->directory[i][j];
		  while (p != NULL)
		    {
		      if (NULL != keyfnc)
			keyfnc((char*)p->key);
		      if (NULL != datfnc)
			datfnc(p->data);
		      q = p->next;
		      free (p);
		      p = q;
		    }
		}
	      free (htab->directory[i]);
	    }
	}
      free (htab);
#if HASH_STATISTICS && HASH_DEBUG
      fprintf (stderr, "[hash_free] Accesses %ld Collisions %ld\n",
		HashAccesses, HashCollisions);
#endif
    }
#if HASH_DEBUG
  xfclose ("hash.dbg", hash_debug_fp);
  hash_debug_fp = NULL;
#endif
}

void
hash_xfree (void *vp)
{
  free (vp);
}

Hash_address_t
_hash_hash (Hash_table *htab, const unsigned char *key)
{
  Hash_address_t h, address;
  register const unsigned char *k = key;

  h = 0;
  /* Convert string to integer */
  while (*k)
    h = h * PRIME_1 ^ (unsigned long)(*k++ - ' ');
  h %= PRIME_2;
  address = MOD (h, htab->maxp);
  if (address < htab->p)
    address = MOD (h, (htab->maxp << 1));	/* h % (2*htab->maxp)	*/

#if HASH_DEBUG
  fprintf (hash_debug_fp, "[_hash_hash] Table %lx key %s hashes to %ld and address %ld\n",
    htab, key, h, address);
#endif

  return address;
}

void
_hash_expand (Hash_table *htab)
{
  Hash_address_t NewAddress;
  long int OldSegmentIndex, NewSegmentIndex;
  long int OldSegmentDir, NewSegmentDir;
  Hash_element **OldSegment, **NewSegment;
  Hash_element *Current, **Previous, **LastOfNew;
  Unsigned32 i;
 
  if (htab->maxp + htab->p < MUL (DIRECTORY_SIZE, SEGMENT_SIZE))
    {
#if HASH_DEBUG
      fprintf (hash_debug_fp, "[_hash_expand] Table %lx expanded\n", htab);
#endif
      /* Locate the bucket to be split */
      OldSegmentDir = DIV (htab->p, SEGMENT_SIZE);
      OldSegment = htab->directory[OldSegmentDir];
      OldSegmentIndex = MOD (htab->p, SEGMENT_SIZE);

      /* Expand address space; if necessary create a new segment */
      NewAddress = htab->maxp + htab->p;
      NewSegmentDir = DIV (NewAddress, SEGMENT_SIZE);
      NewSegmentIndex = MOD (NewAddress, SEGMENT_SIZE);
      if (NewSegmentIndex == 0)
	{
   	  htab->directory[NewSegmentDir] = malloc (sizeof (Hash_element *) * SEGMENT_SIZE);
	  for (i = 0; i < SEGMENT_SIZE; ++i)
	    htab->directory[NewSegmentDir][i] = NULL;
	  htab->segment_count++;
	}
      NewSegment = htab->directory[NewSegmentDir];

      /* Adjust state variables */
      htab->p++;
      if (htab->p == htab->maxp)
	{
	  htab->maxp <<= 1;	/* htab->maxp *= 2	*/
	  htab->p = 0;
	}

      /* Relocate records to the new bucket */
      Previous = &OldSegment[OldSegmentIndex];
      Current = *Previous;
      LastOfNew = &NewSegment[NewSegmentIndex];
      *LastOfNew = NULL;
      while (Current != NULL)
	{
	  if (_hash_hash (htab, Current->key) == NewAddress)
	    {
	      /* Attach it to the end of the new chain */
	      *LastOfNew = Current;

	      /* Remove it from old chain */
	      *Previous = Current->next;
	      LastOfNew = &Current->next;
	      Current = Current->next;
	      *LastOfNew = NULL;
	    }
	  else
	    {
	      /*
	      ** leave it on the old chain
	      */
	      Previous = &Current->next;
	      Current = Current->next;
	    }
	}
    }
  else
    {
      /* this can't be allowed to return--it creates an infinite loop */
      fprintf(stderr,"_hash_expand: hash table is full. stop.\n");
      exit(-1);
#if HASH_DEBUG
     fprintf (hash_debug_fp, "[_hash_expand] Table %lx not expanded\n", htab);
#endif
    }
}

Hash_element *
_hash_lookup (Hash_table *htab, const unsigned char *key, Hash_element ***p_p)
{
  Hash_address_t h;
  Hash_element **CurrentSegment;
  long int SegmentIndex;
  long int SegmentDir;
  Hash_element **p, *q;

  if (NULL == htab)
    return NULL;
#if HASH_STATISTICS
  HashAccesses++;
#endif
  h = _hash_hash (htab, key);
  SegmentDir = DIV (h, SEGMENT_SIZE);
  SegmentIndex = MOD (h, SEGMENT_SIZE);

  /* valid segment ensured by _hash_hash () */
  CurrentSegment = htab->directory [SegmentDir];
  assert (CurrentSegment != NULL);	/* bad failure if tripped */
  p = &CurrentSegment[SegmentIndex];
  q = *p;
  /* Follow collision chain */
  while (q && q->key && strcmp((const char *)q->key, (const char *)key) /*&& q->next*/)
    {
      p = &q->next;
      q = *p;
#if HASH_STATISTICS
      HashCollisions++;
#endif
    }
  if (NULL != p_p)
    *p_p = p;
  return q;
}

void
hash_exec (Hash_table *htab, void (*fnc)(void *))
{
  Unsigned32 i, j;
  Hash_element **s, *p;
 
  if (htab != NULL)
    {
      for (i = 0; i < htab->segment_count; i++)
	{
	  if ((s = htab->directory[i]) != NULL)
	    {
	      for (j = 0; j < SEGMENT_SIZE; j++)
		{
		  p = s[j];
		  while (p != NULL)
		    {
		      fnc (p->data);
		      p = p->next;
		    }
		}
	    }
	}
    }
}

void
hash_exec_user (Hash_table *htab, void (*fnc)(void *, void*), void *user)
{
  Unsigned32 i, j;
  Hash_element **s, *p;
 
  if (htab != NULL)
    {
      for (i = 0; i < htab->segment_count; i++)
	{
	  if ((s = htab->directory[i]) != NULL)
	    {
	      for (j = 0; j < SEGMENT_SIZE; j++)
		{
		  p = s[j];
		  while (p != NULL)
		    {
		      fnc (p->data, user);
		      p = p->next;
		    }
		}
	    }
	}
    }
}

void
hash_exec2 (Hash_table *htab, void (*fnc)(const unsigned char *,void *))
{
  Unsigned32 i, j;
  Hash_element **s, *p;
 
  if (htab != NULL)
    {
      for (i = 0; i < htab->segment_count; i++)
	{
	  if ((s = htab->directory[i]) != NULL)
	    {
	      for (j = 0; j < SEGMENT_SIZE; j++)
		{
		  p = s[j];
		  while (p != NULL)
		    {
		      fnc ((Uchar *)p->key, p->data);
		      p = p->next;
		    }
		}
	    }
	}
    }
}

void
hash_exec_user_key (Hash_table *htab, void (*fnc)(const unsigned char *, void*), void *user)
{
  Unsigned32 i, j;
  Hash_element **s, *p;
 
  if (htab != NULL)
    {
      for (i = 0; i < htab->segment_count; i++)
	{
	  if ((s = htab->directory[i]) != NULL)
	    {
	      for (j = 0; j < SEGMENT_SIZE; j++)
		{
		  p = s[j];
		  while (p != NULL)
		    {
		      fnc (p->key, user);
		      p = p->next;
		    }
		}
	    }
	}
    }
}

void
hash_exec_user_key_data (Hash_table *htab, void (*fnc)(const unsigned char *, void*, void *), void *user)
{
  Unsigned32 i, j;
  Hash_element **s, *p;
 
  if (htab != NULL)
    {
      for (i = 0; i < htab->segment_count; i++)
	{
	  if ((s = htab->directory[i]) != NULL)
	    {
	      for (j = 0; j < SEGMENT_SIZE; j++)
		{
		  p = s[j];
		  while (p != NULL)
		    {
		      fnc (p->key, p->data, user);
		      p = p->next;
		    }
		}
	    }
	}
    }
}

const char **
hash_keys (Hash_table *htab)
{
  return hash_keys2(htab, NULL);
}

const char **
hash_keys2 (Hash_table *htab, int *nkeys)
{
  Unsigned32 i, j;
  Hash_element **s, *p;
 
  if (htab != NULL)
    {
      const char **keys = malloc((1+htab->key_count)*sizeof(char *));
      int keyindex = 0;
      for (i = 0; i < htab->segment_count; i++)
	{
	  if ((s = htab->directory[i]) != NULL)
	    {
	      for (j = 0; j < SEGMENT_SIZE; j++)
		{
		  p = s[j];
		  while (p != NULL)
		    {
		      keys[keyindex++] = (const char*)p->key;
		      p = p->next;
		    }
		}
	    }
	}
      keys[keyindex] = NULL;
      if (nkeys)
	*nkeys = keyindex;
      return keys;
    }
  else
    return NULL;
}

#include "list.h"
List *
hash2list(Hash_table *htab, sort_cmp_func*cmp)
{
  if (htab != NULL)
    {
      static void **v;
      int vindex = 0;
      List *ret;
      Unsigned32 i, j;
      Hash_element **s, *p;
      v = malloc(htab->key_count * sizeof(void*));
      for (i = 0; i < htab->segment_count; i++)
	{
	  if ((s = htab->directory[i]) != NULL)
	    {
	      for (j = 0; j < SEGMENT_SIZE; j++)
		{
		  p = s[j];
		  while (p != NULL)
		    {
		      v[vindex++] = p->data;
		      p = p->next;
		    }
		}
	    }
	}
      if (cmp)
	qsort(v,htab->key_count,sizeof(void *),cmp);
      ret = list_create(LIST_DOUBLE);
      for (vindex = 0; vindex < htab->key_count; ++vindex)
	list_add(ret,v[vindex]);
      free(v);
      return ret;
    }
  else
    return NULL;
  
}

