/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: hash.h,v 0.3 1997/09/08 14:49:57 sjt Exp $
*/

#ifndef HASH_H_
#define HASH_H_ 1

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <list.h>

typedef int sort_cmp_func(const void*,const void*);

typedef void hash_exec_func(void *);
typedef void hash_exec2_func(Uchar*,void *);
typedef void hash_free_func(void *);

#define HASH_DEBUG	0
#define HASH_STATISTICS 0

#if HASH_DEBUG
extern FILE *hash_debug_fp;
#endif

#define SEGMENT_SIZE		(Int)1024
#define SEGMENT_SIZE_SHIFT	10	/* log2(SEGMENT_SIZE)	*/
#define DIRECTORY_SIZE		(Int)1024
#define DIRECTORY_SIZE_SHIFT	10	/* log2(DIRECTORY_SIZE)	*/
#define PRIME_1			37
#define PRIME_2			1048583
#define DEFAULT_MAX_LOAD_FACTOR	5
 
/**Fast arithmetic, relying on powers of 2, and on pre-processor 
 * concatenation property
 */
#define MUL(x,y)		((x) << (y##_SHIFT))
#define DIV(x,y)		((x) >> (y##_SHIFT))
#define MOD(x,y)		((x) & ((y)-1))
 
/** local data templates */
typedef struct _HASH_ELEM
{
  const unsigned char *key;
  void *data;
  struct _HASH_ELEM *next;
} Hash_element;
 
typedef struct Hash
{
  Int p;			/* Next bucket to be split	*/
  Int maxp;			/* upper bound on p during expansion	*/
  Int key_count;		/* current # keys	*/
  Unsigned32 segment_count;	/* current # segments	*/
  int min_load_factor;
  int max_load_factor;
  int freeable_data;            /* do we free data during hash_add? */
  Hash_element **directory[DIRECTORY_SIZE];
} Hash;
 
typedef unsigned long Hash_address_t;

#if HASH_STATISTICS
extern long HashAccesses, HashCollisions;
#endif

#define HASH_EXPAND_TABLE(htab) \
	while ((htab)->key_count \
		/ MUL ((htab)->segment_count, SEGMENT_SIZE) \
	    > (htab)->max_load_factor) \
	  _hash_expand(htab)

extern  void hash_add (Hash *htab, const unsigned char *key, void *data);
extern  Hash *hash_create (Unsigned32 count);
extern  void hash_exec (Hash *htab, void (*fnc)(void *));
extern  void hash_exec_user (Hash *htab, void (*fnc)(void *, void *), void *user);
extern  void hash_exec_user_key (Hash *htab, void (*fnc)(const unsigned char *, void*), void *user);
extern  void hash_exec_user_key_data (Hash *htab, void (*fnc)(const unsigned char *, void*, void *), void *user);
extern  void hash_exec2 (Hash *htab, void (*fnc)(const unsigned char *,void *));
extern  unsigned const char *hash_exists (Hash *htab, const unsigned char *key);
extern  void *hash_find (Hash *htab, const unsigned char *key);
extern  void hash_free (Hash *htab, void (*fnc)(void *));
extern  void hash_free2 (Hash *htab, void (*keyfnc)(void *), void (*datfnc)(void *));
extern  Hash_address_t _hash_hash (Hash *htab, const unsigned char *key);
extern  void _hash_expand(Hash *htab);
extern  const char **hash_keys (Hash *htab);
extern  const char **hash_keys2 (Hash *htab, int *nkeys);
extern  Hash_element *_hash_lookup (Hash *htab, const unsigned char *key, Hash_element ***p_p);
extern  void hash_xfree (void *vp);
extern  void hash_freeable_data(Hash *h,int i);
extern  List *hash2list(Hash *h, sort_cmp_func *cmp);
extern  Hash *hash_from_var_char_star(const char *required, ...);

#endif /*HASH_H_*/
