/* C code produced by gperf version 3.0.3 */
/* Command-line: gperf -N periodmap -tT periodmap.g  */
/* Computed positions: -k'1,5,7,19' */

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

#line 1 "periodmap.g"

#include <string.h>
#include "periodmap.h"

#define TOTAL_KEYWORDS 28
#define MIN_WORD_LENGTH 4
#define MAX_WORD_LENGTH 20
#define MIN_HASH_VALUE 4
#define MAX_HASH_VALUE 52
/* maximum key range = 49, duplicates = 0 */

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
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53,  0, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 23, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 10,  5, 53,  5,  0,
       0, 53,  0,  0, 53, 53, 28,  0, 10,  0,
      53, 53, 53, 53, 53,  0,  3, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53,  0, 15,  8,
      53,  0, 53, 53, 53,  0, 53, 30,  0, 53,
       5,  0, 53, 53,  0,  5,  0, 20, 53, 53,
      53,  0, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 10, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53
    };
  register int hval = (int)len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[18]];
      /*FALLTHROUGH*/
      case 18:
      case 17:
      case 16:
      case 15:
      case 14:
      case 13:
      case 12:
      case 11:
      case 10:
      case 9:
      case 8:
      case 7:
        hval += asso_values[(unsigned char)str[6]];
      /*FALLTHROUGH*/
      case 6:
      case 5:
        hval += asso_values[(unsigned char)str[4]];
      /*FALLTHROUGH*/
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
#ifdef __GNUC_STDC_INLINE__
__attribute__ ((__gnu_inline__))
#endif
#endif
struct periodmap *
periodmap (str, len)
     register const char *str;
     register unsigned int len;
{
  static struct periodmap wordlist[] =
    {
      {""}, {""}, {""}, {""},
#line 16 "periodmap.g"
      {"Ebla", Eb,},
      {""},
#line 20 "periodmap.g"
      {"Ur III", UR,},
#line 11 "periodmap.g"
      {"ED IIIa", EDa,},
#line 8 "periodmap.g"
      {"Uruk III", PC,},
#line 31 "periodmap.g"
      {"Uncertain", XX,},
#line 9 "periodmap.g"
      {"Uruk IV", PC,},
#line 30 "periodmap.g"
      {"Hellenistic", LS,},
#line 33 "periodmap.g"
      {"Unknown", XX,},
      {""},
#line 24 "periodmap.g"
      {"Middle Hittite", LS,},
#line 25 "periodmap.g"
      {"Middle Assyrian", LS,},
#line 27 "periodmap.g"
      {"First Millennium", LS,},
#line 26 "periodmap.g"
      {"Middle Babylonian", LS,},
      {""},
#line 13 "periodmap.g"
      {"Early Dynastic", EDb,},
#line 21 "periodmap.g"
      {"Early Old Babylonian", OB,},
      {""},
#line 14 "periodmap.g"
      {"ED IIIb", EDb,},
      {""},
#line 12 "periodmap.g"
      {"Early Dynastic IIIa", EDa,},
#line 7 "periodmap.g"
      {"Archaic", PC,},
      {""},
#line 22 "periodmap.g"
      {"Old Assyrian", OB,},
      {""},
#line 32 "periodmap.g"
      {"uncertain", XX,},
#line 10 "periodmap.g"
      {"ED I-II", PC,},
      {""},
#line 34 "periodmap.g"
      {"unknown", XX},
      {""},
#line 23 "periodmap.g"
      {"Old Babylonian", OB,},
      {""}, {""},
#line 29 "periodmap.g"
      {"Neo-Assyrian", LS,},
      {""},
#line 15 "periodmap.g"
      {"Early Dynastic IIIb", EDb,},
      {""}, {""},
#line 18 "periodmap.g"
      {"Lagash II", LA,},
      {""},
#line 28 "periodmap.g"
      {"Neo-Babylonian", LS,},
      {""}, {""},
#line 19 "periodmap.g"
      {"Laga\305\241 II", LA,},
      {""}, {""}, {""}, {""},
#line 17 "periodmap.g"
      {"Old Akkadian", OAk,}
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
