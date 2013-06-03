/* C code produced by gperf version 3.0.3 */
/* Command-line: gperf -N lang_atf -tT lang_atf.g  */
/* Computed positions: -k'1-2' */

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

#line 1 "lang_atf.g"

#include <string.h>
#include "lang.h"

#define TOTAL_KEYWORDS 27
#define MIN_WORD_LENGTH 1
#define MAX_WORD_LENGTH 4
#define MIN_HASH_VALUE 1
#define MAX_HASH_VALUE 42
/* maximum key range = 42, duplicates = 0 */

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
      43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
      43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
      43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
      43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
      43, 43, 43, 43, 43, 43, 43, 43, 19,  1,
      28, 18,  8, 43, 43, 43, 43, 43, 43, 43,
      43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
      43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
      43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
      43, 43, 43, 43, 43, 43, 43, 10,  5,  3,
      43,  0, 43,  3, 30, 43, 43, 43, 43, 30,
      20, 10, 43, 43, 43,  5, 43, 15, 43, 43,
      25,  3, 43, 43, 43, 43, 43, 43, 43, 43,
      43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
      43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
      43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
      43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
      43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
      43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
      43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
      43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
      43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
      43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
      43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
      43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
      43, 43, 43, 43, 43, 43
    };
  register int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[1]];
      /*FALLTHROUGH*/
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval;
}

#ifdef __GNUC__
__inline
#ifdef __GNUC_STDC_INLINE__
__attribute__ ((__gnu_inline__))
#endif
#endif
struct langatf *
lang_atf (str, len)
     register const char *str;
     register unsigned int len;
{
  static struct langatf wordlist[] =
    {
      {""},
#line 15 "lang_atf.g"
      {"e",	"sux-x-emesal"},
#line 8 "lang_atf.g"
      {"1",	"sux-x-gloss"},
      {""},
#line 12 "lang_atf.g"
      {"g",	"akk"},
#line 16 "lang_atf.g"
      {"eg",	"sux"},
#line 19 "lang_atf.g"
      {"s",	"sux"},
#line 17 "lang_atf.g"
      {"es",	"sux-x-emesal"},
      {""},
#line 11 "lang_atf.g"
      {"4",	"akk-x-neobab-937-x-dumusza"},
#line 23 "lang_atf.g"
      {"sy", "sux-x-syllabic"},
#line 14 "lang_atf.g"
      {"a",	"akk"},
#line 25 "lang_atf.g"
      {"sb",	"akk-x-stdbab"},
      {""},
#line 33 "lang_atf.g"
      {"eakk",	"akk-x-earakk"},
#line 24 "lang_atf.g"
      {"ca",	"akk-x-conakk"},
#line 21 "lang_atf.g"
      {"u",	"sux-x-udgalnun"},
#line 31 "lang_atf.g"
      {"ob",	"akk-x-oldbab"},
      {""},
#line 10 "lang_atf.g"
      {"3",	"akk-936"},
#line 7 "lang_atf.g"
      {"0",	"sux-x-normal"},
#line 13 "lang_atf.g"
      {"n",	"akk-949"},
#line 30 "lang_atf.g"
      {"oa",	"akk-x-oldass"},
#line 20 "lang_atf.g"
      {"sux",	"sux"},
#line 32 "lang_atf.g"
      {"oakk",	"akk-x-oldakk"},
      {""},
#line 22 "lang_atf.g"
      {"x",	"qcu"},
#line 27 "lang_atf.g"
      {"nb",	"akk-x-neobab"},
      {""},
#line 9 "lang_atf.g"
      {"2",	"akk-935"},
      {""},
#line 18 "lang_atf.g"
      {"h",	"hit"},
#line 26 "lang_atf.g"
      {"na",	"akk-x-neoass"},
      {""}, {""}, {""}, {""},
#line 29 "lang_atf.g"
      {"mb",	"akk-x-midbab"},
      {""}, {""}, {""}, {""},
#line 28 "lang_atf.g"
      {"ma",	"akk-x-midass"}
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
