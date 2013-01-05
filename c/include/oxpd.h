#ifndef _XPD_H
#define _XPD_H
extern const char*xpd_option(const char*name);
extern int xpd_option_int(const char*name);
extern void xpd_init(void);
extern void xpd_term(void);
extern void xpd_echo(const char *xpd_proj,FILE *fp);
#endif/*_XPD_H*/
