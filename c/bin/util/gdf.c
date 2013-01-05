#include <stdlib.h>
#include <stdio.h>
#include "runexpat.h"

static int need_gdf_closer = 0;

struct frag
{
  int nesting;
  const char *xid;
  FILE *fp;
};

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
  if (atts)
    {
      for (ap = atts; ap[0]; )
	{
	  fprintf(frag->fp, " %s=\"",*ap++);
	  printText(*ap++, frag->fp);
	  fputc('"', frag->fp);
	}
    }
  fputc('>', frag->fp);
  ++frag->nesting;
}

void
printEnd(struct frag *frag, const char *name)
{
  printText((const char *)charData_retrieve(), frag->fp);
  fprintf(frag->fp, "</%s>", name);
  if (!--frag->nesting)
    {
      if (need_gdf_closer)
	fputs("</gdf:dataset>", frag->fp);
      fclose(frag->fp);
      exit(0);
    }
}

void
gdf_sH(void *userData, const char *name, const char **atts)
{
  const char *xid = get_xml_id(atts);

  if (xid && !strcmp(((struct frag*)userData)->xid, xid))
    {
      charData_discard();
      printStart(userData, name, atts);
    }
  else if (((struct frag*)userData)->nesting)
    printStart(userData, name, atts);
  else if (!strcmp(name, "gdf:dataset"))
    {
      printStart(userData, "gdf:dataset", atts);
      need_gdf_closer = 1;
      ((struct frag*)userData)->nesting = 0;
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
  if (argv[1] && argv[2])
    gdf_frag_from_file(argv[1], argv[2], stdout);
}

int verbose = 0;
const char *prog = "gdf";
int major_version = 1;
int minor_version = 0;
const char *usage_string = "gdf [XML_FILE] [XML_ID]";
void opts() {}
void help() {}
