/* C code produced by gperf version 3.0.3 */
/* Command-line: /Library/Developer/CommandLineTools/usr/bin/gperf -N nonxtok -tT nonxtok.g  */
/* Computed positions: -k'1,4' */

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

#line 1 "nonxtok.g"

#include <string.h>
#include "nonx.h"

#define TOTAL_KEYWORDS 54
#define MIN_WORD_LENGTH 1
#define MAX_WORD_LENGTH 10
#define MIN_HASH_VALUE 3
#define MAX_HASH_VALUE 111
/* maximum key range = 109, duplicates = 0 */

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
      112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
      112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
      112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
      112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
      112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
      112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
      112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
      112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
      112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
      112, 112, 112, 112, 112, 112, 112,  25,  45,  30,
       15,   0,  25,  25,   0,  40, 112,  60,   5,  40,
       25,  45,  50, 112,   0,  10,  20,   0, 112, 112,
      112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
      112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
      112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
      112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
      112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
      112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
      112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
      112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
      112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
      112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
      112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
      112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
      112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
      112, 112, 112, 112, 112, 112
    };
  register unsigned int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[3]];
      /*FALLTHROUGH*/
      case 3:
      case 2:
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval;
}

struct nonx_token *
nonxtok (str, len)
     register const char *str;
     register unsigned int len;
{
  static struct nonx_token wordlist[] =
    {
      {""}, {""}, {""},
#line 25 "nonxtok.g"
      {"end", 	x_extent, x_end,     "end", 1, 0},
#line 44 "nonxtok.g"
      {"edge",	x_scope,  x_surface, "edge", 0, 0},
#line 41 "nonxtok.g"
      {"right",	x_scope,  x_surface, "right edge", 0, 0},
      {""},
#line 39 "nonxtok.g"
      {"reverse",x_scope,  x_surface, "reverse", 0, 0},
#line 37 "nonxtok.g"
      {"envelope",x_scope, x_object,  "envelope",  0, 0},
#line 51 "nonxtok.g"
      {"line",	x_scope,  x_line,    "line", 0, 0},
#line 52 "nonxtok.g"
      {"lines",	x_scope,  x_line,    "line", 0, 1},
#line 56 "nonxtok.g"
      {"lacuna", x_state,  x_broken,  "missing", 0, 0},
#line 26 "nonxtok.g"
      {"literal",x_extent, x_number,   NULL, 1, 0},
      {""},
#line 28 "nonxtok.g"
      {"some",   x_extent, x_number,   "n", 1, 0},
#line 23 "nonxtok.g"
      {"start",	x_extent, x_start,   "start", 1, 0},
#line 60 "nonxtok.g"
      {"erased", x_state,  x_blank,   "erased", 0, 0},
#line 29 "nonxtok.g"
      {"several",x_extent, x_number,   "n", 1, 0},
      {""},
#line 32 "nonxtok.g"
      {"seal",	x_scope,  x_line,    "seal",  0, 0},
#line 18 "nonxtok.g"
      {"least",  x_qual, x_atleast, "at least", 0, 0,},
      {""},
#line 46 "nonxtok.g"
      {"sealing",x_scope,  x_line,    "sealing", 0, 0},
#line 42 "nonxtok.g"
      {"top",	x_scope,  x_surface, "top edge", 0, 0},
#line 21 "nonxtok.g"
      {"rest",	x_extent, x_rest,    "rest", 1, 0},
#line 68 "nonxtok.g"
      {"empty", x_empty_class, x_empty, "empty", 0,0},
#line 27 "nonxtok.g"
      {"n",      x_extent, x_number,   "n", 1, 0},
#line 15 "nonxtok.g"
      {"at",     x_ignore, x_at,  "at", 0, 0,},
#line 20 "nonxtok.g"
      {"all",	x_extent, x_all,     "all", 1, 0},
#line 40 "nonxtok.g"
      {"left",	x_scope,  x_surface, "left edge", 0, 0},
#line 19 "nonxtok.g"
      {"about",  x_qual, x_atleast, "about", 0, 0,},
#line 31 "nonxtok.g"
      {"tablet",	x_scope,  x_object,  "tablet",  0, 0},
#line 57 "nonxtok.g"
      {"effaced",x_state,  x_effaced, "effaced", 0, 0},
      {""},
#line 49 "nonxtok.g"
      {"case",	x_scope,  x_line,    "case", 0, 0},
#line 50 "nonxtok.g"
      {"cases",	x_scope,  x_line,    "case", 0, 1},
#line 47 "nonxtok.g"
      {"column",	x_scope,  x_column,  "column", 0, 0},
#line 48 "nonxtok.g"
      {"columns",x_scope,  x_column,  "column", 0, 1},
      {""}, {""}, {""},
#line 63 "nonxtok.g"
      {"single", x_extent, x_single,  "1", 0, 1},
#line 45 "nonxtok.g"
      {"surface",x_scope,  x_surface, "surface", 0, 0},
      {""}, {""},
#line 53 "nonxtok.g"
      {"space",	x_scope,  x_space,   "space", 0, 1},
#line 62 "nonxtok.g"
      {"ruling", x_rule,   x_line,    "ruling", 0, 0},
#line 16 "nonxtok.g"
      {"of",     x_ignore, x_of,  "of", 0, 0,},
      {""},
#line 61 "nonxtok.g"
      {"illegible",x_state,x_illegible,"illegible",0,0},
#line 66 "nonxtok.g"
      {"impression", x_impression, x_impress, "impression", 0,0},
#line 30 "nonxtok.g"
      {"object", x_scope,  x_object,  "object", 0, 0},
#line 38 "nonxtok.g"
      {"obverse",x_scope,  x_surface, "obverse", 0, 0},
      {""}, {""},
#line 34 "nonxtok.g"
      {"bulla",	x_scope,  x_object,  "bulla",  0, 0},
#line 58 "nonxtok.g"
      {"traces", x_state,  x_traces,  "traces", 0, 0},
#line 54 "nonxtok.g"
      {"missing",x_state,  x_broken,  "missing", 0, 0},
#line 36 "nonxtok.g"
      {"fragment",x_scope, x_object,  "fragment",  0, 0},
      {""}, {""},
#line 24 "nonxtok.g"
      {"middle", x_extent, x_middle,  "middle", 1, 0},
      {""}, {""},
#line 17 "nonxtok.g"
      {"most",   x_qual, x_atmost, "at most", 0, 0,},
#line 35 "nonxtok.g"
      {"prism",	x_scope,  x_object,  "prism",  0, 0},
#line 64 "nonxtok.g"
      {"double", x_extent, x_double,  "2", 0, 2},
      {""}, {""}, {""},
#line 67 "nonxtok.g"
      {"image", x_image_class, x_image, "image", 0,0},
#line 43 "nonxtok.g"
      {"bottom",	x_scope,  x_surface, "bottom edge", 0, 0},
      {""}, {""}, {""},
#line 59 "nonxtok.g"
      {"blank",  x_state,  x_blank,   "blank", 0, 0},
#line 65 "nonxtok.g"
      {"triple", x_extent, x_triple,  "3", 0, 3},
      {""}, {""}, {""}, {""},
#line 33 "nonxtok.g"
      {"docket",	x_scope,  x_dock,    "docket",  0, 0},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""},
#line 22 "nonxtok.g"
      {"beginning",	x_extent, x_start,   "beginning", 1, 0},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 55 "nonxtok.g"
      {"broken", x_state,  x_broken,  "missing", 0, 0}
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
