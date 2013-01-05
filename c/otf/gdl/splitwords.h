#ifndef _SPLITWORDS_H
#define _SPLITWORDS_H

struct node *sw_get_head(void);
void sw_add(struct node *wp);
void sw_free(void);
char *sw_refs(void);
char *sw_form(void);

#endif /*_SPLITWORDS_H*/
