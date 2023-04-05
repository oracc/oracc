#ifndef PROP_H_
#define PROP_H_

#include <memo.h>

enum propgroup
  {
   PG_NONE = 0,
   PG_GDL_INFO,
   PG_GDL_BREAK,
   PG_GDL_FLAGS,
   PG_GDL_STATE,
   PG_XML,
   PG_XNS,
   PG_LAST
  };

struct prop {
  int p; 		/* property type */
  int g;    		/* group type for easy filtering; some
			   predefined in enum propgroup, some user
			   defined at runtime hence 'int' */
  const char *k; 	/* key */
  const char *v; 	/* value */
  struct prop *next;
};

typedef struct prop Prop;

extern Prop *prop_add(Memo *propmem, Prop *p, int pt, int gt, const char *k, const char *v);
extern Prop *prop_last(Prop *p);
extern void prop_merge(Prop *into, Prop *from);
extern int prop_user_group(void);
extern Prop*prop_find_pg(Prop *p, int ptype, int gtype);
extern Prop*prop_find_kv(Prop *p, const char *key, const char *value);

#endif/*PROP_H_*/
