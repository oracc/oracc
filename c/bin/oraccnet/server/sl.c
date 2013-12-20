#include "resolver.h"

/* grapheme may contain letters, digits and utf-8 characters;
   we don't validate utf-8, just let the search fail if it's bad
 */
static int
sl_valid_grapheme(const char *g)
{
  while (*g)
    {
      if (*g < 128 && !isalnum(*g))
	return 0;
      ++g;
    }
  return 1;
}

/* TODO: the URI versions of this need to support XML/TXT formats as well as the UI return */
static void
sl(struct component *c)
{
  const char *extension, *grapheme;
  
  /* try to set extension and grapheme from component/query_string ; validate grapheme if so */

  /* if not, try to set them from CGI post; validate extension/query string */

  if (!sl_valid_extension(extension) || !sl_valid_grapheme(grapheme))
    do404();

  /* run signlist UI */

  execl("/usr/bin/perl", "perl", "@@ORACC@@/bin/slse-slave.plx", grapheme, extension, NULL);
}
