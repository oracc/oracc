/* C code produced by gperf version 3.0.3 */
/* Command-line: gperf -N sexnum -tT sexnum.g  */
/* Computed positions: -k'2-3,5' */

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

#line 1 "sexnum.g"

#include <string.h>
#include "nsa.h"

#define TOTAL_KEYWORDS 36
#define MIN_WORD_LENGTH 1
#define MAX_WORD_LENGTH 9
#define MIN_HASH_VALUE 1
#define MAX_HASH_VALUE 53
/* maximum key range = 53, duplicates = 0 */

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
      54, 54, 54, 54, 54, 54, 54, 54, 54, 54,
      54, 54, 54, 54, 54, 54, 54, 54, 54, 54,
      54, 54, 54, 54, 54, 54, 54, 54, 54, 54,
      54, 54, 54, 54, 54, 54, 54, 54, 54,  0,
      54, 54, 54, 54, 54, 54, 54, 54, 54, 54,
      20, 54, 54, 54, 54, 54, 54, 54, 54, 54,
      54, 54, 54, 54, 20, 54, 54, 54, 54, 54,
      54, 54, 54, 54, 54, 54, 54, 54, 54, 54,
      54, 54, 54, 54, 54, 54, 54, 54, 54, 54,
      54, 54, 54, 54, 54, 54, 54, 10, 54, 30,
      54, 20, 54, 20, 54, 20, 54, 54, 54, 54,
      54, 54, 54, 54, 54,  5,  0,  0, 54, 54,
      54, 54,  5, 54, 54, 54, 54, 54, 54, 54,
      54, 54, 54, 54, 54, 54, 54, 54, 54, 54,
      54, 54, 54, 54, 54, 54, 54, 54, 54, 54,
      54, 54, 54, 54, 54, 54, 54, 54, 54, 54,
      54,  0, 54, 54, 54, 54, 54, 54, 54, 54,
      54, 54, 54, 54, 54, 54, 54, 54, 54, 54,
      54, 54, 54, 54, 54, 54, 54, 54, 54, 54,
      54, 54, 54, 54, 54, 54, 54,  0, 54, 54,
      54, 54, 54, 54, 54, 54, 54, 54, 54, 54,
      54, 54, 54, 54, 54, 54, 54, 54, 54, 54,
      54, 54, 54, 54, 54, 54, 20, 54, 54, 54,
      54, 54, 54, 54, 54, 54, 54, 54, 54, 54,
      54, 54, 54, 54, 54, 54, 54, 54, 54, 54,
      54, 54, 54, 54, 54, 54
    };
  register int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[4]];
      /*FALLTHROUGH*/
      case 4:
      case 3:
        hval += asso_values[(unsigned char)str[2]];
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
#ifdef __GNUC_STDC_INLINE__
__attribute__ ((__gnu_inline__))
#endif
#endif
struct nsa_sex_tab *
sexnum (str, len)
     register const char *str;
     register unsigned int len;
{
  static struct nsa_sex_tab wordlist[] =
    {
      {""},
#line 13 "sexnum.g"
      {"u",	    nsa_sex_u,	    10},
      {""},
#line 15 "sexnum.g"
      {"a\305\241",	    nsa_sex_asz,	1},
      {""},
#line 33 "sexnum.g"
      {"a\305\241@t",	    nsa_sex_asz,	1},
      {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 23 "sexnum.g"
      {"asz",	nsa_sex_asz,	1},
#line 9 "sexnum.g"
      {"\305\241ar",	nsa_sex_szar,	3600},
#line 42 "sexnum.g"
      {"asz@t",	nsa_sex_asz,	1},
#line 8 "sexnum.g"
      {"\305\241ar'u",	nsa_sex_szaru,	36000},
      {""},
#line 25 "sexnum.g"
      {"\305\241ar'u@c",	nsa_sex_szaru,	36000},
#line 18 "sexnum.g"
      {"szar",	nsa_sex_szar,	3600},
      {""},
#line 17 "sexnum.g"
      {"szar'u",	nsa_sex_szaru,	36000},
      {""},
#line 35 "sexnum.g"
      {"szar'u@c",	nsa_sex_szaru,	36000},
#line 14 "sexnum.g"
      {"di\305\241",	nsa_sex_disz,	1},
#line 10 "sexnum.g"
      {"ge\305\241u",	nsa_sex_geszu,	600},
#line 11 "sexnum.g"
      {"ge\305\241'u",	nsa_sex_geszu,	600},
#line 27 "sexnum.g"
      {"ge\305\241u@c",	nsa_sex_geszu,	600},
#line 28 "sexnum.g"
      {"ge\305\241'u@c",	nsa_sex_geszu,	600},
#line 22 "sexnum.g"
      {"disz",	nsa_sex_disz,	1},
#line 19 "sexnum.g"
      {"geszu",	nsa_sex_geszu,	600},
#line 20 "sexnum.g"
      {"gesz'u",	nsa_sex_geszu,	600},
#line 38 "sexnum.g"
      {"geszu@c",	nsa_sex_geszu,	600},
#line 37 "sexnum.g"
      {"gesz'u@c",	nsa_sex_geszu,	600},
      {""},
#line 32 "sexnum.g"
      {"a\305\241@c",	    nsa_sex_asz,	1},
#line 26 "sexnum.g"
      {"\305\241ar@c",	nsa_sex_szar,	3600},
#line 7 "sexnum.g"
      {"\305\241argal",	nsa_sex_szargal,	216000},
      {""},
#line 24 "sexnum.g"
      {"\305\241argal@c",	nsa_sex_szargal,	216000},
      {""},
#line 36 "sexnum.g"
      {"szar@c",	nsa_sex_szar,	3600},
#line 16 "sexnum.g"
      {"szargal",	nsa_sex_szargal,	216000},
      {""},
#line 34 "sexnum.g"
      {"szargal@c",	nsa_sex_szargal,	216000},
#line 41 "sexnum.g"
      {"asz@c",	nsa_sex_asz,	1},
#line 31 "sexnum.g"
      {"di\305\241@c",	nsa_sex_disz,	1},
#line 12 "sexnum.g"
      {"ge\305\241\342\202\202",	nsa_sex_gesz,	60},
      {""},
#line 29 "sexnum.g"
      {"ge\305\241\342\202\202@c",	nsa_sex_gesz,	60},
#line 21 "sexnum.g"
      {"gesz2",	nsa_sex_gesz,	60},
#line 40 "sexnum.g"
      {"disz@c",	nsa_sex_disz,	1},
#line 39 "sexnum.g"
      {"gesz2@c",	nsa_sex_gesz,	60},
#line 30 "sexnum.g"
      {"u@c",	    nsa_sex_u,	    10}
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
#line 43 "sexnum.g"

