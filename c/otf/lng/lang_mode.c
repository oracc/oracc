/* C code produced by gperf version 3.0.3 */
/* Command-line: /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/gperf -N lang_mode -tT lang_mode.g  */
/* Computed positions: -k'1-3' */

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

#line 1 "lang_mode.g"

#include <string.h>
#include "langmode.h"

#define TOTAL_KEYWORDS 25
#define MIN_WORD_LENGTH 3
#define MAX_WORD_LENGTH 3
#define MIN_HASH_VALUE 3
#define MAX_HASH_VALUE 46
/* maximum key range = 44, duplicates = 0 */

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
     register size_t len;
{
  static unsigned char asso_values[] =
    {
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47,  5, 10,
      26, 20,  3,  1,  8, 23, 20,  0,  6, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47
    };
  return len + asso_values[(unsigned char)str[2]+1] + asso_values[(unsigned char)str[1]] + asso_values[(unsigned char)str[0]];
}

struct langmode *
lang_mode (str, len)
     register const char *str;
     register size_t len;
{
  static struct langmode wordlist[] =
    {
      {""}, {""}, {""},
#line 31 "lang_mode.g"
      {"998", m_graphemic},
      {""}, {""},
#line 29 "lang_mode.g"
      {"948", m_graphemic},
#line 25 "lang_mode.g"
      {"944", m_graphemic},
      {""},
#line 13 "lang_mode.g"
      {"904", m_graphemic},
      {""},
#line 12 "lang_mode.g"
      {"903", m_graphemic},
#line 30 "lang_mode.g"
      {"949", m_normalized},
#line 18 "lang_mode.g"
      {"918", m_graphemic},
#line 26 "lang_mode.g"
      {"945", m_graphemic},
      {""},
#line 14 "lang_mode.g"
      {"905", m_graphemic},
      {""},
#line 9 "lang_mode.g"
      {"900", m_graphemic},
#line 19 "lang_mode.g"
      {"919", m_graphemic},
      {""},
#line 15 "lang_mode.g"
      {"915", m_graphemic},
      {""},
#line 24 "lang_mode.g"
      {"938", m_graphemic},
      {""}, {""},
#line 28 "lang_mode.g"
      {"947", m_graphemic},
      {""},
#line 11 "lang_mode.g"
      {"902", m_graphemic},
#line 27 "lang_mode.g"
      {"946", m_graphemic},
      {""},
#line 21 "lang_mode.g"
      {"935", m_graphemic},
      {""},
#line 17 "lang_mode.g"
      {"917", m_graphemic},
#line 10 "lang_mode.g"
      {"901", m_graphemic},
      {""},
#line 16 "lang_mode.g"
      {"916", m_graphemic},
      {""},
#line 8 "lang_mode.g"
      {"030", m_graphemic},
#line 20 "lang_mode.g"
      {"920", m_graphemic},
      {""}, {""}, {""},
#line 23 "lang_mode.g"
      {"937", m_graphemic},
#line 7 "lang_mode.g"
      {"020", m_graphemic},
      {""},
#line 22 "lang_mode.g"
      {"936", m_graphemic}
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
