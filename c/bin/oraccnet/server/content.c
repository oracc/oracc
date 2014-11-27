#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "warning.h"
#include "runexpat.h"
#include "resolver.h"
#include "content.h"

struct frag
{
  int nesting;
  const char *xid;
  FILE *fp;
  struct content_opts *cop;
};

static int cued_printStart = 0, need_gdf_closer = 0;

static void content_eH(void *userData, const char *name);
static void content_sH(void *userData, const char *name, const char **atts);
static void printEnd(struct frag *frag, const char *name);
static void printHTMLStart(struct frag *frag);
static void printStart(struct frag *frag, const char *name, const char **atts, const char *xid);
static void printText(const char *s, FILE *frag_fp);

void
content(struct content_opts *cop, const char *input)
{
  char const *fnlist[2];
  static struct frag fragdata;

  if (access(input, R_OK))
    {
      extern const char *no_html;
      execl("/bin/cat", "cat", no_html, NULL);
      perror("execl failed");
      exit(0);
    }

  fragdata.cop = cop;
  fragdata.xid = cop->chunk_id;
  fragdata.nesting = 0;
  fragdata.fp = stdout;
  fnlist[0] = input;
  fnlist[1] = NULL;

  /* Setting fragdata.nesting here means that the normal end processing cannot
     occur in printEnd so after running expat we have to finish the wrapper
     manually later in this routine */
  if ((cop->echo || cop->wrap) && !cop->chunk_id)
    {
      printHTMLStart(&fragdata);
      fragdata.nesting = 1;
    }

  runexpatNSuD(i_list, fnlist, content_sH, content_eH, NULL, &fragdata);

  if ((cop->echo || cop->wrap) && !cop->chunk_id)
    fputs("</body></html>", fragdata.fp);

  exit(0);
}

static const char **
addClassSelect(const char **atts)
{
  int i = 0;
  char **newAtts = NULL;
  if (atts)
    {
      int class_i = -1;
      for (i = 0; atts[i]; ++i)
	;
      newAtts = malloc((((i+1)*2)+1) * sizeof(const char *));
      for (i = 0; atts[i]; i += 2)
	{
	  newAtts[i] = (char*)atts[i];
	  newAtts[i+1] = (char*)atts[i+1];
	  if (!strcmp(atts[i], "class"))
	    class_i = i;
	}
      if (class_i >= 0)
	{
	  char *newClass = malloc(strlen(atts[class_i+1]) + strlen(" selected") + 1);
	  strcpy(newClass, atts[class_i+1]);
	  strcat(newClass, " selected");
	  newAtts[class_i+1] = newClass;
	}
      else
	{
	  newAtts[i] = "class";
	  newAtts[i+1] = "selected";
	  newAtts[i+2] = NULL;
	}
    }
  else
    {
      newAtts = malloc(3 * sizeof(const char *));
      newAtts[0] = "class";
      newAtts[1] = "selected";
      newAtts[2] = NULL;
    }
  return (const char **)newAtts;
}

static void
content_eH(void *userData, const char *name)
{
  if (*name == 'r' && !strcmp(name, "rp-wrap"))
    return;

  if (((struct frag*)userData)->nesting)
    printEnd(userData, name);
  else
    charData_discard();
}

struct content_opts *
content_new_options(void)
{
  return calloc(1, sizeof(struct content_opts));
}

static void
content_sH(void *userData, const char *name, const char **atts)
{
  const char *xid = get_xml_id(atts);

  if (*name == 'r' && !strcmp(name, "rp-wrap"))
    return;

  if (((struct frag*)userData)->nesting)
    printStart(userData, name, atts, xid);
  else if (xid && *xid && ((struct frag*)userData)->xid && !strcmp(((struct frag*)userData)->xid, xid))
    {
      charData_discard();
      if (((struct frag*)userData)->cop->wrap)
	printHTMLStart(userData);
      printStart(userData, name, atts, xid);
    }
  else if (((struct frag*)userData)->cop->unwrap)
    {
      if (cued_printStart)
	{
	  cued_printStart = 0;
	  if (((struct frag*)userData)->cop->wrap)
	    printHTMLStart(userData);
	  printStart(userData, name, atts, xid);
	}
      else if (!strcmp(name, "body"))
	{
	  charData_discard();
	  cued_printStart = 1;
	}
      else if (((struct frag*)userData)->nesting)
	printStart(userData, name, atts, xid);
      else
	charData_discard();
    }
  else if (!strcmp(name, "gdf:dataset"))
    {
      printStart(userData, "gdf:dataset", atts, xid);
      need_gdf_closer = 1;
      ((struct frag*)userData)->nesting = 0;
    }
  else if (((struct frag*)userData)->cop->html && !strcmp(name, "head"))
    {
      fputs("<html xmlns=\"http://www.w3.org/1999/xhtml\">", 
	    ((struct frag*)userData)->fp);
      printStart(userData, name, atts, xid);
    }
  else
    charData_discard();
}

