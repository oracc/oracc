#include <stdio.h>
#include <stdlib.h>
#include <psd_base.h>
#include <options.h>
#include "xmlparse.h"

extern char *strdup(const char *);

#include "otlenum.h"

#define badseq()      fprintf(stderr,\
			      "pqoutline: malformed sort sequence (only [tgp] allowed)\n"), exit(-1)

const char *cgi = "cdlpager";
const char *href = "";
const char *outline_type_str;
const char *type = NULL;
const char *output_dir = NULL;
int output_page = -1;
const char *outline_template = NULL;

FILE*outlinefp,*pagesfp,*templatefp = NULL;
char outlinefn[_MAX_PATH], pagesfn[_MAX_PATH], cfn[_MAX_PATH];

struct stash
{
  const char **ptrs;
  int alloced;
  int maxc;
};

struct stash g_stash, p_stash, t_stash, *curr_stash;
struct stash *stashes[3];

char buf[16384]; /* sortvals-.xml is about 12k; we leave 
		    room to grow and still make only one read
		    (should do this dynamically, but laziness
		    is supposed to be a virtue...)
		 */

#define str_of(stash,index) \
  (index>stashes[stash]->maxc) ? "unknown" : stashes[stash]->ptrs[index]

/*
  Level data is:
  0 = level 1 sort
  1 = level 2 sort
  2 = level 3 sort
  3 = page index
  4 = item count
 */

void
process_otl_line(const char *l)
{
  int level = 0;
  int level_data[5] = { -1, -1, -1, -1, -1 };
  static int curr_l1 = -1, curr_l2 = -1;
  static int open_l1 = 0, open_l2 = 0;

  if (NULL == l)
    {
      if (open_l1)
	{
	  if (open_l2 > 0)
	    fputs("</div>",outlinefp);
	  fputs("</div>",outlinefp);
	}
      open_l1 = open_l2 = 0;
      return;
    }

  while (*l)
    {
      if (':' == *l || '\t' == *l)
	++level;
      else if (isdigit(*l))
	{
	  level_data[level] = atoi(l);
	  while (isdigit(l[1]))
	    ++l;
	}
      else if ('\n' == *l)
	;
      else
	fprintf(stderr,"bad outline entry\n"), exit(-1);
      ++l;
    }

  if (level_data[0] >= 0 && level_data[0] != curr_l1)
    {
      if (open_l1)
	{
	  if (open_l2 > 0)
	    fputs("</div>",outlinefp);
	  fputs("</div>",outlinefp);
	}
      fputs("<div class=\"pqotl level1\">",outlinefp);
      fprintf(outlinefp,"<h2>%s</h2>",str_of(0,level_data[0]));
      curr_l1 = level_data[0];
      curr_l2 = -1;
      open_l1 = 1;
      open_l2 = 0;
    }

  if (level_data[1] >= 0 && level_data[1] != curr_l2)
    {
      if (open_l2--)
	fputs("</div>",outlinefp);
      fputs("<div class=\"pqotl level2\">",outlinefp);
      fprintf(outlinefp,"<h3>%s</h3>",str_of(1,level_data[1]));
      curr_l2 = level_data[1];
      open_l2 = 1;
    }

  if (level_data[2] >= 0)
    fprintf(outlinefp,
	    "<p><a onclick=\"p(%d)\">%s [%d]</a></p>",
	    1+level_data[3], 
	    str_of(2,level_data[2]),
	    level_data[4]);
}

void
set_stashes(const char *seq)
{
  switch (seq[0])
    {
    case '\n':
      break;
    case 'g':
      stashes[0] = &g_stash;
      switch (seq[1])
	{
	case 'p':
	  stashes[1] = &p_stash;
	  if (seq[2] == 't')
	    stashes[2] = &t_stash;
	  else
	    badseq();
	  break;
	case 't':
	  stashes[1] = &t_stash;
	  if (seq[2] == 'p')
	    stashes[2] = &p_stash;
	  break;
	default:
	  badseq();
	}
      break;
    case 'p':
      stashes[0] = &p_stash;
      switch (seq[1])
	{
	case 'g':
	  stashes[1] = &g_stash;
	  if (seq[2] == 't')
	    stashes[2] = &t_stash;
	  else
	    badseq();
	  break;
	case 't':
	  stashes[1] = &t_stash;
	  if (seq[2] == 'g')
	    stashes[2] = &g_stash;
	  else
	    badseq();
	  break;
	default:
	  badseq();
	}
      break;
    case 't':
      stashes[0] = &t_stash;
      switch (seq[1])
	{
	case 'g':
	  stashes[1] = &g_stash;
	  if (seq[2] == 'p')
	    stashes[2] = &p_stash;
	  else
	    badseq();
	  break;
	case 'p':
	  stashes[1] = &p_stash;
	  if (seq[2] == 'g')
	    stashes[2] = &g_stash;
	  else
	    badseq();
	  break;
	default:
	  badseq();
	}
      break;
    default:
      badseq();
    }
  outline_type_str = seq;
}

