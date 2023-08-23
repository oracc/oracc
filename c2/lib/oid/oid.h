#ifndef OID_H_
#define OID_H_

#include <stdint.h>
#include <time.h>
#include <hash.h>
#include <roco.h>

/* An Oids structure is the result of loading oid.tab or a key list of
   requests for OIDs */
struct oids
{
  const char *file;		/* source of oids */
  unsigned char *mem;		/* memory allocated by loadfile */
  unsigned char **lines;	/* array of ptrs to lines */
  size_t nlines;		/* count of lines */
  Hash *h;			/* hash with key=oid and value=struct oid* */
  struct oid *o;		/* array of parsed oids */
  struct oid **oo;		/* array of ptrs to the parsed oids */
};
typedef struct oids Oids;

/* the main oid.tab is parsed into struct oid; when new oids are being
   requested, the requests are also parsed into struct oid.

   oid.tab is a 5-field tab:

     OID DOMAIN KEY TYPE EXTENSION

   key.tab is a 4-field tab:

     DOMAIN KEY TYPE EXTENSION
*/
struct oid
{
  const char *id;		/* The OID */
  const char *domain;		/* SL or a LANG which scopes the key */
  unsigned char *key;		/* A unique key to pair with the OID;
				   may change and the change history
				   is maintained by the OID system */
  const char *type;		/* The type for the OID--presently
				   sign, word or sense */
  const char *extension;	/* For sense this is the word OID the
				   sense belongs to */
  struct oid_edit *history;  	/* List of changes for the OID encoded as history nodes */
};

/* Argument to OID parser to tell it what to expect */
enum oid_tab_t { ot_oids , ot_keys };
     
/* history operations */

struct oid_edits
{
  Roco *r;     		/* row-column structure */
  Hash *h;     		/* hash with key=oid and value=struct oid_history * */
  struct oid_edit *e;	/* array of parsed edits */
  struct oid_edit **ee;	/* array of ptrs to the parsed edits */
};
typedef struct oid_edits Oide;

enum oid_what { ow_add = '+' , ow_delete = '-' , ow_merge = '=' , ow_rename = '>' , ow_split = '|' };

/* A simple linked list for use by oid_history */
struct oid_list
{
  struct oid *oid;
  struct oid_list *next;
};

/* A simple linked list for use by oid_edits */
struct oid_ok_pair
{
  const char *oid;
  const char *key;
  struct oid_ok_pair *next;
};

/* An incoming edits file contains variable entries.
 *
 * All entries start with:
 *
 * WHEN OID KEY WHAT
 *
 * The WHEN field must be in ISO8601 YYYYMMDD format
 *
 * OID and KEY must match an entry in the relevant OID table; DOMAIN
 * is unnecessary for edit operations.
 *
 * For WHAT = +  there are no more fields
 * For WHAT = >  field 5 is a new KEY (rename)
 * For WHAT = -  field 5 is a required comment which may or may not contain linkable OIDs
 * For WHAT = =  fields 5-6 are an OID-KEY pair (merge);
 * For WHAT = |  fields 3+2/4/6-etc .. 4+2/4/6-etc contain OID-KEY pairs (split)
 *
 */
struct oid_edit
{
  const char *ymd;		/* year-month-day string for date of change */
  const char *oid;		/* OID affected by change */
  const unsigned char *key;	/* key for OID affected by change */
  enum oid_what what;		/* nature of change */
  struct oid_ok_pair *refs;	/* For a rename merge or split this is
				   a list of one or more OIDs that the
				   ref has moved to */
  struct oid_edit *prev;	/* previous change to this OID */
  unsigned char *comment;	/* Notes on the history action; optional */
  time_t when;			/* date of change */
};

struct oid_domain
{
  const char *name;
  const char *auth;
};

/* Oracc always uses --struct-type so we have to do this even when
   there is no structure in the gperf hash */
struct oid_type
{
  const char *name;
};

extern Oids *oid_load(void);
extern Oids *oid_load_keys(const char *file);
extern Oide *oid_load_edits(const char *file);
extern const char *oid_next_oid(Oids *o);
extern struct oid_domain *oid_domain (register const char *str, register size_t len);
extern struct oid_type *oid_type (register const char *str, register size_t len);
extern int oid_parse(Oids *o, enum oid_tab_t t);
extern void oid_write(FILE *fp, Oids*o);
extern void oid_set_oidtab(const char *s);
extern List *oid_assign(Oids *o, Oids *k);
extern const char *oid_domainify(const char *d, const char *k);
extern List *oid_wants(Oids *o, Oids *k);
extern struct oid_ok_pair *oid_ok_pair_last(struct oid_ok_pair *lp);
extern struct oid_ok_pair *oid_ok_pair(const char *oid, const char *key);

#endif/*OID_H_*/
