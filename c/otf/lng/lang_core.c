/* C code produced by gperf version 3.0.3 */
/* Command-line: gperf -N lang_core -tT lang_core.g  */
/* Computed positions: -k'1,3' */

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

#line 1 "lang_core.g"

#include <string.h>
#include "lang.h"

#define TOTAL_KEYWORDS 15
#define MIN_WORD_LENGTH 3
#define MAX_WORD_LENGTH 3
#define MIN_HASH_VALUE 3
#define MAX_HASH_VALUE 33
/* maximum key range = 31, duplicates = 0 */

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
      34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 34, 34, 34, 34,  4,  2, 15,
      34, 10, 34, 34, 15, 34, 34, 14, 34,  8,
       3, 15, 10,  5, 14,  9, 15,  0, 34, 34,
       0, 34, 34, 34, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 34, 34, 34
    };
  return len + asso_values[(unsigned char)str[2]] + asso_values[(unsigned char)str[0]];
}

#ifdef __GNUC__
__inline
#ifdef __GNUC_STDC_INLINE__
__attribute__ ((__gnu_inline__))
#endif
#endif
struct langcore *
lang_core (str, len)
     register const char *str;
     register unsigned int len;
{
  static struct langcore wordlist[] =
    {
      {""}, {""}, {""},
#line 17 "lang_core.g"
      {"xhu", "020", "xhu", m_sign, m_lang, c_xhu, LF_SAC},
      {""}, {""}, {""},
#line 20 "lang_core.g"
      {"uga", "020", "uga", m_logo, m_lang, c_uga, LF_SAC},
#line 16 "lang_core.g"
      {"qcu", "020", "qcu", m_sign, m_lang, c_qcu, LF_SAC},
      {""},
#line 18 "lang_core.g"
      {"qeb", "020", "qeb", m_sign, m_lang, c_qeb, LF_SAC},
#line 21 "lang_core.g"
      {"qpn", "999", "qpn", m_sign, m_lang, c_qpn, LF_SAC},
#line 7 "lang_core.g"
      {"sux", "020", "akk", m_sign, m_lang, c_sux, LF_BASE|LF_SAC},
#line 13 "lang_core.g"
      {"elx", "020", "sux", m_sign, m_logo, c_elx, LF_SAC},
      {""}, {""},
#line 15 "lang_core.g"
      {"qam", "020", "qam", m_sign, m_lang, c_qam, LF_SAC},
#line 19 "lang_core.g"
      {"xur", "020", "xur", m_sign, m_lang, c_xur, LF_SAC},
#line 12 "lang_core.g"
      {"qpe", "938", "qpe", m_sign, m_lang, c_qpe, 0},
      {""}, {""},
#line 8 "lang_core.g"
      {"akk", "020", "sux", m_logo, m_lang, c_akk, LF_NORM|LF_SAC},
#line 9 "lang_core.g"
      {"arc", "124", "akk", m_sign, m_lang, c_arc, 0},
#line 11 "lang_core.g"
      {"qpc", "900", "qpc", m_sign, m_lang, c_qpc, 0},
      {""}, {""}, {""}, {""},
#line 14 "lang_core.g"
      {"peo", "030", "peo", m_sign, m_lang, c_peo, 0},
      {""}, {""}, {""}, {""},
#line 10 "lang_core.g"
      {"hit", "020", "akk", m_logo, m_logo, c_hit, LF_SAC}
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
#line 22 "lang_core.g"

struct langcore *
langcore_of(const char *lang)
{
  static char base[4];
  char*b = base;
  const char *tmp;
  if ((tmp = lang))
    {
      while (*tmp && '-' != *tmp)
        {
          *b++ = *tmp++;
          if (b - base == 3)
            break;
        }
      *b = '\0';
      return lang_core(base,3);
    }
  return NULL;
}
