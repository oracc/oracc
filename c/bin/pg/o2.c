#include <stdio.h>
#include <stdlib.h>
#include "sortinfo.h"
#include "pg.h"

/* New version of the outliner for use with the collapsible outline
   JS.  This needs ul/li/ul nesting and class="outline" on the top
   ul only */

extern int zoomid;
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

void
pg_outline_dump(FILE *fp, struct outline *o, int nlevels)
{
  static u4 u4max = 0xffffffff;
  int i, j, k;
  u4 *lastlevs = malloc(nlevels * sizeof(u4*));
  int olevs = 0;
  int nheadings = nlevels - 1;

  for (i = 0; i < nlevels; ++i)
    lastlevs[i] = u4max;
  fputs("<ul class=\"outline\">\n",fp);
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
		    fputs("</ul>\n",fp), --olevs;
		  fprintf(fp, "<h%d>%c%s</h%d>",j+2,
			  toupper(sip->pool[o[i].poffsets[j]]),
			  (char*)&sip->pool[o[i].poffsets[j]+1],
			  j+2);
		  fprintf(fp,"<ul class=\"pgotl\">\n",++olevs);
		}
	      else
		{
		  fprintf(fp, "<li%s><a href=\"javascript://\" onclick=\"pz(1,%d)\">%s [%ld]</a></li>", 
			  zoomstr,
			  o[i].sic->id + 1,
			  (char*)&sip->pool[o[i].poffsets[j]], o[i].count);
		}
	      ++j;
	      while (j < nlevels)
		{
		  lastlevs[j] = o[i].poffsets[j];

		  if (j < nheadings)
		    {
		      while (olevs > j)
			fputs("</ul>\n",fp), --olevs;
		      if (outline_keys[j+1])
			{
			  
			}
		      else
			{
			  fprintf(fp, "<h%d>%c%s</h%d>",j+2,
				  toupper(sip->pool[o[i].poffsets[j]]),
				  (char*)&sip->pool[o[i].poffsets[j]+1],
				  j+2);
			}
		      fprintf(fp,"<ul class=\"pgotl level%d\">\n",++olevs);
		    }
		  else
		    {
		      fprintf(fp, "<li%s><a href=\"javascript://\" onclick=\"pz(1,%d)\">%s [%ld]</a></li>", 
			      zoomstr,
			      o[i].sic->id + 1, 
			      (char*)&sip->pool[o[i].poffsets[j]], o[i].count);
		    }
		  ++j;
		}
	    }
	}
    }
  while (olevs-- > 0)
    fputs("</ul>\n",fp);
  fputs("</ul>\n",fp);
}
