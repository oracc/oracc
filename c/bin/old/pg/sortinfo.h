#ifndef _SORTINFO_H
#define _SORTINFO_H

#define PQ_MAX 		0x00200000
#define PQ_BIT 		(1<<31)
#define setQ(x)		((x)|=PQ_BIT)
#define isQ(t)		((t)&PQ_BIT)

#include <stdint.h>

typedef int32_t s4;
typedef uint32_t u4;

struct si_field
{
  unsigned char  n[128];
  unsigned char hr[128];
  u4 field_index;
};

struct sortinfo
{
  u4 nmember; 			/* number of members in the set of texts */
  u4 qoffset; 			/* index into idmap where Q's start */
  u4 nfields; 			/* number of fields used in sortinfo.tab */
  u4 nmapentries; 	       	/* number of fields used in sortinfo.tab */
  u4 pool_len;			/* length of string pool in bytes */
  u4 *idlist;			/* ids in two blocks, each block independently
				   numerically sorted; second block starts
				   at idlist[qoffset] 
				 */
  s4 *scodes;   	       	/* per-member data giving sort codes */
  u4 *pindex; 			/* per-member data giving index of field
				   values in string pool
				 */
  struct si_field *fields; 	/* field name data for each field */
  unsigned char   *pool;   	/* string pool */
};

struct si_cache
{
  s4*codes;
  short id;
  short seq;
};

struct sortinfo *si_load_tab(unsigned char *tab,size_t tabsize);
void si_dump_csi(unsigned char *csiname,struct sortinfo *sip);
struct sortinfo *si_load_csi();

#endif /*_SORTINFO_H*/
