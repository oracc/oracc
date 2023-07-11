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

extern void sx_xml(struct sl_signlist *sl);
extern void sx_xml_init(void);

#endif/*SX_H_*/
