#ifndef _RNVIF_H
#define _RNVIF_H
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
#endif
