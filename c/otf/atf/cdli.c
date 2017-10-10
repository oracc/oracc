/* C code produced by gperf version 3.0.3 */
/* Command-line: /Library/Developer/CommandLineTools/usr/bin/gperf -N cdli -tT cdli.g  */
/* Computed positions: -k'' */

#line 1 "cdli.g"

#include <string.h>
#include "cdli.h"

#define TOTAL_KEYWORDS 1
#define MIN_WORD_LENGTH 6
#define MAX_WORD_LENGTH 6
#define MIN_HASH_VALUE 6
#define MAX_HASH_VALUE 6
/* maximum key range = 1, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
/*ARGSUSED*/
static unsigned int
hash (str, len)
     register const char *str;
     register unsigned int len;
{
  return len;
}

struct cdli_grapheme *
cdli (str, len)
     register const char *str;
     register unsigned int len;
{
  static struct cdli_grapheme wordlist[] =
    {
      {""}, {""}, {""}, {""}, {""}, {""},
#line 11 "cdli.g"
      {"saggal",	"|SAG.IG|"}
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
