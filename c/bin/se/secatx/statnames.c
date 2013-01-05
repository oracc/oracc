/* C code produced by gperf version 3.0.3 */
/* Command-line: gperf -N statnames -tT statnames.g  */
/* Computed positions: -k'3,$' */

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

#line 1 "statnames.g"

#include <string.h>
#include "fields.h"

#define TOTAL_KEYWORDS 66
#define MIN_WORD_LENGTH 4
#define MAX_WORD_LENGTH 22
#define MIN_HASH_VALUE 12
#define MAX_HASH_VALUE 132
/* maximum key range = 121, duplicates = 0 */

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
      133, 133, 133, 133, 133, 133, 133, 133, 133, 133,
      133, 133, 133, 133, 133, 133, 133, 133, 133, 133,
      133, 133, 133, 133, 133, 133, 133, 133, 133, 133,
      133, 133, 133, 133, 133, 133, 133, 133, 133, 133,
      133, 133, 133, 133, 133, 133, 133, 133, 133, 133,
      133, 133, 133, 133, 133, 133, 133, 133, 133, 133,
      133, 133, 133, 133, 133, 133, 133, 133, 133, 133,
      133, 133, 133, 133, 133, 133, 133, 133, 133, 133,
      133, 133, 133, 133, 133, 133, 133, 133, 133, 133,
      133, 133, 133, 133, 133,  15, 133,  80,  45,   5,
       45,  10,   5, 133,   0,   5,  85, 133,  15,   5,
        5,  55,  25,   0,  45,   0,  30, 133,   0, 133,
        0,  20, 133, 133, 133, 133, 133, 133, 133, 133,
      133, 133, 133, 133, 133, 133, 133, 133, 133, 133,
      133, 133, 133, 133, 133, 133, 133, 133, 133, 133,
      133, 133, 133, 133, 133, 133, 133, 133, 133, 133,
      133, 133, 133, 133, 133, 133, 133, 133, 133, 133,
      133, 133, 133, 133, 133, 133, 133, 133, 133, 133,
      133, 133, 133, 133, 133, 133, 133, 133, 133, 133,
      133, 133, 133, 133, 133, 133, 133, 133, 133, 133,
      133, 133, 133, 133, 133, 133, 133, 133, 133, 133,
      133, 133, 133, 133, 133, 133, 133, 133, 133, 133,
      133, 133, 133, 133, 133, 133, 133, 133, 133, 133,
      133, 133, 133, 133, 133, 133, 133, 133, 133, 133,
      133, 133, 133, 133, 133, 133, 133, 133, 133, 133,
      133, 133, 133, 133, 133, 133
    };
  return len + asso_values[(unsigned char)str[2]] + asso_values[(unsigned char)str[len - 1]];
}

