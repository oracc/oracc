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

extern void sx_xml(struct sl_signlist *sl);
extern void sx_xml_init(void);

extern struct sl_functions *sx_asl_init(FILE *fp, const char *fname);
extern void sx_marshall(struct sl_signlist *sl);

#endif/*SX_H_*/
