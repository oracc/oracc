#ifndef OID_H_
#define OID_H_

#include <stdint.h>
#include <time.h>

struct oid
{
  const char *id;		/* The OID */
  const char *domain;		/* SL or a LANG which scopes the key */
  unsigned char *key;		/* A unique key to pair with the OID;
				   may change and the change history
				   is maintained by the OID system */
  const char *type;		/* The type for the OID--presently
				   sign, word or sense */
  const char *parent_id;	/* For sense this is the word OID the
				   sense belongs to */
  struct oid_history *history;  /* List of changes for the OID encoded as history nodes */
};

enum oid_what { ow_delete , ow_merge , ow_rename , ow_split };

/* A simple linked list for use by oid_history */
struct oid_list
{
  struct oid *oid;
  struct oid_list *next;
};

struct oid_history
{
  const char *ref;		/* reference to an OID */
  enum oid_what what;		/* nature of change */
  time_t when;			/* date of change */
  struct oid_history *prev;	/* previous change to this OID */
  struct oid_list *oids;	/* For a merge or split this is a list
				   of one or more OIDs that the ref
				   has moved to */
};

extern void oid_load(void);
extern int32_t oid_next_id(void);

#endif/*OID_H_*/
