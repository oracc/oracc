#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <psd_base.h>
#include <runexpat.h>
#include <messages.h>
#include <list.h>

#ifndef _MAX_PATH
#define _MAX_PATH 1023
#endif

#define BUFLEN 8192 /*16384*/

List *runinfo_stack = NULL;

struct runinfo 
{
  void *list;
  List *filenames_list;
  char input_buf[BUFLEN+1];
  int input_len;
  FILE *source;
  char *fname;
  const char **todo;
  int charData_buflen;
  int charData_bufused;
  int more_sources;
  XML_Char *charData_buf;
  int this_one_is_done;
  enum isource from;
};

struct runinfo *curr_rip;
const char *curr_fname = NULL;

/*static int verbose = 0;*/
extern int verbose;

#if 0
static char input_buf[BUFLEN+1];
static FILE *source = NULL;
static int charData_buflen = 0;
static int charData_bufused = 0;
static int more_sources = 0;
static XML_Char *charData_buf = NULL;
static int this_one_is_done;
static List *filenames_list = NULL;
#endif

static void charHandler(void *userData, const XML_Char *data, int len);
static void fail(XML_Parser parser, struct runinfo *rip);

static const char *get_input(struct runinfo *rip);
static void set_input(struct runinfo *rip);

static void runexpat_init(struct runinfo *rip);
static void runexpat_term(struct runinfo *rip);

const char *atf_name = NULL;
int atf_line = 0;

const char *pi_file = NULL;
int pi_line = 0;

const char *
findAttr(const char **atts, const char *name)
{
  while (*atts)
    if (!strcmp(*atts,name))
      return atts[1];
    else
      atts+=2;
  return "";
}

static void
piHandler(void *userData, const XML_Char*target, const XML_Char*data)
{
  if (!strcmp(target, "atf-file"))
    {
      static char atf_name_buf[1024];
      pi_file = atf_name = strcpy(atf_name_buf,data);
      list_add(curr_rip->filenames_list,xstrdup(pi_file));
    }
  else if (!strcmp(target, "atf-line"))
    {
      pi_line = atf_line = atoi(data);
    }
  else if (!strcmp(target, "file"))
    {
      static char name_buf[1024];
      pi_file = strcpy(name_buf,data);
      list_add(curr_rip->filenames_list,xstrdup(pi_file));
    }
  else if (!strcmp(target, "line"))
    {
      pi_line = atoi(data);
    }
}

XML_Char*
charData_retrieve(void)
{
  curr_rip->charData_buf[curr_rip->charData_bufused] = '\0';
  curr_rip->charData_bufused = 0;
  return curr_rip->charData_buf;
}

void
charData_discard(void)
{
  curr_rip->charData_bufused = 0;
}

static void
charHandler(void *userData, const XML_Char *data, int len)
{
  if (curr_rip->charData_bufused + len + 1 > curr_rip->charData_buflen) {
    do 
      curr_rip->charData_buflen *= 2;
    while (curr_rip->charData_bufused + len > curr_rip->charData_buflen);
    curr_rip->charData_buf = realloc(curr_rip->charData_buf, 
				     curr_rip->charData_buflen);
  }
  memcpy(curr_rip->charData_buf + curr_rip->charData_bufused, data, len);
  curr_rip->charData_bufused += len;
}

static void
fail(XML_Parser parser, struct runinfo *rip)
{
  fprintf(stderr,
	  "runexpat: %s:%d: Expat error '%s'\n",
	  rip->fname,
	  XML_GetCurrentLineNumber(parser),
	  XML_ErrorString(XML_GetErrorCode(parser)));
  exit(-1);
}

void
runexpatNSuD(enum isource from, 
	     void * list,
	     XML_StartElementHandler startElement, 
	     XML_EndElementHandler endElement,
	     XML_Char *ns_sep_p,
	     void *uData)
{
  const char *buf;
  XML_Parser parser;
  static int depth;
  struct runinfo *rip = calloc(1,sizeof(struct runinfo));

  if (curr_rip)
    {
      runinfo_stack = list_create(LIST_LIFO);
      list_add(runinfo_stack,curr_rip);
    }
  curr_rip = rip;

  if (ns_sep_p)
    parser = XML_ParserCreateNS("utf-8",*ns_sep_p);
  else
    parser = XML_ParserCreate("utf-8");

  if (!parser)
    {
      fprintf(stderr,"runexpat: NULL return from XML_ParserCreate()\n");
      return;
    }

  depth = 0;
  runexpat_init(rip);
  rip->from = from;
  rip->list = list;

  rip->charData_buflen = 1024;
  rip->charData_buf = malloc(sizeof(XML_Char) * rip->charData_buflen);
  rip->charData_bufused = 0;
  if (uData)
    XML_SetUserData(parser, uData);
  else
    XML_SetUserData(parser, &depth);
  XML_SetElementHandler(parser, startElement, endElement);
  XML_SetProcessingInstructionHandler(parser, piHandler);
  XML_SetCharacterDataHandler(parser, charHandler);

  if (!XML_Parse(parser,"<rp-wrap>",9,0))
    fail(parser, rip);

  set_input(rip);
  for (;;)
    {
      if (NULL == (buf = get_input(rip)))
	break;
      if (rip->input_len && !XML_Parse(parser, buf, rip->input_len, 0))
	{
	  fprintf(stderr,"===\n%s\n===\n",buf);
	  fail(parser, rip);
	}
    }

  if (!XML_Parse(parser,"</rp-wrap>",10,1))
    fail(parser, rip);

  XML_ParserFree(parser);
  runexpat_term(rip);
  free(rip->fname);
  free(rip);

  if (runinfo_stack)
    {
      curr_rip = list_pop(runinfo_stack);
      curr_fname = curr_rip->fname;
      if (!list_len(runinfo_stack))
	{
	  free(runinfo_stack);
	  runinfo_stack = NULL;
	}
    }
  else
    curr_rip = NULL;
}

