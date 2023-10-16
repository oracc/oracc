/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: dbi.c,v 0.5 1997/09/08 14:50:03 sjt Exp $
*/
#include <sys/types.h>
#include <c2types.h>
#include <xsystem.h>
#include <list.h>
#include <mesg.h>
#include <dbi.h>

extern FILE *fdopen(int,const char*);
extern int mkstemp(char *template);

Dbi_type_e
dbi_add (Dbi_index *dp, Uchar *key, void *data, Unsigned32 count)
{
  Dbi_tnode *find;
  Boolean found;
  Hash_element *q, **p;
  Unsigned32 dsize;

  if (NULL == dp || NULL == key || NULL == data)
    return DBI_ERROR;

  /* find match or insertion point */
  q = _hash_lookup (dp->keys, key, &p);
  dsize = dp->h.data_size;
  
  if (NULL == q)
    {
      *p = malloc (sizeof (Hash_element));
      (*p)->data = malloc (sizeof (Dbi_tnode));
      (*p)->next = NULL;
      find = (*p)->data;
      (*p)->key = find->key = (Uchar*)xstrdup((const char*)key);
      find->kid = dp->h.entry_count++;
      find->dcount = 0;
      find->kcount = 0;
      if (dp->cache_size > 0)
	{
	  find->cache = malloc (dp->cache_size * dsize);
	  find->cache_count = 0;
	}
      else
	{
	  find->cache = NULL;
	  find->cache_count = 0;
	}
      found = FALSE;
      dp->keys->key_count++;
      HASH_EXPAND_TABLE(dp->keys);
    }
  else
    {
      found = TRUE;
      find = q->data;
    }

  if (DBI_BALK == dp->type && found)
    return DBI_BALK;

  if (DBI_ACCRETE == dp->type)
    {
      if (dp->cache_size)
	{
	  if (find->cache_count + count <= dp->cache_size)
	    {
	      memcpy (&((char*)find->cache)[find->cache_count*dsize], data, count * dsize);
	      find->cache_count += count;
	    }
	  else
	    {
	      if (find->cache_count)
		{
		  xfwrite (dp->_tmp_fn, TRUE, &find->kid, sizeof (Unsigned32), 1, dp->_tmp_fp);
		  xfwrite (dp->_tmp_fn, TRUE, &find->cache_count, sizeof (Unsigned32), 1, dp->_tmp_fp);
		  xfwrite (dp->_tmp_fn, TRUE, (char*)find->cache, dsize,
			   find->cache_count, dp->_tmp_fp);
		  ++find->kcount;
		  find->dcount += find->cache_count;
		  find->cache_count = 0;
		}
	      if (count < dp->cache_size)
		{
		  memcpy (find->cache, data, count * dsize);
		  find->cache_count = count;
		}
	      else
		{
		  xfwrite (dp->_tmp_fn, TRUE, &find->kid, sizeof (Unsigned32), 1, dp->_tmp_fp);
		  xfwrite (dp->_tmp_fn, TRUE, &count, sizeof (Unsigned32), 1, dp->_tmp_fp);
		  xfwrite (dp->_tmp_fn, TRUE, data, dsize, count, dp->_tmp_fp);
		  ++find->kcount;
		  find->dcount += count;
		}
	    }
	}
      else
	{
	  xfwrite (dp->_tmp_fn, TRUE, &find->kid, sizeof (Unsigned32), 1, dp->_tmp_fp);
	  xfwrite (dp->_tmp_fn, TRUE, &count, sizeof (Unsigned32), 1, dp->_tmp_fp);
	  xfwrite (dp->_tmp_fn, TRUE, data, dsize, count, dp->_tmp_fp);
	  ++find->kcount;
	  find->dcount += count;
	}
    }
  else
    {
      if (DBI_REPLACE == dp->type)
	{
	  find->offset = ftell (dp->_tmp_fp);
	  find->kcount = 1;
#if 0
	  xfwrite (dp->_tmp_fn, TRUE, &find->kid, sizeof (Unsigned32), 1, dp->_tmp_fp);
#endif
	  xfwrite (dp->_tmp_fn, TRUE, &count, sizeof (Unsigned32), 1, dp->_tmp_fp);
	  xfwrite (dp->_tmp_fn, TRUE, data, dsize, count, dp->_tmp_fp);
	}
      else /* DBI_BALK */
	{
	  find->offset = ftell (dp->i_fp);
      	  xfwrite (dp->i_fname, TRUE, key, 1, strlen((const char*)key) + 1, dp->i_fp);
	  xfwrite (dp->i_fname, TRUE, &count, sizeof (Unsigned32), 1, dp->i_fp);
	  xfwrite (dp->i_fname, TRUE, data, dsize, count, dp->i_fp);
	}
      
    }
  
  if (found)
    return dp->type;
  else
    return DBI_OK;
}

void
dbi_close (Dbi_index *dp)
{
  if (NULL == dp)
    return;
  xfclose (dp->h_fname, dp->h_fp);
  xfclose (dp->i_fname, dp->i_fp);
  if (dp->h.ht_clash_count) 
    {
      free (dp->clash_table);
      free (dp->clash_indexes);
    }
  if (dp->data_buf_len)
    {
      free (dp->data);
      dp->data_buf_len = 0;
    }
  free (dp);
}

