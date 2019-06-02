#ifndef _SELIB_H
#define _SELIB_H

#include <stdlib.h>
#include <stdio.h>
#include "types.h"
#include "ce.h"
#include "v2.h"

struct est;

#define KM_ATF2UTF   0x001 /* map ATF to Unicode */
#define KM_GRAPHEME  0x002 /* use special ATF mapping for graphemes */
#define KM_REDUCE    0x004 /* remove non alpha-numeric and leading zeroes */
#define KM_FOLD      0x008 /* fold case */
#define KM_2VOWEL    0x010 /* eliminate accents on vowels */
#define KM_STEM      0x020 /* use the stemmer */
#define KM_HYPHOK    0x040 /* leave hyphens in KM_REDUCE */

#define langs(a)  attr_by_name((a),"langs")
#define ref(a)    attr_by_name((a),"ref")
#define xml_id(a) attr_by_name((a),"xml:id")
#define xml_lang(a) attr_by_name((a),"xml:lang")

extern const char *curr_se_lang;

extern int use_unicode;
extern int v2;

const char *attr_by_name(const char **atts,const char *name);
extern void ce_cfg(const char *project, const char *index,const char *ce_tag, 
		   const char *ce_ext, enum ce_files ce_type, 
		   const char **proxies);
extern FILE *create_mangle_tab(const char *project, const char *index);
extern const unsigned char *keymangler(const unsigned char *key, int manglerules, char *f, size_t l, 
				       struct est *estp, const char *prefix);
extern void km_use_stemmer(void);
extern void km_end_stemmer(void);
int lang_mask(const char **atts);
void loc8(const char *id, int word, int lmask, struct location8 *l8p);
void wid2loc8(const char *id, const char *lang, struct location8 *l8p);
const char *se_dir(const char *project, const char *xname);
const char *se_file(const char *project, const char *xname, const char *fname);

void est_add(const unsigned char *key, struct est *estp);
void est_dump(struct est *estp);
struct est *est_init(const char *project, const char *index);
void est_term(struct est *estp);

#define PADDED_GRAPHEME_LEN 64

#endif /*_SELIB_H*/