static void
printEnd(struct frag *frag, const char *name)
{
  if (frag->cop->unwrap)
    {
      if (!strcmp(name, "body"))
	{
	  if (frag->cop->wrap)
	    fputs("\n</body></html>", frag->fp);	    
	  fclose(frag->fp);
	  exit(0);
	}
      else
	{
	  printText((const char *)charData_retrieve(), frag->fp);
	  fprintf(frag->fp, "</%s>", name);
	  return;
	}
    }

  printText((const char *)charData_retrieve(), frag->fp);
  fprintf(frag->fp, "</%s>", name);
  if (!--frag->nesting)
    {
      if (frag->cop->html)
	{
	  if (!strcmp(name, "head"))
	    fputs("<body>", frag->fp);
	  else
	    fputs("</body></html>", frag->fp);
	}
      else
	{
	  if (need_gdf_closer)
	    fputs("</gdf:dataset>", frag->fp);
	  else
	    {
	      if (frag->cop->wrap)
		fputs("</body></html>", frag->fp);
	      fclose(frag->fp);
	      exit(0);
	    }
	}
    }
}

static void
printHTMLStart(struct frag *frag)
{
  fprintf(frag->fp,
	  "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n"
	  "<head>\n"
	  "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\"/>\n"
	  "<title>%s</title>\n"
	  "<link rel=\"shortcut icon\" type=\"image/ico\" href=\"/favicon.ico\" />\n"
	  "<link rel=\"stylesheet\" type=\"text/css\" href=\"/%s/css/p3colours.css\" media=\"screen\"/>\n"
	  "<link rel=\"stylesheet\" type=\"text/css\" href=\"/css/p3.css\" media=\"screen\"/>\n"
	  "<script src=\"/js/p3.js\" type=\"text/javascript\"> </script>\n",
	  frag->cop->title ? frag->cop->title : project, project);
  if (frag->cop->ga)
    {
      fprintf(frag->fp,
	      "<script type=\"text/javascript\">\n"
	      "var _gaq = _gaq || [];\n"
	      "_gaq.push(['_setAccount', 'UA-32878242-1']);\n"
	      "_gaq.push(['_trackPageview']);\n"
	      "(function() {\n"
	      "var ga = document.createElement('script'); ga.type = 'text/javascript'; ga.async = true;\n"
	      "ga.src = ('https:' == document.location.protocol ? 'https://ssl' : 'http://www') + '.google-analytics.com/ga.js';\n"
	      "var s = document.getElementsByTagName('script')[0]; s.parentNode.insertBefore(ga, s);\n"
	      "})();\n</script>\n"
	      );
    }
  if (frag->cop->frag_id)
    fprintf(frag->fp, "</head>\n<body onload=\"location.hash='%s'\">\n", frag->cop->frag_id);
  else
    fputs("</head>\n<body>\n", frag->fp);
}

static void
printStart(struct frag *frag, const char *name, const char **atts, const char *xid)
{
  const char **ap = atts;
  printText((const char*)charData_retrieve(), frag->fp);
  fprintf(frag->fp, "<%s", name);
  if (xid && frag->cop->hilite_id && !strcmp(xid, frag->cop->hilite_id))
    atts = addClassSelect(atts);
  if (atts)
    {
      for (ap = atts; ap[0]; )
	{
	  if (frag->cop->sigs && *ap[0] == 'h' && !strcmp(ap[0], "href"))
	    {
	      if (*ap[1] == 'p' && !strncmp(ap[1], "pop1sig(", 8))
		{
		  fprintf(frag->fp, " href=\"pop1sig('%s','',", project);
		  printText(&ap[1][8], frag->fp);
		  fputc('"', frag->fp);
		  ap += 2;
		  continue;
		}
	    }
	  fprintf(frag->fp, " %s=\"",*ap++);
	  printText(*ap++, frag->fp);
	  fputc('"', frag->fp);
	}
    }
  fputc('>', frag->fp);
  ++frag->nesting;
}

static void
printText(const char *s, FILE *frag_fp)
{
  while (*s)
    {
      if (*s == '<')
	fputs("&lt;",frag_fp);
      else if (*s == '&')
	fputs("&amp;",frag_fp);
      else if (*s == '"')
	fputs("&quot;",frag_fp);
      else
	fputc(*s,frag_fp);
      ++s;
    }
}
