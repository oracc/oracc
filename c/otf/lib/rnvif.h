#ifndef _RNVIF_H
#define _RNVIF_H
#include <stdarg.h>
struct node;
extern int rnc_start;
extern const char *rnvif_text_id;
const char *rnvif_get_text(void);
extern void rnvif_init(void);
extern void rnvif_term(void);
extern void rnv_start_element(void *userData,const char *name,const char **attrs);
extern void rnv_end_element(void *userData,const char *name);
extern int rnv_validate(struct node *np);
extern void rnv_characters(void *userData,const char *s,int len);
extern int rnv_validate_start(void);
extern int rnv_validate_finish(void);
extern void (*rnl_verror_handler)(int erno,va_list ap);
extern void (*rnv_verror_handler)(int erno,va_list ap);
extern void (*xrnl_verror_handler)(int erno,va_list ap);
extern void (*xrnv_verror_handler)(int erno,va_list ap);
extern void rnl_set_verror_handler(void (*rnl_eh)(int erno,va_list ap));
extern void rnv_set_verror_handler(void (*rnv_eh)(int erno,va_list ap));
extern char *rnv_xmsg(void);

#endif