Dbi_index *
dbi_create (const char *name, const char *dir, size_t hash_init_size, size_t data_size, Dbi_type_e type)
{
  Dbi_index *tmp;

  if (NULL == name || NULL == dir)
    return NULL;
  else
    tmp = calloc (1,sizeof (Dbi_index));

  memset (tmp->h.name, '\0', DBI_NAME_LEN);
  strcpy ((char*)tmp->h.name, (const char*)name);
  strcpy ((char*)tmp->dir, dir);
#if 1
  sprintf(tmp->h_fname,"%s/%s.dbh", dir, name);
  sprintf(tmp->i_fname,"%s/%s.dbi", dir, name);
#else
  strcpy (tmp->h_fname, fname);
  strcpy (tmp->i_fname, fname);
#endif
  strcpy (tmp->_tmp_fn, "/tmp/dbiXXXXXX");
  tmp->i_fname[strlen (tmp->i_fname) - 1] = 'i';
  tmp->h.data_size = data_size;
  tmp->h.entry_count = 0;
  tmp->type = type;
  tmp->keys = hash_create (hash_init_size);
  /*  tmp->_tmp_fp = xfopen (tmpnam (tmp->_tmp_fn), "wb+");*/
  tmp->_tmp_fp = fdopen(mkstemp(tmp->_tmp_fn),"wb+");
  if (!tmp->_tmp_fp)
    {
      fprintf(stderr,"dbi: fdopen(mkstemp) failed\n");
      exit(-1);
    }
  if (tmp->type == DBI_BALK)
    {
      /* open the index file so we can write it at one pass */
      tmp->i_fp = xfopen (tmp->i_fname, "wb");
      xfwrite (tmp->i_fname, TRUE, &tmp->h.data_size, sizeof (tmp->h.data_size), 1, tmp->i_fp);
    }
  tmp->cache_size = 0;
  tmp->aliases = NULL;
  return tmp;
}

void *
dbi_detach_data (Dbi_index *dp, Unsigned32 *count)
{
  void *tmp;
  if (NULL != dp && dp->nfound)
    {
      tmp = dp->data;
      dp->data = NULL;
      dp->data_buf_len = 0;
      if (NULL != count)
	*count = dp->nfound;
      dp->nfound = 0;
    }
  else
    {
      tmp = NULL;
      if (NULL != count)
	*count = 0;
    }
  return tmp;
}

#define KBUFSIZ 256
static Uchar keybuf[KBUFSIZ];

Uchar *
dbi_each (Dbi_index *dip)
{
  int i = 0;
  Unsigned32 data_len;

  /* skip size info */
  if (dip->each_index < sizeof (dip->h.data_size))
    dip->each_index = sizeof (dip->h.data_size);

  if (dip->each_index != ftell (dip->i_fp))
    fseek (dip->i_fp, dip->each_index, SEEK_SET);

  do
    keybuf[i] = fgetc(dip->i_fp);
  while (keybuf[i] != '\0' && !feof(dip->i_fp) && ++i < KBUFSIZ);

  if (keybuf[i] != '\0')
    {
      if (i == 0) /* normal eof */
	{
	  dip->nfound = 0;
	  if (dip->data_buf_len)
	    {
	      free (dip->data);
	      dip->data = NULL;
	    }
	  dip->data_buf_len = 0;
	  dip->each_index = 0;
	  return NULL;
	}
      else if (feof(dip->i_fp))
	fprintf(stderr, "dbi: end-of-file reached while reading key from index %s", dip->i_fname);
      else
	fprintf(stderr, "dbi: key too long (max %d) while reading from index %s", KBUFSIZ, dip->i_fname);
    }

  xfread (dip->i_fname, TRUE, &dip->nfound, sizeof (Unsigned32), 1, dip->i_fp);
  data_len = dip->nfound * dip->h.data_size;
  if (dip->data_buf_len < data_len)
    {
      dip->data_buf_len = data_len;
      dip->data = realloc (dip->data, dip->data_buf_len);
    }
  xfread (dip->i_fname, TRUE, dip->data, dip->h.data_size, dip->nfound, dip->i_fp);
  dip->each_index = ftell (dip->i_fp);
  return keybuf;
}

static Dbi_index *tmp_dp;
static int clash_hdr_cmp (const void *k, const void *b);
static int clash_index_cmp (const void *k, const void *b);
static int disk_strcmp (const Uchar *key, FILE *fp, Unsigned32 offset);
void *_dbi_data_buf = NULL;
Unsigned32 _dbi_data_buf_len = 0;

void
dbi_find (Dbi_index *dp, const Uchar *key)
{
  Unsigned32 hashval = 0, offset = 0;

  if (dp->h.ht_size)
    {
      hashval = hash_ (key, dp->h.ht_size);
      fseek (dp->h_fp, dp->h.ht_begin + hashval * sizeof (Unsigned32), SEEK_SET);
      xfread (dp->h_fname, TRUE, &offset, sizeof (Unsigned32), 1, dp->h_fp);
    }
  else
    offset = 0;
  if (offset != 0)
    {
      Clash_hdr *cp;
      if (dp->h.ht_clash_count
          && NULL != (cp = bsearch (&hashval, dp->clash_table, 
				    (size_t)dp->h.ht_clash_count, sizeof (Clash_hdr), 
				    clash_hdr_cmp)))
	{
	  Unsigned32 *ret;
  	  tmp_dp = dp;
	  ret = bsearch (key, &dp->clash_indexes[cp->offset], 
			 (size_t)cp->count, sizeof (Unsigned32), clash_index_cmp);
	  if (NULL != ret)
	    offset = *ret;
	  else
	    offset = 0;
	}
      else
	{
	  if (disk_strcmp (key, dp->i_fp, offset))
	    offset = 0;
	}
    }
  if (offset == 0)
    {
      dp->nfound = 0;
      if (dp->data_buf_len)
	{
	  free (dp->data);
	  dp->data = NULL;
	}
      dp->data_buf_len = 0;
    }
  else
    {
      Unsigned32 data_len;
      fseek (dp->i_fp, offset + strlen((const char*)key) + 1, SEEK_SET);
      xfread (dp->i_fname, TRUE, &dp->nfound, sizeof (Unsigned32), 1, dp->i_fp);
      data_len = dp->nfound * dp->h.data_size;
      if (dp->data_buf_len < data_len)
	{
	  dp->data_buf_len = data_len;
          dp->data = realloc (dp->data, dp->data_buf_len);
	}
      xfread (dp->i_fname, TRUE, dp->data, dp->h.data_size, dp->nfound, dp->i_fp);
    }
}

