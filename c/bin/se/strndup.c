/* Implementation of strndup() for libc's that don't have it. */

#if !HAVE_STRNDUP

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>

/* Find the length of STRING, but scan at most MAXLEN characters.
   If no '\0' terminator is found in that many characters, return MAXLEN.  */
size_t
strnlen (const char *string, size_t maxlen)
{
  const char *end = memchr (string, '\0', maxlen);
  return end ? end - string : maxlen;
}

char *
strndup (const char *s, size_t n)
{
  size_t len = strnlen (s, n);
  char *new = malloc (len + 1);

  if (new == NULL)
    return NULL;

  new[len] = '\0';
  return memcpy (new, s, len);
}

#endif
