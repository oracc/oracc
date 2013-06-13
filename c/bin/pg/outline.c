#include <stdio.h>
#include <stdlib.h>
#include <ctype128.h>
#include "xmlutil.h"
#include "sortinfo.h"
#include "pg.h"

extern int zoomid, xml_outline, p3;
extern const char *outline_keys[];
int uzpage = -1;
int zprev = -1;
int znext = -1;

int
o_cmp(const void *a,const void*b)
{
  return ((struct outline*)a)->sic->seq - ((struct outline*)b)->sic->seq;
}

struct outline *
pg_outline(int *nlevelsp)
{
  int i, nlevels = nsortfields - 1, poffset;
  struct outline *o;
  u4 *u4s;
  o = malloc(sic_size * sizeof(struct outline));
  u4s = malloc(2 * sizeof(u4) * sic_size * nlevels);
  poffset = sic_size * nlevels;
  
  for (i = 0; i < sic_size; ++i)
    {
      int index = i * nlevels, j;
      o[i].count = o[i].page = -1;
      o[i].icounts = &u4s[index];
      o[i].poffsets = &u4s[poffset + index];
      o[i].sic = sicache[i];
      for (j = 0; j < nlevels; ++j)
	{
	  u4*pindex = sip->pindex + (sicache[i]->codes - sip->scodes);
	  o[i].poffsets[j] = pindex[sortfields[j]];
	}
    }

  qsort(o,sic_size,sizeof(struct outline),o_cmp);

  *nlevelsp = nlevels;
  return o;
}

const char *xo_head = 
  "<title>Outline</title>\n"
  "<link rel=\"stylesheet\" type=\"text/css\" href=\"/css/p2-p1.css\"/>\n"
  "<script type=\"text/javascript\" src=\"/js/p2.js\">\n</script>\n"
  ;

void
pg_outline_dump(FILE *fp, struct outline *o, int nlevels)
{
  static u4 u4max = 0xffffffff;
  int i, j;
  u4 *lastlevs = malloc(nlevels * sizeof(u4*));
  int olevs = 0;
  int nheadings = nlevels - 1;
  extern int fragment;

  for (i = 0; i < nlevels; ++i)
    lastlevs[i] = u4max;

  if (xml_outline)
    {
      extern int l2;
      if (l2)
	{
	  extern const char *project;
	  if (!fragment)
	    {
	      fprintf(fp, "<html xmlns=\"http://www.w3.org/1999/xhtml\"><head>%s", xo_head);
	      fprintf(fp, "<link rel=\"stylesheet\" type=\"text/css\" href=\"/css/p2.css\"/>\n");
	      fprintf(fp, "<link rel=\"stylesheet\" type=\"text/css\" href=\"/%s/p2.css\"/>\n", project);
	      fprintf(fp, "<script src=\"/js/p2.js\" type=\"text/javascript\">\n</script>\n");
	      fprintf(fp, "<script src=\"/js/p2-shortcuts.js\" type=\"text/javascript\">\n</script>\n");
	      fprintf(fp, "<script src=\"/js/p2-keys.js\" type=\"text/javascript\">\n</script>\n");
	      fprintf(fp, "</head><body onload=\"p2Keys()\">");
	    }
	}
      else
	{
	  fprintf(fp, "<html xmlns=\"http://www.w3.org/1999/xhtml\"><head>%s</head><body>", xo_head);
	}
    }

  fputs("<div class=\"pgotl level0\">",fp);
  for (i = 0; i < sic_size; ++i)
    {
      for (j = 0; ; ++j)
	{
	  const char *zoomstr = "";
	  if (j >= nlevels)
	    break;

	  if (o[i].poffsets[j] != lastlevs[j])
	    {
	      if (o[i].sic->id == zoomid)
		{
		  zoomstr = " class=\"zoomed\"";
		  uzpage = o[i].page;
		  if (i)
		    zprev = o[i-1].sic->id;
		  if (i < sic_size-1)
		    znext = o[i+1].sic->id;
		}
	      lastlevs[j] = o[i].poffsets[j];
	      
	      if (j < nheadings)
		{
		  while (olevs > j)
		    fputs("</div>",fp), --olevs;
		  fprintf(fp, "<h%d>%c%s</h%d>",j+2,
			  toupper(sip->pool[o[i].poffsets[j]]),
			  (char*)xmlify(&sip->pool[o[i].poffsets[j]+1]),
			  j+2);
		  fprintf(fp,"<div class=\"pgotl level%d\">",++olevs);
		}
	      else
		{
		  if (p3)
		    fprintf(fp, "<p%s><a href=\"javascript://\" onclick=\"p3zoom(%d)\">%s [%ld]</a></p>", 
			    zoomstr,
			    o[i].sic->id + 1,
			    (char*)xmlify(&sip->pool[o[i].poffsets[j]]),
			    (long)o[i].count);
		  else
		    fprintf(fp, "<p%s><a href=\"javascript://\" onclick=\"pz(1,%d)\">%s [%ld]</a></p>", 
			    zoomstr,
			    o[i].sic->id + 1,
			    (char*)xmlify(&sip->pool[o[i].poffsets[j]]),
			    (long)o[i].count);
		}
	      ++j;
	      while (j < nlevels)
		{
		  lastlevs[j] = o[i].poffsets[j];

		  if (j < nheadings)
		    {
		      while (olevs > j)
			fputs("</div>",fp), --olevs;
		      if (outline_keys[j+1])
			{
			  
			}
		      else
			{
			  fprintf(fp, "<h%d>%c%s</h%d>",j+2,
				  toupper(sip->pool[o[i].poffsets[j]]),
				  (char*)xmlify(&sip->pool[o[i].poffsets[j]+1]),
				  j+2);
			}
		      fprintf(fp,"<div class=\"pgotl level%d\">",++olevs);
		    }
		  else
		    {
		      if (p3)
			fprintf(fp, "<p%s><a href=\"javascript://\" onclick=\"p3zoom(%d)\">%s [%ld]</a></p>", 
				zoomstr,
				o[i].sic->id + 1,
				(char*)xmlify(&sip->pool[o[i].poffsets[j]]),
				(long)o[i].count);
		      else
			fprintf(fp, "<p%s><a href=\"javascript://\" onclick=\"pz(1,%d)\">%s [%ld]</a></p>", 
				zoomstr,
				o[i].sic->id + 1, 
				(char*)xmlify(&sip->pool[o[i].poffsets[j]]), 
				(long)o[i].count);
		    }
		  ++j;
		}
	    }
	}
    }
  while (olevs-- > 0)
    fputs("</div>",fp);
  fputs("</div>\n",fp);
  if (xml_outline && !fragment)
    fputs("</body></html>",fp);
}