static int
clash_hdr_cmp (const void *k, const void *b)
{
  if (*(Unsigned32*)k > ((Clash_hdr*)b)->hashval)
    return 1;
  else if (*(Unsigned32*)k < ((Clash_hdr*)b)->hashval)
    return -1;
  else
    return 0;
}
static int
clash_index_cmp (const void *k, const void *b)
{
  return disk_strcmp ((Uchar *)k, tmp_dp->i_fp, *(Unsigned32*)b);
}
/* compare the string at offset in fp against key and return as 
 * strcmp would; the string in fp must be \0-terminated.
 */
static int
disk_strcmp (const Uchar *key, FILE *fp, Unsigned32 offset)
{
  register int ch, i;
  fseek (fp, offset, SEEK_SET);
  for (i = 0; ; ++i)
    {
      ch = fgetc (fp);
      if ((unsigned)ch != key[i] || '\0' == ch)
	break;
    }
  if (ch || key[i])
    return (key[i] > (unsigned char)ch) ? 1 : -1;
  else
    return 0;
}

#define FLUSH_DEBUG 0
#define VALIDATE_CLASH 0
#define VALIDATE_OFFSETS 0
#define SHOW_WRITES 0

#define HASH_LOOSENESS		4
#define TMP_FILE_OVERHEAD	(sizeof(Unsigned32)+sizeof(Unsigned32))

#define xxfwrite(_fn,_t,_buf,_siz,_cnt,_fp) \
			if (fwrite(_buf,_siz,_cnt,_fp)!=_cnt)\
			  fprintf(stderr,"dbi: write failed on %s (disk full?)",_fn)
#define xxfread(_fn,_t,_buf,_siz,_cnt,_fp) \
			if (fread(_buf,_siz,_cnt,_fp)!=_cnt)\
			  fprintf(stderr,"dbi: read %ld bytes failed on %s at %s:%d",_cnt*_siz,_fn,__FILE__,__LINE__)

static List *clash_list;
static Uint clash_indexes_count;
static Uint clash_count;
static Uint max_clash_load;

struct Dbi_bin
  {
    int bin_id;
    char fn[L_tmpnam];
    FILE *fp;
    Unsigned32 nkeys;
    size_t size;
    Unsigned32 first_key;
    Unsigned32 last_key;
  };
typedef struct Dbi_bin Dbi_bin;
static Dbi_tnode **node_ptrs;	/* an array of node-pointers sorted
				 * first by kid and later by hash value */
static Uint np_index;		/* used when setting the node_ptrs array
				 * to point to the tree-nodes */
static Dbi_bin *bins;
static Int bins_count;
static Dbi_index *tmp_dp;	/* used to give list_exec function args
				 * access to the .dbh file we're writing */
static size_t direct_keys;

/** prototypes */
static int hash_cmp (const void *k1, const void *k2);
static int kid_cmp (const void *k1, const void *k2);
static int off_cmp (const void *k1, const void *k2);
static Unsigned16 open_bins (Unsigned16 start);
static Unsigned16 assign_bins (Unsigned32 dsize);
static void clash_free (const void *vp);
static void clashes (void);
static void hash_2_ptrs (void *p);
static void into_bins (Unsigned32 data_size, Unsigned32 cache_size);
static void note_clash (Unsigned32 index, Unsigned32 count);
static void reinitialize (void);
static void set_hash_table_info (Dbi_index_hdr * hp);
static void set_hash_vals (void);
static void sort_and_dump_bin (Dbi_index *dip, Dbi_bin *dbp);
static void sort_bins (Dbi_index * dp, Unsigned16 start, Unsigned16 to_do);
static void transfer_bin (Dbi_index*dip, Dbi_bin*dbp);
static void write_clash_hdr (const void *vp);
static void write_clash_indexes (const void *vp);
static void write_hash_table (void);
static void write_multiple (Dbi_index * dp);
static void write_single (Dbi_index * dp);

void
dbi_flush (Dbi_index * dp)
{
  if (NULL == dp)
    return;

  tmp_dp = dp;
  bins_count = 0;

#if FLUSH_DEBUG
  fprintf (stderr, "Dumping %ld keys to %s\n", dp->h.entry_count, dp->i_fname);
#endif

  if (dp->h.entry_count)
    {
      node_ptrs = malloc (dp->h.entry_count * sizeof (Dbi_tnode *));
      np_index = 0;
      if (dp->type == DBI_ACCRETE)
	{
	  /* Open the index file so we can maybe write some keys directly to it.
	     This can only affect DBI_ACCRETE type indexes */
	  dp->i_fp = xfopen (dp->i_fname, "wb");
	  /* write index size in position 0 so that 0 can be used as absence
	   * indicator in hash table (which stores offsets into this file).
	   * This also makes it possible to read the index file without reading
	   * the header.
	   */
	  xxfwrite (dp->i_fname, TRUE, &dp->h.data_size, sizeof (dp->h.data_size), 1, dp->i_fp);
	  direct_keys = 0;
	}
      else
	{
	  if (dp->type == DBI_BALK)
	    direct_keys = dp->h.entry_count;	/* purely for diagnostics: we actually
						   write keys directly to index with
						   no tmp file for DBI_BALK indexes */
	  else
	    direct_keys = 0;
	}
      hash_exec (dp->keys, hash_2_ptrs);
      hash_free (dp->keys, NULL);
#if FLUSH_DEBUG
      if (dp->type != DBI_REPLACE)
	fprintf (stderr, "%u keys written directly to index\n", direct_keys);
#endif
      if (dp->type != DBI_BALK && ftell (tmp_dp->_tmp_fp) > 0)
	{
	  if (dp->type == DBI_ACCRETE)
	    write_multiple (dp);
	  else if (dp->type == DBI_REPLACE)
	    {
	      dp->i_fp = xfopen (dp->i_fname, "wb");
	      /* write index size in position 0 so that 0 can be used as absence
	       * indicator in hash table (which stores offsets into this file).
	       * This also makes it possible to read the index file without reading
	       * the header.
	       */
	      xxfwrite (dp->i_fname, TRUE, &dp->h.data_size, sizeof (dp->h.data_size), 1, dp->i_fp);
	      write_single (dp);
	    }
	}
      xfclose (dp->i_fname, dp->i_fp);

#ifdef VALIDATE_OFFSETS
      for (np_index = 0; np_index < dp->h.entry_count; ++np_index)
	{
	  if (node_ptrs[np_index]->offset == 0)
	    fprintf(stderr,"dbi: key %s has 0 offset; kcount %d", 
		     node_ptrs[np_index]->key,
		     node_ptrs[np_index]->kcount
		     );
	}
#endif

#if FLUSH_DEBUG
  fprintf (stderr, "setting up clash table\n");
#endif
      set_hash_vals ();
      qsort (node_ptrs, dp->h.entry_count, sizeof (Dbi_tnode *), hash_cmp);
      clashes ();
      set_hash_table_info (&dp->h);
    }

  dp->h_fp = xfopen (dp->h_fname, "wb");
#if FLUSH_DEBUG
  fprintf (stderr, "writing header\n");
#endif
  xxfwrite (dp->h_fname, TRUE, &dp->h, sizeof (Dbi_index_hdr), 1, dp->h_fp);
#if FLUSH_DEBUG
  fprintf (stderr, "writing clash header\n");
#endif
  list_exec (clash_list, write_clash_hdr);
#if FLUSH_DEBUG
  fprintf (stderr, "writing clashes\n");
#endif
  list_exec (clash_list, write_clash_indexes);
#if FLUSH_DEBUG
  fprintf (stderr, "writing hash table\n");
#endif
  write_hash_table ();
#if FLUSH_DEBUG
  fprintf (stderr, "done\n");
#endif
  xfclose (dp->h_fname, dp->h_fp);
  reinitialize ();
}

