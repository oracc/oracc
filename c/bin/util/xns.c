/* This is simple demonstration of how to use expat. This program
reads an XML document from standard input and writes a line with the
name of each element to standard output indenting child elements by
one tab stop more than their parent element. */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "xmlparse.h"

const char *oraccns = "http://oracc.org/ns/";

const char *
nsname(const char *name)
{
  static char *tmp = NULL;
  static int tmplen = 0;
  const char *nssep;
  if (tmplen < strlen(name))
    tmp = realloc(tmp,2*strlen(name));

  if ((nssep = strchr(name, '|')))
    {
      if (!strncmp(name,oraccns,strlen(oraccns)))
	{
	  const char *t = name + strlen(oraccns);
	  char *t2 = tmp;

	  while ('/' != *t)
	    *t2++ = *t++;
	  *t2++ = '_';
	  strcpy(t2,nssep+1);
	}
      else if (strstr(name, "http://www.w3.org/XML/1998/namespace"))
	{
	  sprintf(tmp,"xml:%s",nssep+1);
	}
      else
	{
	  fprintf(stderr, "xj: unknown namespace %s\n", name);
	  strcpy(tmp,name);
	}
      return tmp;
    }
  else
    return name;
}

void
printText(const char *s, int len)
{
  int i;
  for (i = 0; i < len; ++i)
#if 0
    putchar(s[i]);
#else
    switch (s[i])
      {
      case '<': printf("&lt;");
	break;
      case '&': printf("&amp;");
	break;
      case '"': printf("&quot;");
	break;
      default: putchar(s[i]);
	break;
      }
#endif
}

void charHandler(void *userData,
			  const XML_Char *s,
			  int len)
{
  printText(s,len);
  /*  fwrite(s,1,len,stdout); */
}

void startElement(void *userData, const char *name, const char **atts)
{
  int *depthPtr = userData;
  const char **ap;
#if 0
  int i;
  for (i = 0; i < *depthPtr; i++)
    putchar('\t');
#endif
  printf("<%s",nsname(name));
  if (atts)
    {
      for (ap = atts; ap[0]; ++ap)
	{
	  printf(" %s=\"",nsname(*ap));
	  ++ap;
	  printText(*ap, strlen(*ap));
	  putchar('"');
	}
    }
  putchar('>');
  *depthPtr += 1;
}

void endElement(void *userData, const char *name)
{
  int *depthPtr = userData;
  *depthPtr -= 1;
  printf("</%s\n>", nsname(name));
}

int main()
{
  char buf[BUFSIZ];
  XML_Parser parser = XML_ParserCreateNS("utf-8",'|');
  int done;
  int depth = 0;
  XML_SetUserData(parser, &depth);
  XML_SetElementHandler(parser, startElement, endElement);
  XML_SetCharacterDataHandler(parser, charHandler);
  do {
    size_t len = fread(buf, 1, sizeof(buf), stdin);
    done = len < sizeof(buf);
    if (!XML_Parse(parser, buf, len, done)) {
      fprintf(stderr,
              "%s at line %d\n",
              XML_ErrorString(XML_GetErrorCode(parser)),
              XML_GetCurrentLineNumber(parser));
      return 1;
    }
  } while (!done);
  XML_ParserFree(parser);
  return 0;
}
