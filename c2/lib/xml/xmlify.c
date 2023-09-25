#include <stdlib.h>
#include <string.h>
#include "xmlify.h"

xmlify_ptr xmlify = xmlify_yes;

static const char * const xmlch[128] =
  {
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, /* 0x00 */
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, /* 0x08 */
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, /* 0x10 */
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, /* 0x18 */
    NULL,NULL,"&quot;",NULL,NULL,NULL,"&amp;",NULL, /* 0x20 */
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, /* 0x28 */
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, /* 0x30 */
    NULL,NULL,NULL,NULL,"&lt;",NULL,"&gt;",NULL, /* 0x38 */
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, /* 0x40 */
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, /* 0x48 */
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, /* 0x50 */
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, /* 0x58 */
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, /* 0x60 */
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, /* 0x68 */
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, /* 0x70 */
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, /* 0x78 */
  };

const unsigned char *
xmlify_not(const unsigned char *c)
{
  return c;
}

const unsigned char *
xmlify_yes(const unsigned char *c)
{
  static unsigned char *xbuf = NULL;
  static int xbuf_alloced = 0;
  int thislen;
  unsigned char *insertp;

  if (NULL == c)
    {
      free(xbuf);
      xbuf = NULL;
      xbuf_alloced = 0;
      return NULL;
    }

  if (!strpbrk((char*)c,"<>&\""))
    return c;

  thislen = (6 * strlen((char*)c)) + 1; /* maximum possible expansion of string */
  if (xbuf_alloced < thislen)
    {
      if (!(xbuf = realloc(xbuf,thislen)))
	abort();
      xbuf_alloced = thislen;
    }

  for (insertp = xbuf; *c; ++c)
    {
      if (*c < 128 && xmlch[*c])
	{
	  strcpy((char*)insertp,xmlch[*c]);
	  insertp += strlen(xmlch[*c]);
	}
      else
	*insertp++ = *c;
    }
  *insertp = '\0';

  return xbuf;
}
