#ifndef GUTIL_H_
#define GUTIL_H_

extern wchar_t *g_wlc(wchar_t *w);
extern wchar_t *g_wuc(wchar_t *w);
extern unsigned char *g_lc(unsigned const char *g);
extern unsigned char *g_uc(unsigned const char *g);
extern unsigned char *g_base_of(const unsigned char *v);
extern int g_index_of(const unsigned char *g, const unsigned char *b);
extern const char *g_sub_of(int i);
extern wchar_t g_subdig_of(wchar_t w);
extern wchar_t g_vowel_of(wchar_t w);

#endif/*GUTIL_H*/