static void
reinitialize ()
{
  Uint i;
  if (node_ptrs)
    {
      for (i = 0; i < np_index; ++i)
	{
	  free (node_ptrs[i]->key);
	  free (node_ptrs[i]);
	}
      free (node_ptrs);
      node_ptrs = NULL;
    }
  np_index = 0;
  if (bins_count)
    {
      free (bins);
      bins_count = 0;
    }
  list_free (clash_list, clash_free);
  clash_list = NULL;
  clash_indexes_count = clash_count = max_clash_load = 0;
}

static void
clash_free (const void *vp)
{
  free (((Clash*)vp)->clashes);
  free ((void*)vp);
}

/* This routine is overloaded to perform two quite separate tasks.
 * The original function simply set the node_ptrs entry. When caching
 * was added, the final cache-flushing was put in here to save making
 * an additional traversal of the hash table.
 *
 * 'p' is a pointer to the data element which is a Dbi_tnode type.
 */
static void
hash_2_ptrs (void *p)
{
  Dbi_tnode *pd = (Dbi_tnode*) p;
  node_ptrs[np_index++] = p;
  if (tmp_dp->type == DBI_ACCRETE)
    {
#if SHOW_WRITES
      fprintf (stderr, "h2p: %s:c=%d:k=%d\n", pd->key, pd->cache_count, pd->kcount);
#endif
      if (pd->cache_count)
	{
	  if (pd->kcount) /* we already wrote stuff to tmp */
	    {
	      xxfwrite (tmp_dp->_tmp_fn, TRUE, &pd->kid, sizeof (Unsigned32), 1, 
		       tmp_dp->_tmp_fp);
	      xxfwrite (tmp_dp->_tmp_fn, TRUE, &pd->cache_count, sizeof (Unsigned32), 1, 
		       tmp_dp->_tmp_fp);
	      xxfwrite (tmp_dp->_tmp_fn, TRUE, pd->cache, tmp_dp->h.data_size, pd->cache_count, 
		       tmp_dp->_tmp_fp);
	      ++pd->kcount;
	      pd->dcount += pd->cache_count;
	    }
	  else
	    {
	      /* write directly to the output file if we have everything in the cache */
#if SHOW_WRITES
	      fprintf (stderr, "%s written directly\n", pd->key);
#endif
	      ++direct_keys;
	      pd->offset = ftell (tmp_dp->i_fp);
	      xxfwrite (tmp_dp->i_fname, TRUE, pd->key, 1, strlen((const char *)pd->key) + 1, tmp_dp->i_fp);
	      xxfwrite (tmp_dp->i_fname, TRUE, &pd->cache_count, sizeof (Unsigned32), 1, tmp_dp->i_fp);
	      xxfwrite (tmp_dp->i_fname, TRUE, pd->cache, tmp_dp->h.data_size, 
		       pd->cache_count, tmp_dp->i_fp);
	    }
	}
      if (NULL != pd->cache)
	free (pd->cache);
    }
}

static void
write_multiple (Dbi_index * dp)
{
  int bins_to_do, bins_opened, first_bin = 0;

  qsort (node_ptrs, (size_t)dp->h.entry_count, sizeof (Dbi_tnode *), kid_cmp);
  bins_to_do = assign_bins (dp->h.data_size);
#if FLUSH_DEBUG
  fprintf (stderr, "splitting into %d bins\n", bins_to_do);
#endif
  if (bins_to_do)
    {
      while (bins_to_do > 0)
        {
          bins_opened = open_bins (first_bin);
          into_bins (dp->h.data_size, dp->cache_size);
          sort_bins (dp, first_bin, bins_opened);
          first_bin += bins_opened;
          bins_to_do -= bins_opened;
        }
    }
  else
    {
      static Dbi_bin odd_bin;
      odd_bin.bin_id = odd_bin.nkeys = 0;
      odd_bin.first_key = 0;
      odd_bin.last_key = dp->h.entry_count - 1;
      odd_bin.size = ftell (tmp_dp->_tmp_fp);
      strcpy (odd_bin.fn, tmp_dp->_tmp_fn);
      odd_bin.fp = tmp_dp->_tmp_fp;
      rewind (tmp_dp->_tmp_fp);
      if (odd_bin.first_key == odd_bin.last_key)
	transfer_bin (dp, &odd_bin);
      else
	sort_and_dump_bin (dp, &odd_bin);
    }
  xfclose (tmp_dp->_tmp_fn, tmp_dp->_tmp_fp);
  xremove (tmp_dp->_tmp_fn);
}

