#ifndef XML_H_
#define XML_H_	1

#include <tree.h>

#ifndef uccp
#define uccp unsigned const char *
#endif

struct xmlhelper {
  FILE *fp;
};

typedef struct xmlhelper Xmlhelper;

extern nodehandlers treexml_o_handlers;
extern nodehandlers treexml_p_handlers;
extern nodehandlers treexml_c_handlers;

extern void treexml_o_generic(Node *np, void *user);
extern void treexml_c_generic(Node *np, void *user);
extern void tree_ns_xml_print(Tree *tp, FILE *fp);
extern Xmlhelper *xmlh_init(FILE *fp);
extern const unsigned char *xmlify(const unsigned char *);
extern void xml_attr(const char **atts, FILE *fp);


#endif /* XML_H_ */