void
startElement(void *userData, const char *name, const char **atts)
{
  int i;
  if (name[0] == 'v' && name[1] == '\0' && type)
    {
      int c = -1;
      const char *n = NULL;
      for (i = 0; atts[i] != NULL; ++i)
	{
	  if (atts[i][0] == 'c' && atts[i][1] == '\0')
	    c = atoi(atts[i+1]);
	  else if (atts[i][0] == 'n' && atts[i][1] == '\0')
	    n = strdup(atts[i+1]);
	}
      if (c >= 0 && n)
	{
	  /* printf("%d=>%s\n",c,n); */
	  while (c >= curr_stash->alloced)
	    {
	      curr_stash->alloced += 2048;
	      curr_stash->ptrs = realloc(curr_stash->ptrs,
					 curr_stash->alloced*sizeof(char *));
	    }
	  curr_stash->ptrs[c] = n;
	  if (c > curr_stash->maxc)
	    curr_stash->maxc = c;
	}
      else
	{
	  fprintf(stderr,"pqoutline: failed to find c and n\n");
	  exit(-1);
	}
    }
  else
    {
      switch (*name)
	{
	case 'p':
	  if (name[1] == 'l')
	    {
	      type = "p";
	      curr_stash = &p_stash;
	    }
	  else if (name[1] == 'e')
	    {
	      type = "t";
	      curr_stash = &t_stash;
	    }
	  break;
	case 's':
	  type = "g";
	  curr_stash = &g_stash;
	  break;
	default:
	  type = NULL;
	  break;
	}
#if 0
      if (type)
	printf("%s=>%s\n", name,type);
#endif
    }
}

int
main(int argc, char **argv)
{
  XML_Parser parser = XML_ParserCreate("utf-8");
  int done;
  int depth = 0;
  FILE*sortvals = fopen("/usr/local/oracc/lib/data/sortvals-.xml","r");
  int in_outline = 0;
  char *l;
  FILE *cfp;
  int item_count = 0;
  int page_count = 0;
  const char *newline = NULL;

  options(argc,argv,"c:d:h:p:t:");

  if (output_dir)
    {
      strcpy(outlinefn,output_dir);
      if ('/' == outlinefn[strlen(outlinefn)-1]) {
	outlinefn[strlen(outlinefn)-1] = '\0';
      }
      strcpy(pagesfn,outlinefn);
      strcpy(cfn,outlinefn);
      strcat(outlinefn,"/outline.html");
      strcat(pagesfn,"/pages.xml");
      strcat(cfn,"/search");
      outlinefp = xfopen(outlinefn,"w");
      pagesfp = xfopen(pagesfn,"w");
      cfp = xfopen(cfn,"a");
      if (outline_template)
	{
	  int ch;
	  templatefp = xfopen(outline_template, "r");
	  while (EOF != (ch = fgetc(templatefp)))
	    {
	      if ('@' == ch)
		{
		  int ch2 = fgetc(templatefp);
		  if (ch2 == '@')
		    {
		      fputs("@@",outlinefp);
		      break;
		    }
		  else
		    {
		      fputc(ch,outlinefp);
		      fputc(ch2,outlinefp);
		    }
		}
	      else
		fputc(ch,outlinefp);
	    }
	}
      newline = "";
    }
  else
    {
      outlinefp = stdout;
      pagesfp = stdout;
      cfp = stdout;
      newline = "\n";
    }

  XML_SetElementHandler(parser, startElement, NULL);
  for (;;)
    {
      int done;
      size_t len = fread(buf, 1, sizeof(buf), sortvals);
      if (ferror(sortvals))
	{
	  fprintf(stderr,"pqoutline: read error\n");
	  exit(-1);
	}
      done = feof(sortvals);
      if (!XML_Parse(parser, buf, len, done))
	{
	  fprintf(stderr,
		  "pqoutline: %s at line %d\n",
		  XML_ErrorString(XML_GetErrorCode(parser)),
		  XML_GetCurrentLineNumber(parser));
	  exit(-1);
	}
      if (done)
	break;
    }
  XML_ParserFree(parser);
  fputs("<div class=\"pqotl level0\">",outlinefp);
  while (NULL != (l = fgets(buf,8191,stdin)))
    {
      if (isdigit(*l) || ':' == *l)
	{
	  ++in_outline;
	  if (l[strlen(l)-1] != '\n')
	    {
	      fprintf(stderr,"malformed outline line entry: %s\n",l);
	      exit(-1);
	    }
	  process_otl_line(l);
	}
      else if (*l == 't' || *l == 'p' || *l == 'g')
	{
	  if (in_outline)
	    break;
	  else
	    set_stashes(l);
	}
      else if (*l == 'i')
	{
	  item_count = atoi(l+2);
	}
      else
	break;
    }
  process_otl_line(NULL);
  fputs("</div>",outlinefp);
  if (*newline)
    fputc(*newline,outlinefp);
  if (templatefp)
    {
      int ch;
      while (EOF != (ch = fgetc(templatefp)))
	fputc(ch,outlinefp);
    }
  if (output_dir)
    xfclose(outlinefn,outlinefp);
  if (l)
    {
      if (output_page < 0)
	fputs("<pages>",pagesfp);
      while (1)
	{
	  ++page_count;
	  buf[strlen(buf)-1] = '\0';
	  if (output_page < 0)
	    fprintf(pagesfp,"<p>%s</p>",buf);
	  else if (output_page == page_count)
	    fputs(buf,pagesfp);
	  if (NULL == fgets(buf,8191,stdin))
	    break;
	}
      if (output_page < 0)
	fputs("</pages>",pagesfp);
    }
  if (output_dir)
    xfclose(pagesfn,pagesfp);

  if (cfp)
    {
      fprintf(cfp,"%spages %d\n",newline,page_count);
      fprintf(cfp,"items %d\n",item_count);
      if (output_dir)
	xfclose(cfn,cfp);
    }

  return 0;
}

int
opts(int argc, char *arg)
{
  switch (argc)
    {
    case 'c':
      cgi = arg;
      break;
    case 'd':
      output_dir = arg;
      break;
    case 'h':
      href = arg;
      break;
    case 'p':
      output_page = atoi(arg);
      break;
    case 't':
      outline_template = arg;
      break;
    default:
      return 1;
    }
  return 0;
}

const char *prog = "pqoutline";
int major_version = 1, minor_version = 0;
const char *usage_string = "[-d output-dir] [-t outline-template]";
void
help ()
{
  ;
}