static int
kid_cmp (const void *k1, const void *k2)
{
  if ((*(Dbi_tnode **) k1)->kid > (*(Dbi_tnode **) k2)->kid)
    return 1;
  else if ((*(Dbi_tnode **) k1)->kid < (*(Dbi_tnode **) k2)->kid)
    return -1;
  else
    return 0;
}

static Unsigned16
assign_bins (Unsigned32 dsize)
{
  Int cutoff;
  Int i;
  Unsigned16 bin_id = 0;
  Int sofar = 0, len;

  len = ftell (tmp_dp->_tmp_fp);

  cutoff = 1024 * 1024;

  if (!len)
    abort();

  while ((cutoff * 10) < len)
    cutoff *= 2;

  bins_count = (Int) (len / cutoff + (len % cutoff != 0));
  
  if (1 == bins_count)
    return 0;

#if FLUSH_DEBUG
  fprintf(stderr,"assign_bins: bins_count = %d\n", (int)bins_count);
#endif

  bins = calloc (bins_count, sizeof (Dbi_bin));
  /*  bins[0].first_key = 0; */
  for (i = 0; i < tmp_dp->h.entry_count; ++i)
    {
#if 0
      static Dbi_tnode*dtp;
      dtp = node_ptrs[i];
#endif
      if (node_ptrs[i]->kcount)
	{
	  Unsigned32 new = ((node_ptrs[i]->kcount * TMP_FILE_OVERHEAD)
			    + (node_ptrs[i]->dcount * dsize));
	  if (sofar && sofar + new > cutoff)
	    {
	      bins[bin_id].size = sofar;
	      sofar = 0;
	      bins[bin_id].last_key = i - 1;
	      ++bin_id;
	      if (bin_id == bins_count)
		{
		  ++bins_count;
#if FLUSH_DEBUG
		  fprintf(stderr,"assign_bins: bins_count = %d\n", (int)bins_count);
#endif
		  bins = realloc (bins, bins_count * sizeof (Dbi_bin));
		}
	      bins[bin_id].first_key = i;
	    }
	  sofar += new;
	  node_ptrs[i]->bin = bin_id;
	  ++bins[bin_id].nkeys;
	}
    }
  if (sofar)
    {
      bins[bin_id].size = sofar;
      bins[bin_id].last_key = i - 1;
      ++bin_id;
    }

  /* boundary case: final bin can be zero length */
  if (!bins[bin_id-1].size)
    --bin_id;

  /* revise bins_count to reflect actual rather than projected number of bins */
  bins_count = bin_id;

  return bin_id;
}

/* open as many file descriptors as possible for the bins; if we get
 * a NULL back, use the previous one for all remaining bins, and
 * save it so we know what to open for input next time round.
 *
 * Return the number of bins which were successfully opened, excluding
 * the catch-all bin that has to take up the slack.
 */
static Unsigned16
open_bins (Unsigned16 start)
{
  static char new_tmp_fn[L_tmpnam];
  static FILE *new_tmp_fp = NULL;
  Unsigned16 i, bins_opened;
  Boolean tmp_flag = FALSE;

  if (new_tmp_fp != NULL)
    {
      xfclose (tmp_dp->_tmp_fn, tmp_dp->_tmp_fp);
      xremove (tmp_dp->_tmp_fn);
      strcpy (tmp_dp->_tmp_fn, new_tmp_fn);
      tmp_dp->_tmp_fp = new_tmp_fp;
      rewind (tmp_dp->_tmp_fp);
      new_tmp_fp = NULL;
    }

  for (bins_opened = 0, i = start; i < bins_count; ++i)
    {
      /*       bins[i].fp = fopen (tmpnam (bins[i].fn), "wb+"); */
      strcpy(bins[i].fn,"/tmp/binXXXXXX");
      bins[i].fp = fdopen(mkstemp(bins[i].fn),"wb+");
      if (!bins[i].fp)
	{
	  fprintf(stderr, "bin: fdopen(mkstemp) failed\n");
	  exit(-1);
	}
      bins[i].bin_id = start+i;
      if (NULL == bins[i].fp)
	{
	  if (i == start)
	    fprintf(stderr, "dbi: unable to open files for second stage of indexing");
	  --i;
	  tmp_flag = TRUE;
	  strcpy (new_tmp_fn, bins[i].fn);
	  new_tmp_fp = bins[i].fp;
	  while (++i < bins_count)
	    bins[i].fp = new_tmp_fp;
	  break;
	}
      else
	{
	  ++bins_opened;
#if FLUSH_DEBUG
	  fprintf(stderr,"opened bin %s; %d bins now open\n",bins[i].fn,bins_opened);
#endif
	}
    }

  /* discount any tmp bin at the end */
  return bins_opened - (tmp_flag ? 1 : 0);
}

void
into_bins (Unsigned32 data_size, Unsigned32 buf_size)
{
  Unsigned32 kid;
  void *buf;
  Unsigned32 buf_len = (buf_size > 0 ? buf_size : 8);

  buf = malloc (buf_len * data_size);
  rewind (tmp_dp->_tmp_fp);
#define DBP (&bins[node_ptrs[kid]->bin])
  for (;;)
    {
      Unsigned32 count;
      
      if (1 != fread (&kid, sizeof (Unsigned32), 1, tmp_dp->_tmp_fp))
	goto read_error;
      if (1 != fwrite (&kid, sizeof (Unsigned32), 1, DBP->fp))
	goto write_error;
      if (1 != fread (&count, sizeof (Unsigned32), 1, tmp_dp->_tmp_fp))
	goto read_error;
      if (1 != fwrite (&count, sizeof (Unsigned32), 1, DBP->fp))
	goto write_error;
      if (count > buf_len)
	{
	  while (count > buf_len)
	    buf_len *= 2;
	  buf = realloc (buf, buf_len * data_size);
	}
      if (count != fread (buf, data_size, count, tmp_dp->_tmp_fp))
	goto read_error;
      if (count != fwrite (buf, data_size, count, DBP->fp))
	goto write_error;
    }
 read_error:
  if (!feof (tmp_dp->_tmp_fp))
    fprintf(stderr, "dbi: error reading tmp file containing index data");
  else
    return;
 write_error:
  fprintf(stderr, "dbi: error writing index file (out of disk-space?)");
}

