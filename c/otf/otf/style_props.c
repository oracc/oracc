/* C code produced by gperf version 3.0.3 */
/* Command-line: gperf -N style_props -tT style_props.g  */
/* Computed positions: -k'4,12,22,25,$' */

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

#line 1 "style_props.g"

#include <string.h>
#include "cdt.h"

#define TOTAL_KEYWORDS 126
#define MIN_WORD_LENGTH 8
#define MAX_WORD_LENGTH 35
#define MIN_HASH_VALUE 13
#define MAX_HASH_VALUE 344
/* maximum key range = 332, duplicates = 0 */

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
  static unsigned short asso_values[] =
    {
      345, 345, 345, 345, 345, 345, 345, 345, 345, 345,
      345, 345, 345, 345, 345, 345, 345, 345, 345, 345,
      345, 345, 345, 345, 345, 345, 345, 345, 345, 345,
      345, 345, 345, 345, 345, 345, 345, 345, 345, 345,
      345, 345, 345, 345, 345, 115, 345, 345, 345, 345,
      345, 345, 345, 345, 345, 345, 345, 345, 345, 345,
      345, 345, 345, 345, 345, 345, 345, 345, 345, 345,
      345, 345, 345, 345, 345, 345, 345, 345, 345, 345,
      345, 345, 345, 345, 345, 345, 345, 345, 345, 345,
      345, 345, 345, 345, 345, 345, 345,  80, 110,  40,
       70,  15,  60,   0,  85,  17, 345,  70,   0, 115,
        0,  65,  60, 345,  35,  20,   0,  65, 345,  90,
       65,  95, 345, 345, 345, 345, 345, 345, 345, 345,
      345, 345, 345, 345, 345, 345, 345, 345, 345, 345,
      345, 345, 345, 345, 345, 345, 345, 345, 345, 345,
      345, 345, 345, 345, 345, 345, 345, 345, 345, 345,
      345, 345, 345, 345, 345, 345, 345, 345, 345, 345,
      345, 345, 345, 345, 345, 345, 345, 345, 345, 345,
      345, 345, 345, 345, 345, 345, 345, 345, 345, 345,
      345, 345, 345, 345, 345, 345, 345, 345, 345, 345,
      345, 345, 345, 345, 345, 345, 345, 345, 345, 345,
      345, 345, 345, 345, 345, 345, 345, 345, 345, 345,
      345, 345, 345, 345, 345, 345, 345, 345, 345, 345,
      345, 345, 345, 345, 345, 345, 345, 345, 345, 345,
      345, 345, 345, 345, 345, 345, 345, 345, 345, 345,
      345, 345, 345, 345, 345, 345
    };
  register int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[24]];
      /*FALLTHROUGH*/
      case 24:
      case 23:
      case 22:
        hval += asso_values[(unsigned char)str[21]];
      /*FALLTHROUGH*/
      case 21:
      case 20:
      case 19:
      case 18:
      case 17:
      case 16:
      case 15:
      case 14:
      case 13:
      case 12:
        hval += asso_values[(unsigned char)str[11]];
      /*FALLTHROUGH*/
      case 11:
      case 10:
      case 9:
      case 8:
      case 7:
      case 6:
      case 5:
      case 4:
        hval += asso_values[(unsigned char)str[3]];
        break;
    }
  return hval + asso_values[(unsigned char)str[len - 1]];
}