void
runexpat_init(struct runinfo *rip)
{
  rip->filenames_list = list_create(LIST_SINGLE);
}

void
runexpat_term(struct runinfo *rip)
{
  free(rip->charData_buf);
  rip->charData_buf = NULL;
  rip->charData_buflen = 0;
  if (rip->filenames_list)
    {
      list_free(rip->filenames_list,list_xfree);
      rip->filenames_list = NULL;
    }
}

static const char *
get_input(struct runinfo *rip)
{
  size_t nread = 0;
  char *inputp;

  if (rip->this_one_is_done)
    {
      if (rip->more_sources)
	set_input(rip);
      else
	rip->source = NULL;
    }

  if (!rip->source)
    return NULL;

  nread = fread(rip->input_buf, 1, BUFLEN, rip->source);
  if (ferror(rip->source))
    {
      fprintf(stderr,"read error\n");
      exit(-1);
    }
  rip->input_buf[nread] = '\0';
  inputp = rip->input_buf;
  if (!strncmp(inputp,"<?xml",5))
    {
      inputp += 5;
      while (inputp[0] != '?' && inputp[1] != '>')
	++inputp;
      inputp += 2;
      while ('\n' == inputp[0] || '\r' == inputp[0])
	++inputp;
    }
  if (!strncmp(inputp,"<!DOCTYPE", 9))
    {
      inputp += 9;
      while ('>' != inputp[0])
	++inputp;
      ++inputp;
      while ('\n' == inputp[0] || '\r' == inputp[0])
	++inputp;
    }

  nread -= inputp - rip->input_buf;
  rip->input_len = nread;

  if ((rip->this_one_is_done = feof(rip->source)))
    fclose(rip->source);

  return inputp;
}

static void
set_input(struct runinfo *rip)
{
  if (rip->from == i_stdin)
    {
      rip->more_sources = 0;
      rip->source = stdin;
      curr_fname = rip->fname = xstrdup("<stdin>");
    }
  else if (rip->from == i_names)
    {
      rip->source = NULL;
      while (rip->more_sources)
	{
	  static char fname_buf[_MAX_PATH + 1];
	  char *fp = fgets(fname_buf,_MAX_PATH,stdin);
	  if (fp)
	    {
	      fp = fname_buf + strlen(fname_buf);
	      if ('\n' == fp[-1])
		fp[-1] = '\0';
	      if (NULL == (rip->source = fopen(fname_buf,"r")) && verbose)
		fprintf(stderr,"runexpat: open failed on %s\n",fname_buf);
	      else if (verbose)
		fprintf(stderr,"runexpat: inputting %s\n",fname_buf);
	      rip->more_sources = feof(stdin);
	      curr_fname = rip->fname = xstrdup(fname_buf);
	    }
	  else
	    {
	      rip->source = NULL;
	      rip->more_sources = 0;
	    }
	  if (rip->source)
	    break;
	}
    }
  else if (rip->from == i_list)
    {
      if (rip->todo == NULL)
	{
	  rip->todo = rip->list;
	  if (NULL == rip->todo || NULL == *rip->todo)
	    {
	      rip->source = NULL;
	      rip->more_sources = 0;
	      rip->todo = NULL;
	      return;
	    }
	}
      rip->more_sources = 1;
      while (rip->more_sources)
	{
	  if (NULL == (rip->source = fopen(*rip->todo,"r")) && verbose)
	    fprintf(stderr,"runexpat: open failed on %s\n",*rip->todo);
	  else
	    {
	      if (verbose)
		fprintf(stderr,"runexpat: inputting %s\n",*rip->todo);
	      curr_fname = *rip->todo;
	      rip->fname = xstrdup(curr_fname);
	      progress("xmlinput: %s\n",curr_fname);
	    }
	  rip->more_sources = (NULL != *++rip->todo);
	  if (!rip->more_sources)
	    rip->todo = NULL;
	  if (rip->source)
	    break;
	}
    }
}

const char *
get_xml_id(const char **atts)
{
  /*Different versions of the expat library handle the xml: prefix differently :( */
  /*And now Oracc harvest uses '|' for the delimiter not :, :(( */
  static const char *xml_id1 = "xml:id";
  static const char *xml_id2 = "http://www.w3.org/XML/1998/namespace:id";
  static const char *xml_id3 = "xml|id";
  static const char *xml_id4 = "http://www.w3.org/XML/1998/namespace|id";
  const char *x = findAttr(atts,xml_id1);
  if (!*x)
    x = findAttr(atts,xml_id2);
  if (!*x)
    x = findAttr(atts,xml_id3);
  if (!*x)
    x = findAttr(atts,xml_id4);
  return x;
}
