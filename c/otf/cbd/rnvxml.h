#ifndef _RNVXML_H
#define _RNVXML_H

#include "xnn.h"
#include "rnvval.h"

extern void rnvxml_init(struct xnn_data *xdp);
extern void rnvxml_term(void);
extern void rnvxml_ch(const char *ch);
extern void rnvxml_ea(const char *pname, struct rnvval_atts *ratts);
extern void rnvxml_ee(const char *pname);

#endif/*_RNVXML_H*/
