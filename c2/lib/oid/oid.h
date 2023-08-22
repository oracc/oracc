#ifndef OID_H_
#define OID_H_

#include <stdint.h>
#include <time.h>
#include <hash.h>


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
  struct oid_history *history;  /* List of changes for the OID encoded as history nodes */
};

/* Argument to OID parser to tell it what to expect */
enum oid_tab_t { ot_oids , ot_keys };

/* History .tab has the following fields:

   TIME OID ACTION REFOIDS

   ACTION is one of:

     delete merge rename split comment

   REFOIDS is

     empty for delete;
     a singleton for merge and rename;
     multiple for split
 */
     
/* history operations */
enum oid_what { ow_delete , ow_merge , ow_rename , ow_split };

/* A simple linked list for use by oid_history */
struct oid_list
{
  struct oid *oid;
  struct oid_list *next;
};

struct oid_history
{
  const char *oid;		/* OID affected by change */
  enum oid_what what;		/* nature of change */
  time_t when;			/* date of change */
  struct oid_history *prev;	/* previous change to this OID */
  struct oid_list *oids;	/* For a merge or split this is a list
				   of one or more OIDs that the ref
				   has moved to */
  unsigned char *comment;	/* Notes on the history action; optional */
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
extern int32_t oid_next_id(Oids *o);
extern struct oid_domain *oid_domain (register const char *str, register size_t len);
extern struct oid_type *oid_type (register const char *str, register size_t len);
extern int oid_parse(Oids *o, enum oid_tab_t t);


#endif/*OID_H_*/
