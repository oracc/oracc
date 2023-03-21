#ifndef PROP_H_
#define PROP_H_

struct prop {
  int p; 		/* property type */
  int g; 		/* group type for easy filtering */
  const char *k; 	/* key */
  const char *v; 	/* value */
  struct prop *next;
};

typedef struct prop Prop;

extern Prop *prop_add(Prop *, int pt, int gt, const char *k, const char *v);

#endif/*PROP_H_*/
