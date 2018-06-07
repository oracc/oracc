#ifndef _ATF_H
#define _ATF_H
#include <stddef.h>
#include <sys/types.h>
#include "atflocale.h"
#include "lang.h"

enum t_unicode 
{ 
  u_sz, u_SZ, u_sin, u_SIN, u_sadhe, u_SADHE, u_tet, u_TET, u_aleph, u_ayin ,
  u_eng, u_ENG, u_heth, u_HETH,
  u_s0, u_s1, u_s2, u_s3, u_s4, u_s5, u_s6, u_s7, u_s8, u_s9, u_splus, u_s_x ,
  u_amacr, u_emacr, u_imacr, u_umacr,
  u_Amacr, u_Emacr, u_Imacr, u_Umacr,
  u_acirc, u_ecirc, u_icirc, u_ucirc,
  u_Acirc, u_Ecirc, u_Icirc, u_Ucirc,
  u_times,
  u_mod_r , 
  u_ulhsq , u_urhsq , u_llhsq , u_lrhsq , 
  u_cdot ,
  u_top 
};

#define U_cdot  0x00b7
#define U_degree 0x00b0
#define U_sz	0x0161
#define U_SZ 	0x0160
#define U_s0 	0x2080
#define U_s1 	0x2081
#define U_s2 	0x2082
#define U_s3 	0x2083
#define U_s4 	0x2084
#define U_s5 	0x2085
#define U_s6 	0x2086
#define U_s7 	0x2087
#define U_s8 	0x2088
#define U_s9 	0x2089
#define U_splus 0x208a
#define U_s_x   0x2093
#define U_eng 	0x014b
#define U_ENG 	0x014a
#define U_heth 	0x1e2b
#define U_HETH 	0x1e2a
#define U_sadhe	0x1e63
#define U_SADHE 0x1e62
#define U_sin 	0x015b
#define U_SIN 	0x015a
#define U_tet 	0x1e6d
#define U_TET 	0x1e6c
#define U_aleph 0x02be
#define U_ayin  0x02bf
#define U_amacr 0x0101
#define U_emacr 0x0113
#define U_imacr 0x012b
#define U_umacr 0x016b
#define U_Amacr 0x0100
#define U_Emacr 0x0112
#define U_Imacr 0x012a
#define U_Umacr 0x016a
#define U_acirc 0x00e2
#define U_ecirc 0x00ea
#define U_icirc 0x00ee
#define U_ucirc 0x00fb
#define U_Acirc 0x00c2
#define U_Ecirc 0x00ca
#define U_Icirc 0x00ce
#define U_Ucirc 0x00db
#define U_times 0x00d7
#define U_mod_r 0x02b3

#define U_ulhsq 0x2e22
#define U_urhsq 0x2e23
#define U_llhsq 0x2e24
#define U_lrhsq 0x2e25

struct langdata;

extern const char *file;
extern int lnum;

extern unsigned char *atf2utf(register const unsigned char *s, int rx);
extern void atf2utf_init(void);
extern unsigned char * gatf2utf(const unsigned char *lem, const char *f, ssize_t ln);
extern unsigned char *wcs2atf(wchar_t *wc, size_t len);
extern unsigned char *wcs2utf(const wchar_t*ws, size_t len);
extern wchar_t*wcs_lcase(wchar_t*ws);
extern wchar_t*wcs_ucase(wchar_t*ws);
extern wchar_t*wcs_undet(wchar_t*ws);
extern unsigned char *remap_detchars(unsigned char *s);
extern unsigned char *utf8_times(void);
extern const unsigned char *utf_lcase(const unsigned char *s);
extern const unsigned char *utf_ucase(const unsigned char *s);
extern wchar_t utf1char(const unsigned char *src,size_t *len);
extern unsigned char *utf2atf(const unsigned char *src);
extern wchar_t* utf2wcs(const unsigned char *src, size_t *len);
extern int u_isupper(const unsigned char *g);
extern int u_islower(const unsigned char *g);
extern void atf_init(void);
extern void atf_term(void);
extern void charsets_init(void);
extern void charset_init_lang(struct lang_context *lp);
extern void charsets_term(void);
extern int u_charbytes(const unsigned char *g);
extern unsigned char *utf8_check(unsigned char *s);
extern int vchars(unsigned char *ftext, ssize_t fsize);
extern int is_signlist(register const unsigned char *s);

extern unsigned char *accnum(const unsigned char *);
extern unsigned char *numacc(const unsigned char *g);
extern int hasacc(const unsigned char *g);

extern const unsigned char *natf2utf(const char *str, const char *end, int rx, 
				     const char *file, int lnum);

extern const unsigned char *atf_strip_base_chars(const unsigned char *u);
#endif /*ATF_H*/
