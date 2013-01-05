#ifndef _XMD_H
#define _XMD_H
extern void xmd_init(void);
extern void xmd_term(void);
extern Hash_table *xmd_load(const char *project, const char *pq);
extern Hash_table *l2_xmd_load(const char *project, const char *pq);
#endif /*_XMD_H*/