static void
sort_bins (Dbi_index * dp, Unsigned16 start, Unsigned16 to_do)
{
  Unsigned16 i;

  for (i = start; i < start + to_do; ++i)
    {
      /* reset the bin output for input */
      fflush (bins[i].fp);
      rewind (bins[i].fp);

      /* if the bin contains only one key read and write it one entry at
       * a time (the file may be too large to read at one go). If the bin
       * contains more than one key we know its <= cutoff in size, so
       * the sort routine slurps it up, sorts it and writes it out.
       */
      if (bins[i].first_key == bins[i].last_key)
	transfer_bin (dp, &bins[i]);
      else
	sort_and_dump_bin (dp, &bins[i]);

      /* clean up the bin */
      xfclose (bins[i].fn, bins[i].fp);
      xremove (bins[i].fn);
    }
}

/**The entire contents of this file have the same key id, so we just need
 * to dump the id, count and data onto the permanent data file.
 *
 * The tmp file has the structure 
 *	kid (size_t)
 *	count (size_t)
 *	data (dip->h.data_size*count)
 */
static void
transfer_bin (Dbi_index*dip, Dbi_bin*dbp)
{
  Unsigned32 i;
  Unsigned32 buf_len = 0;
  Dbi_tnode *np;
  void *buf = NULL;
  size_t new;

  /* set node pointer */
  np = node_ptrs[dbp->first_key];

#if FLUSH_DEBUG
  fprintf (stderr, "transferring bin %d: %ld bytes, %ld items, %ld blocks\n",
	   dbp->bin_id, dbp->size, (long)np->dcount, (long)np->kcount);
#endif

#if SHOW_WRITES
	      fprintf (stderr, "%s transferred\n", np->key);
#endif

  /* remember the offset of key for hash table */
  np->offset = ftell (dip->i_fp);

  /* write the key */
  xxfwrite (dip->i_fname, TRUE, np->key, 1, strlen((const char *)np->key) + 1, dip->i_fp);

  /* write count of data entries for key */
  xxfwrite (dip->i_fname, TRUE, &np->dcount, sizeof (Unsigned32), 1, dip->i_fp);

  /* write all data elements for key; recall that ->kcount gives the number of
     blocks for this key */
  for (i = 0; i < np->kcount; ++i)
    {
      fseek (dbp->fp, sizeof (Unsigned32), SEEK_CUR);	/* skip the kid */
      xxfread (dbp->fn, TRUE, &new, sizeof (Unsigned32), 1, dbp->fp);
      if (new > buf_len)
	{
	  buf_len = new;
	  free (buf);
	  buf = malloc (buf_len * dip->h.data_size);
	}
      xxfread (dbp->fn, TRUE, buf, dip->h.data_size, new, dbp->fp);
      xxfwrite (dip->i_fname, TRUE, buf, dip->h.data_size, new, dip->i_fp);
    }
}

struct D_info
{
  Unsigned32 count;
  void *data;
};
typedef struct D_info D_info;

static void
sort_and_dump_bin (Dbi_index *dip, Dbi_bin *dbp)
{
  D_info **di_bases, **di_ends, *di_rover, *di_rend;
  char *buf, *bp;
  Unsigned32 nkids = dbp->last_key - dbp->first_key + 1;
  Unsigned32 i, nkcounts, dsize = dip->h.data_size;

#if FLUSH_DEBUG
  fprintf (stderr, "sorting+dumping bin %d=%s: %ld bytes, %u keys (first=%d/last=%d)\n",
	   dbp->bin_id, dbp->fn, dbp->size, (int)dbp->nkeys, 
	   (int)dbp->first_key, (int)dbp->last_key);
#endif

  /* read in file */
  buf = malloc ((size_t)dbp->size);
  xxfread (dbp->fn, TRUE, buf, 1, dbp->size, dbp->fp);

  /* allocate array of pointers to D_info structures, one per kid in bin */
  di_bases = malloc (nkids * sizeof (D_info*));
  di_ends = malloc (nkids * sizeof (D_info*));

  /* allocate kcount D_info structures to each kid */
  for (nkcounts = i = 0; i < nkids; ++i)
    {
      Dbi_tnode *np = node_ptrs[dbp->first_key + i];
      if (np->kcount)
	{
	  di_bases[i] = malloc (sizeof (D_info) * np->kcount);
	  di_ends[i] = di_bases[i];
	  nkcounts += np->kcount;
	}
      else
	di_bases[i] = NULL;
    }

  /* run thru the file filling in the D_info's */
  for (i = 0, bp = buf; i < nkcounts; ++i)
    {
      Unsigned32 index = *((Unsigned32*)bp) - dbp->first_key;
      di_ends[index]->count = *(Unsigned32*)(bp+sizeof(Unsigned32));
      di_ends[index]->data = (void*)(bp+(2*sizeof(Unsigned32)));
      bp += (di_ends[index]->count * dsize) + (2*sizeof(Unsigned32));
      if (bp-buf > dbp->size)
	abort();
      ++di_ends[index];
    }
  
  /* write the stuff for each key */
  for (i = 0; i < nkids; ++i)
    {
      if (NULL != di_bases[i])
	{
	  register Dbi_tnode *np = node_ptrs[dbp->first_key + i];

#if SHOW_WRITES
	  fprintf (stderr, "%s sort&dumped\n", np->key);
#endif

	  /* remember the offset of key for hash table */
	  np->offset = ftell (dip->i_fp);
	  
	  /* write the key */
	  xxfwrite (dip->i_fname, TRUE, np->key, 1, strlen((const char *)np->key) + 1, dip->i_fp);

	  /* write data count for key */
	  xxfwrite (dip->i_fname, TRUE, &np->dcount, sizeof (Unsigned32), 1, dip->i_fp);

	  for (di_rover = di_bases[i], di_rend = di_ends[i]; di_rover < di_rend; ++di_rover)
	    {
	      xxfwrite (dip->i_fname, TRUE, di_rover->data, dsize, di_rover->count, dip->i_fp);
	    }
	}
    }
  free (buf);
  for (i = 0; i < nkids; ++i)
    if (NULL != di_bases[i])
      free (di_bases[i]);
  free (di_bases);
  free (di_ends);
}

