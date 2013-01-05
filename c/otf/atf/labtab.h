#ifndef _LABTAB_H
struct labtab_node
{
  unsigned char *label;
  unsigned char *xmlid;
};

struct labtab
{
  int alloced;
  int used;
  struct labtab_node *table;
};

extern struct labtab* labtab;
extern struct labtab* create_labtab(void);
extern void destroy_labtab(struct labtab*lt);
extern int find_label(struct labtab*lt,int start,unsigned char *l);
extern void register_label(struct labtab*lt,unsigned char *xid, unsigned char *label);

#endif /*_LABTAB_H*/
