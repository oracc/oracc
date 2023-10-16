#include <stdlib.h>
#include <string.h>
#include "json.h"

static const char * const jsnch[128] =
  {
    [0]="\\u0000",
    [1]="\\u0001",
    [2]="\\u0002",
    [3]="\\u0003",
    [4]="\\u0004",
    [5]="\\u0005",
    [6]="\\u0006",
    [7]="\\u0007",
    [8]="\\b",
    [9]="\\t",
    [10]="\\n",
    [11]="\\u000B",
    [12]="\\f",
    [13]="\\r",
    [14]="\\u000E",
    [15]="\\u000F",
    [16]="\\u0010",
    [17]="\\u0011",
    [18]="\\u0012",
    [19]="\\u0013",
    [20]="\\u0014",
    [21]="\\u0015",
    [22]="\\u0016",
    [23]="\\u0017",
    [24]="\\u0018",
    [25]="\\u0019",
    [26]="\\u001A",
    [27]="\\u001B",
    [28]="\\u001C",
    [29]="\\u001D",
    [30]="\\u001E",
    [31]="\\u001F",
    ['"']="\\\"",
    ['\\']="\\\\",
    ['/']="\\/"
  };

const unsigned char *
jsonify(const unsigned char *c)
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

  if (!strpbrk((char*)c,
	       "\x1\x2\x3\x4\x5\x6\x7\x8\x9\xa\xb\xc\xd\xe\xf"
	       "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f"
	       "\"\\/"))
    return c;

  thislen = (5 * strlen((char*)c)) + 1; /* maximum possible expansion of string */
  if (xbuf_alloced < thislen)
    {
      if (!(xbuf = realloc(xbuf,thislen)))
	abort();
      xbuf_alloced = thislen;
    }

  for (insertp = xbuf; *c; ++c)
    {
      if (*c < 128 && jsnch[*c])
	{
	  strcpy((char*)insertp,jsnch[*c]);
	  insertp += strlen(jsnch[*c]);
	}
      else
	*insertp++ = *c;
    }
  *insertp = '\0';

  return xbuf;
}
