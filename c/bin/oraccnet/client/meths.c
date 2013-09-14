/* C code produced by gperf version 3.0.3 */
/* Command-line: gperf -N meths -tT meths.g  */
/* Computed positions: -k'' */

#line 1 "meths.g"

#include <xmlrpc-c/base.h>
#include "oraccnet.h"
extern struct client_method_info debug_client_info;
extern struct client_method_info environment_client_info;

#define TOTAL_KEYWORDS 2
#define MIN_WORD_LENGTH 5
#define MAX_WORD_LENGTH 11
#define MIN_HASH_VALUE 5
#define MAX_HASH_VALUE 11
/* maximum key range = 7, duplicates = 0 */

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

#ifdef __GNUC__
__inline
#ifdef __GNUC_STDC_INLINE__
__attribute__ ((__gnu_inline__))
#endif
#endif
struct meths_tab *
meths (str, len)
     register const char *str;
     register unsigned int len;
{
  static struct meths_tab wordlist[] =
    {
      {""}, {""}, {""}, {""}, {""},
#line 9 "meths.g"
      {"debug", &debug_client_info},
      {""}, {""}, {""}, {""}, {""},
#line 10 "meths.g"
      {"environment", &environment_client_info}
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
