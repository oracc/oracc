#ifndef SX_H_
#define SX_H_
#include <asl.h>
#if 0
extern void sx_p_form(Node *np, char *data);
extern void sx_p_sign(Node *np, char *data);
extern void sx_p_v(Node *np, char *data);
extern void sx_gparse(Node *np, char *data);
extern void sx_vpp_parse(Node *np, char *data);
#endif

extern int identity_mode;
extern Hash *oids;
extern Hash *oid_sort_keys;

extern void sx_xml(struct sl_signlist *sl);
extern void sx_xml_init(void);

extern struct sl_functions *sx_asl_init(FILE *fp, const char *fname);
extern struct sl_functions *sx_sll_init(FILE *fp, const char *fname);

extern void sx_compounds(struct sl_signlist *sl);
extern void sx_compound_digests(struct sl_signlist *sl);
extern void sx_homophones(struct sl_signlist *sl);
extern void sx_inherited(struct sl_signlist *sl);
extern void sx_marshall(struct sl_signlist *sl);
extern void sx_qualified(struct sl_signlist *sl);
extern void sx_values_by_oid(struct sl_signlist *sl);
extern void sx_values_parents(struct sl_signlist *sl);
extern void sx_values_parents_dump(struct sl_signlist *sl);

#endif/*SX_H_*/
