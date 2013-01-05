/* C code produced by gperf version 3.0.4 */
/* Command-line: gperf -N pqx_h_tab -tT pqx_h_tab.g  */
/* Computed positions: -k'1,10' */

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

#line 1 "pqx_h_tab.g"

#include "resolver.h"

#define TOTAL_KEYWORDS 24
#define MIN_WORD_LENGTH 3
#define MAX_WORD_LENGTH 16
#define MIN_HASH_VALUE 3
#define MAX_HASH_VALUE 48
/* maximum key range = 46, duplicates = 0 */

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
      49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
      49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
      49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
      49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
      49, 49, 49, 49, 49, 49, 49,  5, 49, 49,
      49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
      49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
      49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
      49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
      49, 49, 49, 49, 49, 49, 49, 49, 49, 10,
      49, 49, 49, 49, 10, 20, 49, 49,  5, 49,
      49,  0, 25, 49, 49, 15,  0, 49,  0, 49,
      25, 49, 49, 49, 49, 49, 49, 49, 49, 49,
      49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
      49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
      49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
      49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
      49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
      49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
      49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
      49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
      49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
      49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
      49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
      49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
      49, 49, 49, 49, 49, 49
    };
  register int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[9]];
      /*FALLTHROUGH*/
      case 9:
      case 8:
      case 7:
      case 6:
      case 5:
      case 4:
      case 3:
      case 2:
      case 1:
        hval += asso_values[(unsigned char)str[0]];
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
struct h_pqxtab *
pqx_h_tab (str, len)
     register const char *str;
     register unsigned int len;
{
  static struct h_pqxtab wordlist[] =
    {
      {""}, {""}, {""},
#line 8 "pqx_h_tab.g"
      {"tei",              h_pqx_tlit_tei},
#line 29 "pqx_h_tab.g"
      {"view", 	  h_pqx_view},
#line 18 "pqx_h_tab.g"
      {"thumb", 	    	  h_pqx_thumb},
      {""}, {""},
#line 9 "pqx_h_tab.g"
      {"translit",     	  h_pqx_tlit_html},
      {""}, {""},
#line 10 "pqx_h_tab.g"
      {"translation", 	  h_pqx_trans_html},
#line 28 "pqx_h_tab.g"
      {"translit/tei",     h_pqx_tlit_tei},
#line 7 "pqx_h_tab.g"
      {"cat", 	    	  h_pqx_cat_html},
#line 6 "pqx_h_tab.g"
      {"html",	    	  h_pqx_html},
#line 26 "pqx_h_tab.g"
      {"translation/xml",  h_pqx_trans_xml},
#line 14 "pqx_h_tab.g"
      {"translation/html", h_pqx_trans_html},
#line 24 "pqx_h_tab.g"
      {"cat/xml", 	  h_pqx_cat_xml},
#line 12 "pqx_h_tab.g"
      {"cat/html", 	  h_pqx_cat_html},
#line 19 "pqx_h_tab.g"
      {"cuneified",	  h_pqx_cuneified_html},
#line 11 "pqx_h_tab.g"
      {"score", 	    	  h_pqx_score_html},
      {""}, {""},
#line 13 "pqx_h_tab.g"
      {"translit/html",    h_pqx_tlit_html},
#line 27 "pqx_h_tab.g"
      {"score/xml", 	  h_pqx_score_html},
#line 17 "pqx_h_tab.g"
      {"image", 	    	  h_pqx_image},
#line 16 "pqx_h_tab.g"
      {"images",     	  h_pqx_images},
      {""},
#line 23 "pqx_h_tab.g"
      {"xml",		  h_pqx_xml},
#line 21 "pqx_h_tab.g"
      {"cuneified/html",	  h_pqx_cuneified_html},
#line 15 "pqx_h_tab.g"
      {"score/html", 	  h_pqx_score_html},
      {""}, {""},
#line 20 "pqx_h_tab.g"
      {"proofing",	  h_pqx_proofing_html},
      {""}, {""}, {""},
#line 25 "pqx_h_tab.g"
      {"translit/xml",     h_pqx_tlit_xml},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""},
#line 22 "pqx_h_tab.g"
      {"proofing/html",	  h_pqx_proofing_html}
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