#ifdef __GNUC__
__inline
#ifdef __GNUC_STDC_INLINE__
__attribute__ ((__gnu_inline__))
#endif
#endif
struct style_prop_fam *
style_props (str, len)
     register const char *str;
     register unsigned int len;
{
  static struct style_prop_fam wordlist[] =
    {
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""},
#line 44 "style_props.g"
      {"fo:text-align", sp_paragraph},
#line 31 "style_props.g"
      {"fo:line-height", sp_paragraph},
      {""}, {""}, {""},
#line 43 "style_props.g"
      {"fo:text-align-last", sp_paragraph},
      {""}, {""},
#line 67 "style_props.g"
      {"style:font-name-asian", sp_text},
#line 49 "style_props.g"
      {"style:auto-text-indent", sp_paragraph},
      {""}, {""}, {""},
#line 29 "style_props.g"
      {"fo:language", sp_text},
      {""},
#line 110 "style_props.g"
      {"style:text-line-through-text", sp_text},
#line 45 "style_props.g"
      {"fo:text-indent", sp_paragraph},
#line 69 "style_props.g"
      {"style:font-name", sp_text},
#line 128 "style_props.g"
      {"text:condition", sp_text},
#line 97 "style_props.g"
      {"style:script-type", sp_text},
#line 127 "style_props.g"
      {"style:writing-mode", sp_paragraph},
#line 96 "style_props.g"
      {"style:register-true", sp_paragraph},
      {""}, {""}, {""},
#line 92 "style_props.g"
      {"style:line-spacing", sp_paragraph},
#line 78 "style_props.g"
      {"style:font-size-rel", sp_text},
      {""},
#line 74 "style_props.g"
      {"style:font-size-asian", sp_text},
#line 79 "style_props.g"
      {"style:font-style-asian", sp_text},
#line 112 "style_props.g"
      {"style:text-line-through-type", sp_text},
      {""}, {""}, {""}, {""}, {""},
#line 111 "style_props.g"
      {"style:text-line-through-text-style", sp_text},
#line 106 "style_props.g"
      {"style:text-emphasize", sp_text},
#line 118 "style_props.g"
      {"style:text-scale", sp_text},
#line 93 "style_props.g"
      {"style:page-number", sp_paragraph},
#line 94 "style_props.g"
      {"style:protect", sp_section},
#line 56 "style_props.g"
      {"style:country-asian", sp_text},
#line 89 "style_props.g"
      {"style:letter-kerning", sp_text},
#line 83 "style_props.g"
      {"style:font-style-name", sp_text},
      {""},
#line 60 "style_props.g"
      {"style:font-charset", sp_text},
      {""},
#line 125 "style_props.g"
      {"style:vertical-align", sp_paragraph},
      {""}, {""},
#line 52 "style_props.g"
      {"style:border-line-width-left", sp_paragraph},
#line 109 "style_props.g"
      {"style:text-line-through-style", sp_text},
#line 76 "style_props.g"
      {"style:font-size-rel-asian", sp_text},
      {""}, {""}, {""},
#line 66 "style_props.g"
      {"style:font-independent-line-spacing", sp_paragraph},
#line 41 "style_props.g"
      {"fo:padding", sp_paragraph},
      {""}, {""},
#line 104 "style_props.g"
      {"style:text-combine", sp_text},
#line 21 "style_props.g"
      {"fo:font-weight", sp_text},
#line 39 "style_props.g"
      {"fo:padding-left", sp_paragraph},
      {""},
#line 30 "style_props.g"
      {"fo:letter-spacing", sp_text},
#line 100 "style_props.g"
      {"style:tab-stop-distance", sp_paragraph},
#line 115 "style_props.g"
      {"style:text-position", sp_text},
      {""},
#line 58 "style_props.g"
      {"style:font-charset-asian", sp_text},
#line 70 "style_props.g"
      {"style:font-pitch-asian", sp_text},
#line 15 "style_props.g"
      {"fo:color", sp_text},
      {""}, {""}, {""}, {""},
#line 19 "style_props.g"
      {"fo:font-style", sp_text},
      {""},
#line 116 "style_props.g"
      {"style:text-rotation-angle", sp_text},
      {""},
#line 20 "style_props.g"
      {"fo:font-variant", sp_text},
      {""}, {""},
#line 37 "style_props.g"
      {"fo:orphans", sp_paragraph},
      {""}, {""},
#line 114 "style_props.g"
      {"style:text-outline", sp_text},
#line 53 "style_props.g"
      {"style:border-line-width-right", sp_paragraph},
#line 87 "style_props.g"
      {"style:language-asian", sp_text},
      {""},
#line 18 "style_props.g"
      {"fo:font-size", sp_text},
#line 68 "style_props.g"
      {"style:font-name-complex", sp_text},
#line 107 "style_props.g"
      {"style:text-line-through-color", sp_text},
      {""}, {""}, {""}, {""},
#line 80 "style_props.g"
      {"style:font-style-complex", sp_text},
      {""},
#line 40 "style_props.g"
      {"fo:padding-right", sp_paragraph},
#line 73 "style_props.g"
      {"style:font-relief", sp_text},
      {""}, {""},
#line 101 "style_props.g"
      {"style:text-autospace", sp_paragraph},
#line 131 "style_props.g"
      {"text:line-number", sp_paragraph},
      {""}, {""},
#line 48 "style_props.g"
      {"fo:widows", sp_paragraph},
#line 122 "style_props.g"
      {"style:text-underline-type", sp_text},
#line 57 "style_props.g"
      {"style:country-complex", sp_text},
#line 55 "style_props.g"
      {"style:border-line-width-top", sp_paragraph},
#line 75 "style_props.g"
      {"style:font-size-complex", sp_text},
#line 35 "style_props.g"
      {"fo:margin", sp_paragraph},
      {""},
#line 121 "style_props.g"
      {"style:text-underline-style", sp_text},
#line 22 "style_props.g"
      {"fo:hyphenate", sp_text},
      {""},
#line 102 "style_props.g"
      {"style:text-blinking", sp_text},
#line 117 "style_props.g"
      {"style:text-rotation-scale", sp_text},
#line 91 "style_props.g"
      {"style:line-height-at-least", sp_paragraph},
#line 47 "style_props.g"
      {"fo:text-transform", sp_text},
      {""},
#line 42 "style_props.g"
      {"fo:padding-top", sp_paragraph},
      {""},
#line 27 "style_props.g"
      {"fo:keep-together", sp_paragraph},
      {""}, {""},
#line 9 "style_props.g"
      {"fo:border-left", sp_paragraph},
      {""}, {""},
#line 10 "style_props.g"
      {"fo:border-right", sp_paragraph},
#line 54 "style_props.g"
      {"style:border-line-width", sp_paragraph},
#line 33 "style_props.g"
      {"fo:margin-left", sp_paragraph},
#line 16 "style_props.g"
      {"fo:country", sp_text},
#line 63 "style_props.g"
      {"style:font-family-generic-asian", sp_text},
#line 34 "style_props.g"
      {"fo:margin-right", sp_paragraph},
#line 126 "style_props.g"
      {"style:writing-mode-automatic", sp_paragraph},
#line 71 "style_props.g"
      {"style:font-pitch-complex", sp_text},
      {""}, {""},
#line 132 "style_props.g"
      {"text:number-lines", sp_paragraph},
      {""},
#line 11 "style_props.g"
      {"fo:border", sp_paragraph},
      {""}, {""}, {""},
#line 108 "style_props.g"
      {"style:text-line-through-mode", sp_text},
#line 13 "style_props.g"
      {"fo:break-after", sp_paragraph},
      {""},
#line 72 "style_props.g"
      {"style:font-pitch", sp_text},
      {""},
#line 61 "style_props.g"
      {"style:font-family-asian", sp_text},
#line 7 "style_props.g"
      {"fo:background-color", sp_paragraph},
#line 99 "style_props.g"
      {"style:snap-to-layout-grid", sp_paragraph},
      {""}, {""}, {""}, {""}, {""}, {""},
#line 28 "style_props.g"
      {"fo:keep-with-next", sp_paragraph},
      {""},
#line 46 "style_props.g"
      {"fo:text-shadow", sp_text},
      {""}, {""},
#line 77 "style_props.g"
      {"style:font-size-rel-complex", sp_text},
      {""},
#line 81 "style_props.g"
      {"style:font-style-name-asian", sp_text},
#line 65 "style_props.g"
      {"style:font-family-generic", sp_text},
#line 23 "style_props.g"
      {"fo:hyphenation-keep", sp_paragraph},
      {""}, {""}, {""}, {""},
#line 17 "style_props.g"
      {"fo:font-family", sp_text},
      {""}, {""}, {""}, {""}, {""},
#line 98 "style_props.g"
      {"style:shadow", sp_paragraph},
#line 84 "style_props.g"
      {"style:font-weight-asian", sp_text},
#line 124 "style_props.g"
      {"style:use-window-font-color", sp_text},
#line 130 "style_props.g"
      {"text:dont-balance-text-columns", sp_section},
#line 90 "style_props.g"
      {"style:line-break", sp_paragraph},
      {""}, {""}, {""},
#line 14 "style_props.g"
      {"fo:break-before", sp_paragraph},
      {""},
#line 129 "style_props.g"
      {"text:display", sp_text},
      {""},
#line 113 "style_props.g"
      {"style:text-line-through-width", sp_text},
      {""}, {""},
#line 95 "style_props.g"
      {"style:punctuation-wrap", sp_paragraph},
      {""}, {""}, {""}, {""}, {""},
#line 64 "style_props.g"
      {"style:font-family-generic-complex", sp_text},
#line 50 "style_props.g"
      {"style:background-transparency", sp_paragraph},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 119 "style_props.g"
      {"style:text-underline-color", sp_text},
#line 88 "style_props.g"
      {"style:language-complex", sp_text},
      {""}, {""},
#line 120 "style_props.g"
      {"style:text-underline-mode", sp_text},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""},
