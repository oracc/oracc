#ifndef _RNVVAL_H
#define _RNVVAL_H

struct rnvval_atts {
  const char **atts;
  const char **qatts;
};

extern void rnvval_init_err(void (*eh)(int erno,va_list ap));
extern void rnvval_init(struct xnn_data *xdp, const char *rncfile);
extern void rnvval_term(void);
extern void rnvval_free_atts(struct rnvval_atts *ratts);
extern struct rnvval_atts *rnvval_aa(const char *pname, ...);
extern struct rnvval_atts *rnvval_aa_qatts(char **atts, int natts);
extern void rnvval_ch(const char *ch);
extern void rnvval_ea(const char *pname, struct rnvval_atts *ratts);
extern void rnvval_ee(const char *pname);

#endif/*_RNVVAL_H*/
