/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: dbi.h,v 0.3 1997/09/08 14:49:55 sjt Exp $
*/

#ifndef DBI_H
#define DBI_H_	1

#include <dra.h>
#include <hash.h>

/**A database index uses two files, the header file, with a .dbh 
 * extension, and the index file, with a .dbi extension.
 *
 * The structure of a .dbh file:
 *
 *	Dbi_index_hdr structure
 *	Table of clash headers
 *	Array of indexes of keys which clash
 *	hash table proper: each entry is 0 or the index of key in the
 *		.dbi file
 *
 * The structure of a .dbi file is a sequence of groups consisting of:
 *
 *	Key, \0-terminated
 *	count
 *	data
 */
#define HASH_KEYS	1	/* initial implementation used tree for
				 * keys in memory, but new version uses
				 * hash table (avoids need to implement
				 * balancing scheme in tree) */
#define DBI_NAME_LEN	64	/* not too important how long we allow 
				 * because it's just for humans to know
				 * what the file contains */


/**This is all the information that needs to be written in the header */
struct Dbi_index_hdr
{
  Uchar name[DBI_NAME_LEN];	/* name of index, informal (for humans only) */
  Unsigned32 entry_count;	/* number of keys in it */
  Unsigned32 data_size;		/* size of datum elements */
  Unsigned32 ht_size;		/* size of hash table */
  Unsigned32 ht_clash_count;	/* number of clashes in hash table */
  Unsigned32 ht_clash_indexes_count;	/* length of clash index array */
  Unsigned32 ht_begin;		/* offset of hash table in .dbh file */
  Unsigned32 ht_user;		/* for communicating data-type from 
				   indexer to search-engine */
};
typedef struct Dbi_index_hdr Dbi_index_hdr;

/**There are three types of index: those in which data is gathered to
 * a key, those whose key has only one data item, and those in which it
 * is an error to find a key already there.
 * 
 * This type is also used for return values from dbi_add, hence the 
 * inclusion of DBI_OK.
 */
enum Dbi_type_e 
{
  DBI_ERROR, DBI_OK, DBI_ACCRETE, DBI_REPLACE, DBI_BALK
};
typedef enum Dbi_type_e Dbi_type_e;

/**We store the keys in a tree along with some necessary info
 * 
 * This structure is used only internally while building index files and
 * thus is allowed to contain variable-size data types (like Uint)
 */
struct Dbi_tnode
{
  Uchar *key;			/* pointer to key */
  Unsigned32 kid;		/* unique numeric ID of key (KID) */
  Unsigned32 dcount;		/* count of data items found for key;
				 * used for offset in tmp file if index
				 * type is DBI_REPLACE */
  Unsigned32 kcount;			/* count of blocks of data in tmp file
				 * pertaining to key */
  Unsigned32 offset;		/* offset of key in permanent data file */
  Unsigned32 hashval;		/* hash value of key */
  Unsigned16 bin;		/* which temp bin the data for key goes in */
  void *cache;
  Unsigned32 cache_count;
};
typedef struct Dbi_tnode Dbi_tnode;

struct Clash_hdr
{
  Unsigned32 hashval;		/* value which clashes */
  Unsigned32 offset;		/* offset of its clashes in hash table file */
  Unsigned32 count;		/* how many clashes there were */
};
typedef struct Clash_hdr Clash_hdr;
struct Clash
{
  Clash_hdr h;
  Unsigned32 *clashes;		/* array of offsets at which the clashing
				 * keys begin */
};
typedef struct Clash Clash;

/**It's also useful to keep other administrative info around when
 * working with an index
 */
struct Dbi_index
{
  Dbi_index_hdr h;		/* header info */
  char dir[_MAX_PATH];		/* directory where this dbi's files live */
  char h_fname[_MAX_PATH];	/* file name of hash.dbi file */
  char i_fname[_MAX_PATH];	/* file name of inst.dbi file */
  char _tmp_fn[L_tmpnam];	/* tmp file for building index */
  FILE *h_fp;			/* header fp */
  FILE *i_fp;			/* index fp */
  FILE *_tmp_fp;		/* tmp file fp */
  Clash_hdr *clash_table;	/* table of hash items which clash */
  Unsigned32 *clash_indexes;	/* array of indexes to keys for hash-clashes */
  Hash_table *keys;		/* hash table of keys used when building index */
  Unsigned32 nfound;		/* number of data elements found by lookup */
  void *data;			/* actual data found */
  Unsigned32 data_buf_len;     	/* length of 'data' in bytes */
  Boolean suspended;		/* true when an index has been suspended */
  Dbi_type_e type;		/* do we accrete, replace or balk on finding
				 * key when adding data? */
  Unsigned32 cache_size;	/* size of cache used for keys when accreting
				 * expressed as the number of data elements that
				 * will fit in the cache */
  off_t each_index;		/* the current location in i_fp for dbi_each */
  struct Dbi_index *aliases;	/* aliases for this index */
  struct Dbi_index *signmap;	/* sign map for this index */
};
typedef struct Dbi_index Dbi_index;

/**prototypes */
extern Dbi_index *	dbi_create (const char *name, const char *fname, size_t hash_init_size, size_t data_size, Dbi_type_e type);
extern Dbi_index *	dbi_open (const char *project,const char *index);
extern Dbi_type_e	dbi_add (Dbi_index *dp, Uchar *key, void *data, Unsigned32 count);
extern Unsigned32	hash_init (Unsigned32 n);
extern Unsigned32	hash (register const Uchar *s, Unsigned32 hash_prime);
extern void	dbi_flush (Dbi_index * dp);
extern void	dbi_free (Dbi_index *dp);
extern void	dbi_close (Dbi_index *dp);
extern void	dbi_find (Dbi_index *dp, const Uchar *key);
extern void	dbi_resume (Dbi_index *dp);
extern void	dbi_sort (const char *dbname, int (*cmp) (const void *, const void *));
extern void	dbi_suspend (Dbi_index *dp);
extern void	hash_term (void);
extern void	set_clash_offset (void *vp);
extern void *	dbi_detach_data (Dbi_index *dp, Unsigned32 *count);
extern void 	dbi_set_cache (Dbi_index *dp, size_t elt_count);
extern void	dbi_set_user (Dbi_index *dp, Unsigned32 udata);
extern Uchar *	dbi_each (Dbi_index *dip);

#endif /* DBI_H_ */
