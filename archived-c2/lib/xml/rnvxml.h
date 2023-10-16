#ifndef RNVXML_H_
#define RNVXML_H_

#include <tree.h>
#include "xnn.h"
#include "rnvval.h"

struct rnvdata {
  const char *tag;
  unsigned const char *chardata;
  struct rnvval_atts *ratts;
  int ns;
};

extern void rnc_default_verror_handler(int er_no,va_list ap);
extern void rnvxml_init_err(void);
extern void rnvxml_init_tree(Tree *tp, struct xnn_data *xdp, const char *rncbase);
extern void rnvxml_init(struct xnn_data *xdp, const char *rncbase);
extern void rnvxml_term_tree(Tree *tp);
extern void rnvxml_term(void);
extern void rnvxml_ch(const char *ch);
extern void rnvxml_ea(const char *pname, struct rnvval_atts *ratts);
extern void rnvxml_ec(const char *pname, struct rnvval_atts *ratts);
extern void rnvxml_ee(const char *pname);
extern void rnvxml_et(const char *pname, struct rnvval_atts *ratts, const char *ch);

#endif/*RNVXML_H_*/
