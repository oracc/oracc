/* C code produced by gperf version 3.0.4 */
/* Command-line: gperf -N blocktok -tT blocktok.g  */
/* Computed positions: -k'2-3' */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

#line 1 "blocktok.g"

#include <string.h>
#include "block.h"
#include "blocktok.h"

#define TOTAL_KEYWORDS 52
#define MIN_WORD_LENGTH 1
#define MAX_WORD_LENGTH 15
#define MIN_HASH_VALUE 1
#define MAX_HASH_VALUE 104
/* maximum key range = 104, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
hash (str, len)
     register const char *str;
     register unsigned int len;
{
  static unsigned char asso_values[] =
    {
      105, 105, 105, 105, 105, 105, 105, 105, 105, 105,
      105, 105, 105, 105, 105, 105, 105, 105, 105, 105,
      105, 105, 105, 105, 105, 105, 105, 105, 105, 105,
      105, 105, 105, 105, 105, 105, 105, 105, 105, 105,
      105, 105, 105, 105, 105, 105, 105, 105, 105,  35,
       30,   0, 105, 105, 105, 105, 105, 105, 105, 105,
      105, 105, 105, 105, 105, 105, 105, 105, 105, 105,
      105, 105, 105, 105, 105, 105, 105, 105, 105, 105,
      105, 105, 105, 105, 105, 105, 105, 105, 105, 105,
      105, 105, 105, 105, 105, 105, 105,  55,   5,   0,
       20,   0,  45,  45,  60,  35,  40,   5, 105,   0,
        0,  20,   5,  10,   5,   5, 105,  10, 105,   0,
      105,  20, 105, 105, 105, 105, 105, 105, 105, 105,
      105, 105, 105, 105, 105, 105, 105, 105, 105, 105,
      105, 105, 105, 105, 105, 105, 105, 105, 105, 105,
      105, 105, 105, 105, 105, 105, 105, 105, 105, 105,
      105, 105, 105, 105, 105, 105, 105, 105, 105, 105,
      105, 105, 105, 105, 105, 105, 105, 105, 105, 105,
      105, 105, 105, 105, 105, 105, 105, 105, 105, 105,
      105, 105, 105, 105, 105, 105, 105, 105, 105, 105,
      105, 105, 105, 105, 105, 105, 105, 105, 105, 105,
      105, 105, 105, 105, 105, 105, 105, 105, 105, 105,
      105, 105, 105, 105, 105, 105, 105, 105, 105, 105,
      105, 105, 105, 105, 105, 105, 105, 105, 105, 105,
      105, 105, 105, 105, 105, 105, 105, 105, 105, 105,
      105, 105, 105, 105, 105, 105, 105
    };
  register int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[2]+1];
      /*FALLTHROUGH*/
      case 2:
        hval += asso_values[(unsigned char)str[1]];
      /*FALLTHROUGH*/
      case 1:
        break;
    }
  return hval;
}