/* This is used for indexes of type DBI_REPLACE only */
static void
write_single (Dbi_index * dp)
{
  void *buf;
  Uint buf_len = 8, i;

  buf = malloc ((size_t)(buf_len * dp->h.data_size));
  qsort (node_ptrs, (size_t)dp->h.entry_count, sizeof (Dbi_tnode *), off_cmp);
  rewind (dp->_tmp_fp);
  for (i = 0; i < dp->h.entry_count; ++i)
    {
      size_t count;
      register Dbi_tnode *np = node_ptrs[i];

      fseek (dp->_tmp_fp, node_ptrs[i]->offset, SEEK_SET);
      /* key is already known */
      /* read count of data elements */
      xxfread (dp->_tmp_fn, TRUE, &count, sizeof (Unsigned32), 1, dp->_tmp_fp);
      if (count > buf_len)
	{
	  buf_len *= 2;
	  free (buf);
	  buf = malloc ((size_t)buf_len * sizeof (dp->h.data_size));
	}
      /* read data proper */
      xxfread (dp->_tmp_fn, TRUE, buf, dp->h.data_size, count, dp->_tmp_fp);
      node_ptrs[i]->offset = ftell (dp->i_fp);
      /* write key */
      xxfwrite (dp->i_fname, TRUE, np->key, 1, strlen((const char *)np->key) + 1,
	       dp->i_fp);
      /* write count, ensuring correct size on 16 bit architectures */
#if defined(MSDOS)
      {
        Unsigned32 tmp = count;
        xxfwrite (dp->i_fname, TRUE, &tmp, sizeof (Unsigned32), 1, dp->i_fp);
      }
#else
      xxfwrite (dp->i_fname, TRUE, &count, sizeof (Unsigned32), 1, dp->i_fp);
#endif
      /* write data */
      xxfwrite (dp->i_fname, TRUE, buf, dp->h.data_size, count, dp->i_fp);
    }
  xfclose (dp->_tmp_fn, dp->_tmp_fp);
  xremove (dp->_tmp_fn);
  free (buf);
}

/* we know the comparands are unique */
static int
off_cmp (const void *k1, const void *k2)
{
  if ((*(Dbi_tnode **) k1)->offset > (*(Dbi_tnode **) k2)->offset)
    return 1;
  else if ((*(Dbi_tnode **) k1)->offset < (*(Dbi_tnode **) k2)->offset)
    return -1;
  else
    return 0;
}

static Unsigned32 hash_prime;
static void
set_hash_vals ()
{
  Uint i;

  hash_prime = hash_init (HASH_LOOSENESS * tmp_dp->h.entry_count);
  for (i = 0; i < tmp_dp->h.entry_count; ++i)
    node_ptrs[i]->hashval = hash_ (node_ptrs[i]->key, hash_prime);
  hash_term ();
}

/* sort primarily by hashval, but secondarily with strcmp, so that 
   keys that hash to the same will be in the right order for bsearch
   when clash resolution is performed
 */
static int
hash_cmp (const void *k1, const void *k2)
{
  if ((*(Dbi_tnode **) k1)->hashval > (*(Dbi_tnode **) k2)->hashval)
    return 1;
  else if ((*(Dbi_tnode **) k1)->hashval < (*(Dbi_tnode **) k2)->hashval)
    return -1;
  else
    return strcmp ((const char*)(*(Dbi_tnode **) k1)->key, 
		   (const char*)(*(Dbi_tnode **) k2)->key);
}

static Unsigned32 clash_offset;
static void
clashes ()
{
  Unsigned32 i = 0;

  while (i < tmp_dp->h.entry_count - 1)
    {
      if (node_ptrs[i]->hashval == node_ptrs[i + 1]->hashval)
	{
	  Unsigned32 j, nclashes;
	  for (j = i + 2; 
	       j < tmp_dp->h.entry_count && node_ptrs[i]->hashval == node_ptrs[j]->hashval; 
	       ++j)
	    ;
	  nclashes = j - i;
	  note_clash (i, nclashes);
	  i = j;
	}
      else
        ++i;
    }
  /* now we know how many clashes there are we can calculate the offset
   * member for each clash
   */
  clash_offset = 0;
  list_exec (clash_list, set_clash_offset);
}

void
set_clash_offset (const void *vp)
{
  ((Clash *) vp)->h.offset = clash_offset;
  clash_offset += ((Clash *) vp)->h.count;
}

/**Make a note that the hash values of index and the next count elements
 * clash.  The keys are guaranteed to be in strcmp collating sequence by
 * hash_cmp.
 */
static void
note_clash (Unsigned32 index, Unsigned32 count)
{
  Clash *tmp = malloc (sizeof (Clash));
  Uint i;

  if (NULL == clash_list)
    clash_list = list_create (LIST_SINGLE);

  tmp->h.hashval = node_ptrs[index]->hashval;
  tmp->h.count = count;
  tmp->clashes = malloc (count * sizeof (Unsigned32));

  for (i = 0; i < count; ++i)
    tmp->clashes[i] = node_ptrs[index + i]->offset;

  list_add (clash_list, tmp);
  clash_indexes_count += count;
  ++clash_count;
  if (count > (unsigned)max_clash_load)
    max_clash_load = count;
}

static void
set_hash_table_info (Dbi_index_hdr * hp)
{
  hp->ht_size = hash_prime;
  hp->ht_clash_count = clash_count;
  hp->ht_clash_indexes_count = clash_indexes_count;
  hp->ht_begin = sizeof (Dbi_index_hdr)
    + clash_count * sizeof (Clash_hdr)
    + clash_indexes_count * sizeof (Unsigned32);
}

