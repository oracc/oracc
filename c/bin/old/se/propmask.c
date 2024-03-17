/* C code produced by gperf version 3.0.3 */
/* Command-line: /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/gperf -L ANSI -N propmask -tT propmask.g  */
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

#line 1 "propmask.g"

#include <string.h>
#include "se.h"
#include "types.h"
#include "property.h"

#define TOTAL_KEYWORDS 20
#define MIN_WORD_LENGTH 1
#define MAX_WORD_LENGTH 2
#define MIN_HASH_VALUE 1
#define MAX_HASH_VALUE 57
/* maximum key range = 57, duplicates = 0 */

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
      58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
      58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
      58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
      58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
      58, 58, 58, 58, 58, 58, 58, 58, 58, 18,
      30, 58, 58, 58, 58, 58, 58, 58, 58, 58,
      58, 58, 58, 58, 58, 58, 58, 58, 23, 58,
      18, 13, 58, 58, 58, 58, 20, 10, 15, 30,
       8, 58,  3, 58, 30, 58, 10, 58, 25, 58,
      58, 58, 58, 58, 58, 58, 58, 58, 10, 58,
      58,  0, 58,  0, 58, 58, 58, 58,  5, 25,
      58, 58, 58, 58, 58, 58, 58, 58, 58,  0,
      58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
      58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
      58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
      58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
      58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
      58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
      58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
      58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
      58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
      58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
      58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
      58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
      58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
      58, 58, 58, 58, 58, 58
    };
  register unsigned int hval = len;

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

struct prop_tab *
propmask (str, len)
     register const char *str;
     register unsigned int len;
{
  static struct prop_tab wordlist[] =
    {
      {""},
#line 25 "propmask.g"
      {"g", IF_GUIDE, pg_start_column},
#line 22 "propmask.g"
      {"we", PROP_WE, pg_properties},
      {""}, {""}, {""},
#line 26 "propmask.g"
      {"l", IF_LEMMA, pg_start_column},
#line 24 "propmask.g"
      {"le", PROP_LE, pg_properties},
      {""}, {""}, {""}, {""},
#line 21 "propmask.g"
      {"wb", PROP_WB, pg_properties},
      {""}, {""}, {""}, {""},
#line 23 "propmask.g"
      {"lb", PROP_LB, pg_properties},
      {""}, {""},
#line 11 "propmask.g"
      {"RN", PROP_RN, pg_properties},
      {""},
#line 17 "propmask.g"
      {"VM", PROP_VM, pg_properties},
      {""}, {""},
#line 9 "propmask.g"
      {"PN", PROP_PN, pg_properties},
      {""},
#line 16 "propmask.g"
      {"NM", PROP_NM, pg_properties},
      {""}, {""},
#line 14 "propmask.g"
      {"GN", PROP_GN, pg_properties},
      {""},
#line 19 "propmask.g"
      {"VL", PROP_VL, pg_properties},
      {""}, {""},
#line 12 "propmask.g"
      {"FN", PROP_FN, pg_properties},
      {""},
#line 18 "propmask.g"
      {"NL", PROP_NL, pg_properties},
      {""}, {""},
#line 10 "propmask.g"
      {"DN", PROP_DN, pg_properties},
      {""},
#line 15 "propmask.g"
      {"XN", PROP_XN, pg_properties},
      {""}, {""},
#line 27 "propmask.g"
      {"m1", IF_MORPH, pg_start_column},
      {""},
#line 13 "propmask.g"
      {"TN", PROP_TN, pg_properties},
      {""}, {""}, {""}, {""},
#line 20 "propmask.g"
      {"OL", PROP_OL, pg_properties},
      {""}, {""}, {""}, {""},
#line 28 "propmask.g"
      {"m2", IF_MORPH2, pg_start_column}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      unsigned int key = hash (str, len);

      if (key <= MAX_HASH_VALUE)
        {
          register const char *s = wordlist[key].name;

          if (*str == *s && !strcmp (str + 1, s + 1))
            return &wordlist[key];
        }
    }
  return 0;
}
