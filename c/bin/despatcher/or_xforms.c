#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "resolver.h"

static char *xml_read_sans_content_length(size_t *lenp);
static int xml_content(const char *ct);
static char *xml_docelem(const char *xml);
static void xml_handler(char *xml, size_t len);
static char *xml_value(const char *xml, const char *tag);

void
xforms_handler(void)
{
  const char *content_type = getenv("CONTENT_TYPE"), *content_length;
  char *xml = NULL;
  size_t len = 0;
  if (content_type && xml_content(content_type))
    {
      if ((content_length = getenv("CONTENT_LENGTH")))
	{
	  if ((len = atoi(content_length)) > 0)
	    {
	      xml = malloc(len+1);
	      if (fread(xml, 1, len, stdin) == len)
		xml_handler(xml, len);
	      else
		do404();
	    }
	}
      else
	{
	  /* Bloody Safari */
	  xml = xml_read_sans_content_length(&len);
	  if (len)
	    xml_handler(xml, len);
	  else
	    do404();
	}
    }
  else
    do404();
  exit(1);
}

static void
xml_handler(char *xml, size_t len)
{
  char *xml_type = NULL;
  
  /* skip XML decl / PIs */
  while (*xml && xml[0] == '<' && xml[1] == '?')
    {
      xml += 2;
      while (*xml && xml[0] != '?' && xml[1] != '>')
	++xml;
      xml += 2;
      while (isspace(*xml))
	++xml;
    }

  xml_type = xml_docelem(xml);
  fprintf(stderr, "oracc-despatcher: xml_type=%s\n", xml_type);
  if (!strcmp(xml_type, "browse")
      || !strcmp(xml_type, "pager")
      || !strcmp(xml_type, "search"))
    {
      char *session = xml_value(xml, "session");
      if (session)
	{
	  char *out = malloc(1 + strlen("/var/tmp/oracc/pager//.xml") +
			     strlen(session)+strlen(xml_type));
	  char *prg = malloc(1 + strlen("/usr/local/oracc/bin/p2-.plx"));
	  sprintf(out, "/var/tmp/oracc/pager/%s/%s.xml", session, xml_type);
	  sprintf(prg, "/usr/local/oracc/bin/p2-%s.plx", xml_type);
	  fprintf(stderr, "writing to %s", out);
	  FILE *x = fopen(out, "w");
	  xml[len] = 0;
	  fwrite(xml, 1, len, x);
	  fclose(x);
	  fprintf(stderr, "calling %s %s", prg, out);
	  unsetenv("SCRIPT_URL");
	  execl(prg, xml_type, out, NULL);
	  perror("execl failed");
	  exit(1);
	}
      else
	do404();
    }
  else
    do404();
}

static char *
xml_docelem(const char *xml)
{
  const char *start, *end;
  char *buf;
  int len;

  /* FireFox strips <browse xmlns=""> to <browse> but others
     don't */
  if ('<' == *xml)
    {
      start = xml+1;
      for (end = start; *end && !isspace(*end) && '>' != *end; ++end)
	;
      len = end-start;
      buf = malloc(1 + len);
      strncpy(buf, start, len);
      buf[len] = '\0';
      return buf;
    }
  else
    return NULL;
}

static char * 
xml_value(const char *xml, const char *tag)
{
  const char *t = xml, *e = NULL, *v = NULL, *v_end = NULL;
  while (t && (t = strstr(t, tag)))
    {
      if (t[-1] == '<')
	{
	  e = t + strlen(tag);
	  if (*e == '>' || (*e == '/' && *++e == '>'))
	    {
	      v = e + 1;
	      break;
	    }
	}
      else
	t = strchr(t, '>');
    }
    if (v)
      {
	while (e && (e = strstr(e, tag)))
	  {
	    if ('/' == e[-1] && '<' == e[-2])
	      {
		if (e[strlen(tag)] == '>')
		  {
		    v_end = e - 2;
		    break;
		  }
	      }
	    else
	      e = strchr(e, '>');
	  }
	if (v_end)
	  {
	    char *ret = malloc(1 + v_end - v);
	    strncpy(ret, v, v_end - v);
	    ret[v_end - v] = '\0';
	    return ret;
	  }
      }    
    return NULL;
}

static int
xml_content(const char *ct)
{
  return strstr(ct, "application/xml") != NULL;
}

/* We define XML_BUF_MAX to a size far larger than
   any reasonable use of the Oracc 2 XForms--even
   the largest Advanced Search should fit easily
   in 10k.  If it doesn't, someone is probably up
   to no good, so we bounce them.
   
   99.99% of instances sent over the wire will be
   less than 2K.
 */
#define XML_BUF_BLOCK 2048
#define XML_BUF_MAX  10240
static char *
xml_read_sans_content_length(size_t *lenp)
{
  char *xml = malloc(XML_BUF_BLOCK);
  size_t alloced = XML_BUF_BLOCK;
  size_t used = 0;
  int ch = 0;
  while (EOF != ((ch = getchar())))
    {
      if (used == alloced)
	{
	  if (alloced == XML_BUF_MAX)
	    {
	      free(xml);
	      do404();
	    }
	  xml = realloc(xml, alloced += XML_BUF_BLOCK);
	}
      xml[used++] = ch;
    }
  xml[used] = '\0';
  *lenp = used;
}