#ifdef __GNUC__
__inline
#ifdef __GNUC_STDC_INLINE__
__attribute__ ((__gnu_inline__))
#endif
#endif
struct sn_tab *
statnames (str, len)
     register const char *str;
     register unsigned int len;
{
  static struct sn_tab wordlist[] =
    {
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""},
#line 67 "statnames.g"
      {"text_remarks",		text_remarks},
      {""},
#line 68 "statnames.g"
      {"thickness",		thickness},
      {""},
#line 25 "statnames.g"
      {"designation",		designation},
      {""},
#line 29 "statnames.g"
      {"envelope",		envelope},
#line 42 "statnames.g"
      {"name",			name},
#line 34 "statnames.g"
      {"genre", 			genre},
#line 32 "statnames.g"
      {"findspot_remarks",	findspot_remarks},
#line 26 "statnames.g"
      {"display_size",		display_size},
#line 38 "statnames.g"
      {"language",		language},
#line 28 "statnames.g"
      {"elevation",		elevation},
#line 12 "statnames.g"
      {"atf_source",		atf_source},
#line 37 "statnames.g"
      {"join_information",	join_information},
      {""},
#line 71 "statnames.g"
      {"writing_phase",		writing_phase},
#line 50 "statnames.g"
      {"primary_publication", 	primary_publication},
#line 33 "statnames.g"
      {"findspot_square",	findspot_square},
#line 19 "statnames.g"
      {"condition_description",	condition_description},
      {""}, {""},
#line 16 "statnames.g"
      {"cdli_collation",		cdli_collation},
      {""},
#line 13 "statnames.g"
      {"atf_up",			atf_up},
#line 27 "statnames.g"
      {"electronic_publication",	electronic_publication},
      {""},
#line 9 "statnames.g"
      {"acquisition_history",	acquisition_history},
#line 39 "statnames.g"
      {"lineart_up",		lineart_up},
#line 35 "statnames.g"
      {"height",			height},
#line 23 "statnames.g"
      {"date_remarks",		date_remarks},
#line 17 "statnames.g"
      {"citation",		citation},
#line 15 "statnames.g"
      {"author_remarks",		author_remarks},
#line 65 "statnames.g"
      {"supergenre",		supergenre},
      {""},
#line 11 "statnames.g"
      {"alternative_years",	alternative_years},
      {""},
#line 22 "statnames.g"
      {"date_of_origin",		date_of_origin},
#line 70 "statnames.g"
      {"width",			width},
#line 20 "statnames.g"
      {"corpus",			corpus},
#line 36 "statnames.g"
      {"id_text",		id_text},
#line 40 "statnames.g"
      {"material",		material},
#line 62 "statnames.g"
      {"subcorpus",		subcorpus},
#line 46 "statnames.g"
      {"owner",			owner},
#line 53 "statnames.g"
      {"public",			public},
      {""},
#line 55 "statnames.g"
      {"public_images",		public_images},
#line 48 "statnames.g"
      {"period_remarks",		period_remarks},
#line 54 "statnames.g"
      {"public_atf",		public_atf},
#line 64 "statnames.g"
      {"subgenre_remarks",	subgenre_remarks},
#line 18 "statnames.g"
      {"composite_id",		composite_id},
#line 63 "statnames.g"
      {"subgenre", 		subgenre},
#line 41 "statnames.g"
      {"museum_no", 		museum_no},
      {""}, {""},
#line 8 "statnames.g"
      {"accounting_period",	accounting_period},
      {""},
#line 58 "statnames.g"
      {"published_collation",	published_collation},
#line 66 "statnames.g"
      {"surface_preservation",	surface_preservation},
#line 56 "statnames.g"
      {"publication_date", 	publication_date},
#line 7 "statnames.g"
      {"accession_no",		accession_no},
#line 30 "statnames.g"
      {"excavation_no", 		excavation_no},
#line 52 "statnames.g"
      {"provenience_remarks",	provenience_remarks},
      {""},
#line 51 "statnames.g"
      {"provenience",		provenience},
      {""}, {""},
#line 61 "statnames.g"
      {"stratigraphic_level",	stratigraphic_level},
      {""},
#line 14 "statnames.g"
      {"author", 		author},
      {""},
#line 72 "statnames.g"
      {"xproject",		xproject},
#line 57 "statnames.g"
      {"publication_history",	publication_history},
      {""},
#line 31 "statnames.g"
      {"external_id",		external_id},
#line 21 "statnames.g"
      {"date_entered",		date_entered},
#line 49 "statnames.g"
      {"photo_up",		photo_up},
      {""}, {""},
#line 24 "statnames.g"
      {"dates_referenced",	dates_referenced},
      {""}, {""},
#line 69 "statnames.g"
      {"tradition",		tradition},
      {""},
#line 47 "statnames.g"
      {"period", 		period},
      {""},
#line 10 "statnames.g"
      {"aka_name",		aka_name},
#line 44 "statnames.g"
      {"object_remarks",		object_remarks},
      {""},
#line 60 "statnames.g"
      {"seal_information",	seal_information},
      {""}, {""}, {""}, {""},
#line 45 "statnames.g"
      {"object_type",		object_type},
      {""}, {""},
#line 43 "statnames.g"
      {"object_preservation",	object_preservation},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""},
#line 59 "statnames.g"
      {"seal_id",		seal_id}
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