static void
write_clash_hdr (const void *vp)
{
  xxfwrite (tmp_dp->h_fname, TRUE, &((Clash *) vp)->h, sizeof (Clash_hdr), 1, tmp_dp->h_fp);
}

static void
write_clash_indexes (const void *vp)
{
#if VALIDATE_CLASH
  /* validate the clash list */
  Unsigned32 i;
  for (i = 0; i < ((Clash *)vp)->h.count; ++i) 
    {
      if (((Clash *)vp)->clashes[i] == 0)
	fprintf(stderr, "dbi: invalid clash table");
    }
#endif
  xxfwrite (tmp_dp->h_fname, TRUE,
	   ((Clash *)vp)->clashes, sizeof(Unsigned32), ((Clash *)vp)->h.count, 
	   tmp_dp->h_fp);
}

/**writing of '0's could be optimized here by calculating number needed
 * and simply writing n 0-bytes in one go.
 */
static void
write_hash_table ()
{
  Uint i = 0, j, next_hash_value = 0, empty = 0;

  while (i < tmp_dp->h.entry_count)
    {
      /* write 0's until we reach this key's hash value */
      for (j = next_hash_value; j < node_ptrs[i]->hashval; ++j)
	xxfwrite (tmp_dp->h_fname, TRUE, &empty, sizeof (Unsigned32), 1, tmp_dp->h_fp);

      /* write the offset that this key is associated with */
      xxfwrite (tmp_dp->h_fname, TRUE, 
	       &node_ptrs[i]->offset, sizeof (Unsigned32), 1, tmp_dp->h_fp);

      /* remember next hash table value that we need an entry for */
      next_hash_value = node_ptrs[i]->hashval + 1;

      /* skip any clashes:
       *    if no clashes, move i by 1, 
       *    else move i to the first hash value after the clashes */
      do
        ++i;
      while (i < tmp_dp->h.entry_count && node_ptrs[i - 1]->hashval == node_ptrs[i]->hashval);
    }

  /* write 0's for tail of hash table */
  if (tmp_dp->h.entry_count)
    {
      for (j = next_hash_value; j < tmp_dp->h.ht_size; ++j)
	xxfwrite (tmp_dp->h_fname, TRUE, &empty, sizeof (Unsigned32), 1, tmp_dp->h_fp);
    }
}

void
dbi_free (Dbi_index *dp)
{
  free(dp);
}

Dbi_index *
dbi_open (const char *name, const char *dir)
{
  Dbi_index *tmp;

  if (!name || !dir)
    return NULL;
  tmp = malloc (sizeof (Dbi_index));
  sprintf(tmp->h_fname, "%s/%s.dbh", dir, name);
  sprintf(tmp->i_fname, "%s/%s.dbi", dir, name);
  tmp->h_fp = xfopen (tmp->h_fname, "rb");
  xxfread (tmp->h_fname, TRUE, &tmp->h, sizeof (Dbi_index_hdr), 1, tmp->h_fp);
  tmp->i_fp = xfopen (tmp->i_fname, "rb");
  if (tmp->h.ht_clash_count)
    {
      tmp->clash_table = malloc ((size_t)tmp->h.ht_clash_count * sizeof (Clash_hdr));
      tmp->clash_indexes = malloc ((size_t)tmp->h.ht_clash_indexes_count * sizeof (Unsigned32));
      xxfread (tmp->h_fname, TRUE, 
	      tmp->clash_table, sizeof (Clash_hdr), (size_t)tmp->h.ht_clash_count, 
	      tmp->h_fp);
      xxfread (tmp->h_fname, TRUE, 
	      tmp->clash_indexes, sizeof (Unsigned32), (size_t)tmp->h.ht_clash_indexes_count, 
	      tmp->h_fp);
    }
  tmp->suspended = FALSE;
  tmp->data_buf_len = 0;
  tmp->data = NULL;
  tmp->each_index = 0;
  tmp->aliases = NULL;
  return tmp;
}

void
dbi_set_cache (Dbi_index *dp, size_t elt_count)
{
  if (NULL == dp || dp->cache_size)
    abort();
  dp->cache_size = (Unsigned32)elt_count;
}

void
dbi_set_user (Dbi_index *dp, Unsigned32 udata)
{
  if (NULL == dp)
    abort();
  dp->h.ht_user = udata;
}

/**Hashing routine optimized for short strings with few distinct
 * characters (one application is use for hashing Sumerian graphemes
 * in transliteration: these have only 18 distinct characters and are
 * mostly of length 2-5.)
 */
static Unsigned32	next_prime (Unsigned32 n);

Unsigned32
hash_init (Unsigned32 n)
{
  return next_prime (n);
}

void
hash_term ()
{
}

Unsigned32
hash_ (register const Uchar *s, Unsigned32 hash_prime)
{
  Unsigned32 h = 0;
  int i;

  for (i = 0; s[i] != '\0'; ++i)
    h = h * 1009 ^ (s[i] + (i*256));
  return h % hash_prime;
}

/**return next prime bigger than n, based on Jon Bentley's P5 on pp. 5--7
 * of More Programming Pearls. Note that this code has the bug which his
 * P4 suffers from, but since this affects only the numbers 2, 3 and 5
 * it is of no concern to us here.
 */
static Unsigned32
next_prime (Unsigned32 n)
{
  Unsigned32 i;
  size_t entry = n;
  Boolean flag;

  while (++n != 0)
    {
      if (n % 2 == 0 || n % 3 == 0 || n % 5 == 0)
        continue;
      flag = TRUE;
      for (i = 7; i * i <= n; i += 2)
	{
          if (n % i == 0)
	    {
	      flag = FALSE;
	      break;
	    }
	}
      if (flag)
        break;
    }
  if (n == 0)
    fprintf(stderr, "dbi: Couldn't find a prime greater than %lu", entry);
  return n;
}
