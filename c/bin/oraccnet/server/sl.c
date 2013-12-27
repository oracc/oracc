#include <string.h>
#include "ctype128.h"
#include "resolver.h"
#include "ccgi/ccgi.h"

static int
sl_valid_extension(const char *e)
{
  if (!e
      || !strcmp(e, "compounds")
      || !strcmp(e, "contained")
      || !strcmp(e, "container")
      || !strcmp(e, "homophones")
      || !strcmp(e, "list"))
    return 1;
  else
    return 0;
}

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
void
sl(struct component *c)
{
  CGI_varlist *vl = NULL;
  const char *extension, *grapheme;
  
  /* try to set extension and grapheme from component/query_string ; validate grapheme if so */
  if (c[2].text)
    extension = c[2].text;
  if (query_string)
    grapheme = query_string;

  /* if not, try to set them from CGI post; validate extension/query string */
  vl = CGI_get_post(0, NULL);
  extension = CGI_lookup(vl, "ext");
  grapheme = CGI_lookup(vl, "k1");

  if (!sl_valid_extension(extension) || !sl_valid_grapheme(grapheme))
    do404();

  /* run signlist UI */
  execl("/usr/bin/perl", "perl", "@@ORACC@@/bin/slse-slave.plx", grapheme, extension, NULL);
}
