#ifndef V2_H_
#define V2_H_

extern const char *v2g_get(unsigned int);
extern void v2g_init(const char *dir);
extern void v2g_term(void);
extern int v2_ids(const char *proj, const char *indx);
extern uintptr_t v2s_add(const unsigned char *s);
extern void v2s_init(void);
extern void v2s_term(void);
extern int v2s_file(const char *dir);
extern int v2s_save(int fh);

#endif
