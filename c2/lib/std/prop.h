#ifndef PROP_H_
#define PROP_H_

#include <keva.h>
#include <memo.h>
#include <tree.h>

/*#include <../gdl/gdlstate.h>*/

typedef unsigned long gdlstate_t;

enum propgroup
  {
    PU_GDLSTATE = 0, /* PU_GDLSTATE is its own macro group and uses u.s */
    PG_GDL_INFO,     /* PG_XXX > 0 && < PU_VOIDSTAR use u.k which may be null */
    PG_GDL_BREAK,
    PG_GDL_FLAGS,
    PG_GDL_GROUP,
    PG_GDL_STATE,
    PG_GVL_INFO,
    PG_XML,
    PG_XNS,
    PU_VOIDSTAR,     /* PG_XXX > PG_VOIDSTAR use u.v */
    PG_LAST
  };

struct prop
{
  int p; 		/* property type */
  int g;    		/* group type for easy filtering; some
			   predefined in enum propgroup, some user
			   defined at runtime hence 'int' */
  union {
    Keva *k;
    gdlstate_t s;
    void *v;
  } u;

  struct prop *next;
};

typedef struct prop Prop;
struct node;

/*extern gdlstate_t *prop_state(struct node *np, gdlstate_t *sp);*/
extern Prop *prop_add(Memo *propmem, Prop *p, int ptype, int gtype);
extern Prop *prop_add_kv(Memo *propmem, Memo *kevamem, Prop *p, int ptype, int gtype,
			 const char *key, const char *value);
extern Prop *prop_add_v(Memo *propmem, Prop *p, int ptype, int gtype, void *vp);  
extern Prop *prop_last(Prop *p);
extern void prop_merge(Prop *into, Prop *from);
extern void prop_node_add(struct node *np, int ptype, int gtype, const char *key, const char *value);
extern int prop_user_group(void);
extern Prop*prop_find_pg(Prop *p, int ptype, int gtype);
extern Prop*prop_find_kv(Prop *p, const char *key, const char *value);

#endif/*PROP_H_*/