#line 12 "style_props.g"
      {"fo:border-top", sp_paragraph},
      {""}, {""}, {""}, {""},
#line 36 "style_props.g"
      {"fo:margin-top", sp_paragraph},
      {""}, {""}, {""},
#line 103 "style_props.g"
      {"style:text-combine-end-char", sp_text},
      {""}, {""}, {""},
#line 59 "style_props.g"
      {"style:font-charset-complex", sp_text},
      {""}, {""}, {""}, {""},
#line 123 "style_props.g"
      {"style:text-underline-width", sp_text},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 62 "style_props.g"
      {"style:font-family-complex", sp_text},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""},
#line 51 "style_props.g"
      {"style:border-line-width-bottom", sp_paragraph},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 105 "style_props.g"
      {"style:text-combine-start-char", sp_text},
      {""}, {""},
#line 38 "style_props.g"
      {"fo:padding-bottom", sp_paragraph},
      {""}, {""},
#line 85 "style_props.g"
      {"style:font-weight-complex", sp_text},
#line 8 "style_props.g"
      {"fo:border-bottom", sp_paragraph},
      {""}, {""},
#line 24 "style_props.g"
      {"fo:hyphenation-ladder-count", sp_paragraph},
      {""},
#line 32 "style_props.g"
      {"fo:margin-bottom", sp_paragraph},
      {""}, {""}, {""},
#line 86 "style_props.g"
      {"style:justify-single-word", sp_paragraph},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""},
#line 26 "style_props.g"
      {"fo:hyphenation-remain-char-count", sp_text},
      {""}, {""},
#line 25 "style_props.g"
      {"fo:hyphenation-push-char-count", sp_text},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""},
#line 82 "style_props.g"
      {"style:font-style-name-complex", sp_text}
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