#ifdef __GNUC__
__inline
#if defined __GNUC_STDC_INLINE__ || defined __GNUC_GNU_INLINE__
__attribute__ ((__gnu_inline__))
#endif
#endif
struct block_token *
blocktok (str, len)
     register const char *str;
     register unsigned int len;
{
  static struct block_token wordlist[] =
    {
      {""},
#line 38 "blocktok.g"
      {"m",			e_m, "", MILESTONE, NULL, NULL, NULL},
#line 65 "blocktok.g"
      {"h3",         e_xh_h3,    NULL, TR_H3,    NULL, NULL, NULL},
#line 53 "blocktok.g"
      {"end",        e_div, "end", DIVISION, NULL, NULL, NULL},
      {""}, {""}, {""},
#line 26 "blocktok.g"
      {"reverse",	e_surface, "reverse", SURFACE, "Reverse", "Rev.", "r"},
#line 23 "blocktok.g"
      {"envelope",	e_object, "envelope", OBJECT, "Envelope", "Env.", "env"},
#line 35 "blocktok.g"
      {"seal",		e_surface, "seal", SURFACE, "Seal", "seal", NULL},
#line 18 "blocktok.g"
      {"score",  e_score, NULL, TEXT, NULL, NULL, NULL},
#line 68 "blocktok.g"
      {"endvariants",e_variants, "endvariants", DIVISION, NULL, NULL, NULL},
#line 25 "blocktok.g"
      {"obverse", 	e_surface, "obverse", SURFACE, "Obverse", "Obv.", "o"},
#line 45 "blocktok.g"
      {"sealings",	e_m, "sealers", MILESTONE, NULL, NULL, NULL},
#line 58 "blocktok.g"
      {"span",	    e_xh_span,  NULL, TR_SPAN,  NULL, NULL, NULL},
#line 21 "blocktok.g"
      {"bulla",		e_object, "bulla", OBJECT, "Bulla", "Bulla", "bulla"},
#line 24 "blocktok.g"
      {"object",	    e_object, "", OBJECT, NULL, NULL, NULL},
#line 47 "blocktok.g"
      {"summary",	e_m, "summary", MILESTONE, NULL, NULL, NULL},
#line 39 "blocktok.g"
      {"fragment",	e_m, "fragment", MILESTONE, "Fragment", "Frag.", "frg"},
#line 55 "blocktok.g"
      {"transtype",  e_xtr_translation, NULL, TR_SWITCH, NULL, NULL, NULL},
      {""},
#line 54 "blocktok.g"
      {"translation", e_xtr_translation, NULL, TR_TRANS, NULL, NULL, NULL},
#line 34 "blocktok.g"
      {"surface",	e_surface, "", SURFACE, NULL, NULL, NULL},
#line 62 "blocktok.g"
      {"h3x",        e_h,        NULL, H3,       NULL, NULL, NULL},
#line 41 "blocktok.g"
      {"body",		e_m, "body", MILESTONE, NULL, NULL, NULL},
#line 19 "blocktok.g"
      {"transliteration", e_transliteration, NULL, TEXT, NULL, NULL, NULL},
#line 37 "blocktok.g"
      {"column", 	e_column, NULL, COLUMN, NULL, NULL, NULL},
#line 50 "blocktok.g"
      {"include",    e_include,"include",MILESTONE, NULL, NULL, NULL},
#line 42 "blocktok.g"
      {"colophon",	e_m, "colophon", MILESTONE, NULL, NULL, NULL},
#line 17 "blocktok.g"
      {"composite",  e_composite, NULL, TEXT, NULL, NULL, NULL},
      {""}, {""},
#line 64 "blocktok.g"
      {"h2",         e_xh_h2,    NULL, TR_H2,    NULL, NULL, NULL},
#line 30 "blocktok.g"
      {"top",		e_surface, "top", SURFACE, "Top edge", "Top", "t.e."},
#line 59 "blocktok.g"
      {"note",	    e_xh_div,   NULL, TR_NOTE,  NULL, NULL, NULL},
      {""},
#line 31 "blocktok.g"
      {"bottom",	    e_surface, "bottom", SURFACE, "Bottom edge", "Bottom", "b.e."},
#line 63 "blocktok.g"
      {"h1",         e_xh_h1,    NULL, TR_H1,    NULL, NULL, NULL},
#line 52 "blocktok.g"
      {"div",        e_div, "div", DIVISION, NULL, NULL, NULL},
#line 27 "blocktok.g"
      {"side",	e_surface, "side", SURFACE, "Side", "Side", "s."},
      {""}, {""}, {""}, {""},
#line 57 "blocktok.g"
      {"unit",	    e_enum_top, NULL, TR_UNIT,  NULL, NULL, NULL},
      {""},
#line 36 "blocktok.g"
      {"docket",		e_surface, "docket", SURFACE, "Docket", "docket", NULL},
      {""}, {""},
#line 28 "blocktok.g"
      {"left",		e_surface, "left", SURFACE, "Left edge", "Left", "l.e."},
#line 22 "blocktok.g"
      {"prism",		e_object, "prism", OBJECT, "Prism", "Prism", "prism"},
      {""},
#line 51 "blocktok.g"
      {"referto",    e_referto,"referto",MILESTONE, NULL, NULL, NULL},
#line 61 "blocktok.g"
      {"h2x",        e_h,        NULL, H2,       NULL, NULL, NULL},
#line 49 "blocktok.g"
      {"witnesses",  e_m, "witnesses", MILESTONE, NULL, NULL, NULL},
      {""}, {""}, {""},
#line 60 "blocktok.g"
      {"h1x",        e_h,        NULL, H1,       NULL, NULL, NULL},
#line 40 "blocktok.g"
      {"cfragment",	e_m, "cfragment", MILESTONE, "Fragment", "Frag.", "frg"},
#line 56 "blocktok.g"
      {"label",	    e_enum_top, NULL, TR_LABEL, NULL, NULL, NULL},
#line 20 "blocktok.g"
      {"tablet",  	e_object, "tablet", OBJECT, NULL, NULL, NULL},
      {""}, {""},
#line 46 "blocktok.g"
      {"linecount",	e_m, "linecount", MILESTONE, NULL, NULL, NULL},
      {""}, {""},
#line 67 "blocktok.g"
      {"variant",    e_variant,  "variant", DIVISION, NULL, NULL, NULL},
#line 66 "blocktok.g"
      {"variants",	e_variants, "variants", DIVISION, NULL, NULL, NULL},
#line 44 "blocktok.g"
      {"date",		e_m, "date", MILESTONE, NULL, NULL, NULL},
      {""}, {""}, {""}, {""},
#line 43 "blocktok.g"
      {"catchline",	e_m, "catchline", MILESTONE, NULL, NULL, NULL},
      {""}, {""}, {""}, {""},
#line 33 "blocktok.g"
      {"face",		e_surface, "face", SURFACE, "Face", "face", NULL},
      {""}, {""}, {""}, {""},
#line 32 "blocktok.g"
      {"edge",		e_surface, "edge", SURFACE, "Edge", "Edge", "e."},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""},
#line 29 "blocktok.g"
      {"right",		e_surface, "right", SURFACE, "Right edge", "Right", "r.e."},
      {""}, {""}, {""},
#line 48 "blocktok.g"
      {"signature",  e_m, "signature", MILESTONE, NULL, NULL, NULL}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register const char *s = wordlist[key].name;

          if (*str == *s && !strcmp (str + 1, s + 1))
            return &wordlist[key];
        }
    }
  return 0;
}
