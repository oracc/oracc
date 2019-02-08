#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "warning.h"
#include "runexpat.h"

/* FIXME: parameterize/compute location of xsltproc at 
   install time or run time */
const char *xsltproc = "/usr/bin/xsltproc";

extern int options(int, char**,const char*);
extern int optind;

static int html_mode = 0, sig_fixer = 0, unwrap_html = 0, htmlid = 0;
static int cued_printStart = 0, need_gdf_closer = 0;

static const char *project = NULL, *xsl = NULL;

struct frag
{
  int nesting;
  const char *xid;
  FILE *fp;
};


static char *
argv_cmd(void)
{
  char *cmd = malloc(strlen(xsl) + strlen("xsltproc - ") + 1);
  sprintf(cmd, "%s %s -", xsltproc, xsl);
  return cmd;
}

void
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

void
printStart(struct frag *frag, const char *name, const char **atts)
{
  const char **ap = atts;
  printText((const char*)charData_retrieve(), frag->fp);
  fprintf(frag->fp, "<%s", name);
  if (sig_fixer && !strcmp(name, "a"))
    {
      for (ap = atts; ap[0]; )
	{
	  fprintf(frag->fp, " %s=\"",*ap++);
	  if (!strcmp(ap[-1], "href") && !strncmp(ap[0], "javascript:pop1sig", 18))
	    {
	      fprintf(frag->fp, "javascript:pop1sig('%s','',", project);
	      printText(&ap[0][19], frag->fp);
	      ++ap;
	    }
	  else
	    printText(*ap++, frag->fp);
	  fputc('"', frag->fp);
	}
    }
  else 
    {
      if (atts)
	{
	  for (ap = atts; ap[0]; )
	    {
	      fprintf(frag->fp, " %s=\"",*ap++);
	      printText(*ap++, frag->fp);
	      fputc('"', frag->fp);
	    }
	}
    }
  fputc('>', frag->fp);
  ++frag->nesting;
}

void
printEnd(struct frag *frag, const char *name)
{
  if (unwrap_html)
    {
      if (!strcmp(name, "body"))
	{
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
      if (html_mode)
	{
	  if (!strcmp(name, "head"))
	    fputs("<body>", frag->fp);
	  else
	    fputs("</body></html>", frag->fp);
	}
      else
	{
	  if (need_gdf_closer)
	    fputs("</o:records>", frag->fp);
	  else
	    {
	      fclose(frag->fp);
	      exit(0);
	    }
	}
    }
}

void
gdf_sH(void *userData, const char *name, const char **atts)
{
  const char *xid = (htmlid ? findAttr(atts, "id") : get_xml_id(atts));

  if (unwrap_html)
    {
      if (cued_printStart)
	{
	  cued_printStart = 0;
	  printStart(userData, name, atts);
	}
      else if (!strcmp(name, "body"))
	{
	  charData_discard();
	  cued_printStart = 1;
	}
      else if (((struct frag*)userData)->nesting)
	printStart(userData, name, atts);
      else
	charData_discard();
    }
  else if (xid && !strcmp(((struct frag*)userData)->xid, xid))
    {
      charData_discard();
      printStart(userData, name, atts);
    }
  else if (((struct frag*)userData)->nesting)
    printStart(userData, name, atts);
  else if (!strcmp(name, "o:records"))
    {
      printStart(userData, "o:records", atts);
      need_gdf_closer = 1;
      ((struct frag*)userData)->nesting = 0;
    }
  else if (html_mode && !strcmp(name, "head"))
    {
      fputs("<html xmlns=\"http://www.w3.org/1999/xhtml\">", 
	    ((struct frag*)userData)->fp);
      printStart(userData, name, atts);
    }
  else
    charData_discard();
}

void
gdf_eH(void *userData, const char *name)
{
  if (((struct frag*)userData)->nesting)
    printEnd(userData, name);
  else
    charData_discard();
}

void
gdf_frag_from_file(const char *fname, const char *xml_id, FILE *outfp)
{
  char const *fnlist[2];
  static struct frag fragdata;
  fragdata.xid = xml_id;
  fragdata.nesting = 0;
  fragdata.fp = outfp;
  fnlist[0] = fname;
  fnlist[1] = NULL;
  runexpatNSuD(i_list, fnlist, gdf_sH, gdf_eH, NULL, &fragdata);
}

int
main(int argc, char **argv)
{
  FILE *outfp = stdout;

  options(argc, argv, "hHp:sux:");
  
  if (xsl)
    {
      char *xsltcommand = argv_cmd();
      fprintf(stderr, "xfrag xsltcommand = %s\n", xsltcommand);
      outfp = popen(xsltcommand, "w");
    }
  
  if (argv[optind] && argv[optind+1])
    gdf_frag_from_file(argv[optind], argv[optind+1], outfp);
  else if (unwrap_html && argv[optind])
    gdf_frag_from_file(argv[optind], NULL, stdout);

  if (xsl)
    (void)pclose(outfp);

  return 0;
}

int verbose = 0;
const char *prog = "xfrag";
int major_version = 1;
int minor_version = 0;
const char *usage_string = " [-h -H -s] [-p PROJECT] [XML_FILE] [XML_ID]";
int
opts(int argc, char *arg)
{
  switch (argc)
    {
    case 'h': html_mode = 1; break;
    case 'H': htmlid = 1; break;
    case 'p': project = arg; break;
    case 's': sig_fixer = 1; break;
    case 'u': unwrap_html = 1; break;
    case 'x': xsl = arg; break;
    default: return 1;
    }
  return 0;
}
void help() {}
