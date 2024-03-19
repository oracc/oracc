/* ANSI-C code produced by gperf version 3.1 */
/* Command-line: gperf -N periodmap -tT periodmap.g  */
/* Computed positions: -k'1,6,$' */

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
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gperf@gnu.org>."
#endif

#line 1 "periodmap.g"

#include <string.h>
#include "periodmap.h"

#define TOTAL_KEYWORDS 51
#define MIN_WORD_LENGTH 4
#define MAX_WORD_LENGTH 27
#define MIN_HASH_VALUE 4
#define MAX_HASH_VALUE 103
/* maximum key range = 100, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
hash (register const char *str, register size_t len)
{
  static unsigned char asso_values[] =
    {
      104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
      104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
      104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
      104, 104,   0, 104, 104, 104, 104, 104, 104, 104,
      104,  45, 104, 104, 104,   5, 104, 104,  25, 104,
      104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
      104, 104, 104, 104, 104,  10,  15, 104, 104,   0,
        5, 104,  30,   0, 104, 104,   5,   0,  20,  30,
       40, 104,   0,  50,   0,   0,  15, 104, 104, 104,
      104, 104, 104, 104, 104, 104, 104,   5,  35,  45,
        0,   0, 104,   0,  16,   5, 104,  30,   0,   5,
        0, 104,   0, 104,  45,   5,  20,   5, 104,  20,
      104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
      104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
      104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
      104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
      104,  55, 104, 104, 104, 104, 104, 104, 104, 104,
      104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
      104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
      104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
      104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
      104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
      104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
      104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
      104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
      104, 104, 104, 104, 104, 104
    };
  register unsigned int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[5]];
      /*FALLTHROUGH*/
      case 5:
      case 4:
      case 3:
      case 2:
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval + asso_values[(unsigned char)str[len - 1]];
}

struct periodmap *
periodmap (register const char *str, register size_t len)
{
  static struct periodmap wordlist[] =
    {
      {""}, {""}, {""}, {""},
#line 14 "periodmap.g"
      {"ED I", PC,},
#line 52 "periodmap.g"
      {"Roman", LS,},
#line 31 "periodmap.g"
      {"Ur III", UR,},
#line 15 "periodmap.g"
      {"ED I-II", PC,},
#line 12 "periodmap.g"
      {"Uruk III", PC,},
#line 23 "periodmap.g"
      {"Ebla", Eb,},
      {""}, {""},
#line 17 "periodmap.g"
      {"ED IIIa", EDa,},
#line 39 "periodmap.g"
      {"Urartian", LS,},
#line 35 "periodmap.g"
      {"Middle Hittite", LS,},
#line 36 "periodmap.g"
      {"Middle Assyrian", LS,},
      {""},
#line 37 "periodmap.g"
      {"Middle Babylonian", LS,},
      {""},
#line 16 "periodmap.g"
      {"Early Dynastic I-II", PC,},
#line 32 "periodmap.g"
      {"Early Old Babylonian", OB,},
#line 22 "periodmap.g"
      {"Third Millennium", EDb,},
#line 11 "periodmap.g"
      {"Uruk IV", PC,},
      {""},
#line 18 "periodmap.g"
      {"Early Dynastic IIIa", EDa,},
#line 47 "periodmap.g"
      {"Achaemenid", LS,},
#line 41 "periodmap.g"
      {"First Millennium", LS,},
#line 56 "periodmap.g"
      {"Unknown", XX,},
      {""},
#line 53 "periodmap.g"
      {"Uncertain", XX,},
#line 26 "periodmap.g"
      {"Lagash II", LA,},
#line 46 "periodmap.g"
      {"Neo-Elamite", LS,},
#line 57 "periodmap.g"
      {"unknown", XX},
      {""},
#line 54 "periodmap.g"
      {"uncertain", XX,},
#line 43 "periodmap.g"
      {"Late Babylonian", LS,},
#line 10 "periodmap.g"
      {"Uruk V", PC,},
#line 45 "periodmap.g"
      {"Neo-Assyrian", LS,},
#line 28 "periodmap.g"
      {"Harappan", UR,},
#line 44 "periodmap.g"
      {"Neo-Babylonian", LS,},
#line 8 "periodmap.g"
      {"Egyptian 0", PC,},
#line 25 "periodmap.g"
      {"Old Elamite", OAk,},
#line 19 "periodmap.g"
      {"ED IIIb", EDb,},
      {""},
#line 42 "periodmap.g"
      {"Neo/Late Babylonian", LS,},
      {""}, {""},
#line 33 "periodmap.g"
      {"Old Assyrian", OB,},
      {""},
#line 34 "periodmap.g"
      {"Old Babylonian", OB,},
      {""}, {""},
#line 48 "periodmap.g"
      {"Persian", LS,},
#line 51 "periodmap.g"
      {"Parthian", LS,},
#line 21 "periodmap.g"
      {"Early Dynastic IIIb", EDb,},
      {""}, {""},
#line 29 "periodmap.g"
      {"Hittite", LS,},
#line 13 "periodmap.g"
      {"Proto-Elamite", PC,},
#line 20 "periodmap.g"
      {"Early Dynastic", EDb,},
      {""}, {""},
#line 55 "periodmap.g"
      {"unclear", XX,},
      {""},
#line 30 "periodmap.g"
      {"Linear Elamite", UR,},
      {""}, {""},
#line 9 "periodmap.g"
      {"Archaic", PC,},
      {""},
#line 27 "periodmap.g"
      {"Laga\305\241 II", LA,},
      {""}, {""},
#line 24 "periodmap.g"
      {"Old Akkadian", OAk,},
      {""},
#line 38 "periodmap.g"
      {"Standard Babylonian", LS,},
      {""}, {""},
#line 40 "periodmap.g"
      {"Urartian (First Millennium)", LS,},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 49 "periodmap.g"
      {"Hellenistic", LS,},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 7 "periodmap.g"
      {"Pre-Writing", PC,},
      {""}, {""}, {""}, {""}, {""}, {""},
#line 50 "periodmap.g"
      {"Seleucid", LS,}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register unsigned int key = hash (str, len);

      if (key <= MAX_HASH_VALUE)
        {
          register const char *s = wordlist[key].name;

          if (*str == *s && !strcmp (str + 1, s + 1))
            return &wordlist[key];
        }
    }
  return 0;
}
