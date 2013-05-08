/* C code produced by gperf version 3.0.3 */
/* Command-line: gperf -N cdttoks -tT cdttoks.g  */
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

#line 1 "cdttoks.g"

#include "cdt.h"
/* N.B.: do not include any inline tokens here as it breaks
   block_tester in cdt_reader */

#define TOTAL_KEYWORDS 22
#define MIN_WORD_LENGTH 1
#define MAX_WORD_LENGTH 12
#define MIN_HASH_VALUE 1
#define MAX_HASH_VALUE 52
/* maximum key range = 52, duplicates = 0 */

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
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53,  5, 20,  0,
       0,  0,  5,  0, 15, 15, 53, 53, 20, 53,
      15, 30,  0, 53, 53, 20,  5, 53, 53, 10,
      15,  5, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
      53, 53, 53, 53, 53, 53
    };
  register int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[2]];
      /*FALLTHROUGH*/
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
struct cdt_node *
cdttoks (str, len)
     register const char *str;
     register unsigned int len;
{
  static struct cdt_node wordlist[] =
    {
      {""},
#line 15 "cdttoks.g"
      {"p",               odt_text,    odt_p,                cdt_para,  cdt_text},
      {""},
#line 24 "cdttoks.g"
      {"end",             odt_none,    cdt_end_tag,          cdt_line,  cdt_none},
      {""}, {""},
#line 27 "cdttoks.g"
      {"pageno",		     odt_text,	  cdt_pageno,			cdt_line, cdt_none},
#line 16 "cdttoks.g"
      {"caption",         odt_text,    cdt_caption,          cdt_para,  cdt_text},
#line 8 "cdttoks.g"
      {"document",        odt_text,    odt_document,         cdt_white, cdt_list},
      {""},
#line 29 "cdttoks.g"
      {"figno",		     odt_text,	  cdt_figno,			cdt_line, cdt_none},
#line 12 "cdttoks.g"
      {"figure",          odt_text,    cdt_figure,           cdt_line,  cdt_list},
#line 21 "cdttoks.g"
      {"catalog",         odt_foreign, foreign_catalog,      cdt_end,   cdt_data},
#line 17 "cdttoks.g"
      {"atf",             odt_foreign, foreign_atf,          cdt_end,   cdt_data},
      {""},
#line 9 "cdttoks.g"
      {"title",           odt_meta,    odt_title,            cdt_para,  cdt_text},
#line 14 "cdttoks.g"
      {"h",               odt_text,    odt_h,                cdt_para,  cdt_text},
#line 18 "cdttoks.g"
      {"atffile",         odt_none,    cdt_atf_file,         cdt_line,  cdt_none},
      {""}, {""}, {""}, {""},
#line 22 "cdttoks.g"
      {"include",         odt_none,    cdt_include,          cdt_line,  cdt_none},
      {""}, {""},
#line 28 "cdttoks.g"
      {"image",           odt_text,    cdt_image,            cdt_self,  cdt_none},
      {""},
#line 11 "cdttoks.g"
      {"section",         odt_text,    odt_section,          cdt_line,  cdt_list},
      {""}, {""},
#line 10 "cdttoks.g"
      {"style",           odt_style,   cdt_style,            cdt_para,  cdt_style},
      {""},
#line 25 "cdttoks.g"
      {"newpage",		 odt_text,	  cdt_newpage,			cdt_white, cdt_none},
      {""}, {""},
#line 26 "cdttoks.g"
      {"newoddpage",	     odt_text,	  cdt_newoddpage,		cdt_white, cdt_none},
      {""},
#line 23 "cdttoks.g"
      {"project",         odt_meta,    cdt_project_c,        cdt_line,  cdt_none},
      {""}, {""}, {""},
#line 13 "cdttoks.g"
      {"insert",          odt_text,    cdt_insert,           cdt_line,  cdt_list},
#line 19 "cdttoks.g"
      {"xmlfile",         odt_none,    cdt_xml_file,         cdt_line,  cdt_none},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 20 "cdttoks.g"
      {"bibliography",    odt_foreign, foreign_bibliography, cdt_end,   cdt_data}
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
