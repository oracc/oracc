#include <stdio.h>
#include "sortinfo.h"
#include "pg.h"

extern struct sortinfo*sip;

void
dbg_dump_items(struct item *items, int nitems)
{
  int i;
  for (i = 0; i < nitems; ++i)
    fprintf(fdbg,"%s\t%d\t%ld\n",items[i].s,items[i].grp,(long)items[i].skey);
}
void
dbg_dump_outline(struct outline *op, int nlevels)
{
  int i;
  for (i = 0; i < sic_size; ++i)
    {
      int j;
      fprintf(fdbg,"pg=%ld\tct=%ld\t", (long)op[i].page, (long)op[i].count);
      for (j = 0; j < nlevels; ++j)
	{
	  fprintf(fdbg,"%ld\t",(long)op[i].poffsets[j]);
	}
      fputc('\n',fdbg);
    }
}
void
dbg_dump_pitems(struct item **pitems, int nitems)
{
  int i;
  for (i = 0; i < nitems; ++i)
    fprintf(fdbg,"%s\t%d\t%ld\n",pitems[i]->s,pitems[i]->grp,(long)pitems[i]->skey);
}
void
dbg_dump_sicache(struct si_cache**sic, int nitems)
{
  int i;
  for (i = 0; i < nitems; ++i)
    {
      int j;
      fprintf(fdbg,"%ld %d",(long)sic[i]->id,sic[i]->seq);
      fprintf(fdbg," P%06ld",
	      (long) sip->idlist[(sic[i]->codes - sip->scodes) / sip->nfields]);
      for (j = 0; j < sip->nfields; ++j)
	fprintf(fdbg," %d=%ld",j,(long)sic[i]->codes[j]);
      fprintf(fdbg,"\n");
    }
}
void
dbg_show_codes(const char *msg, int id, int idindex, s4*codes)
{
  int j;
  fprintf(fdbg,"%s: %c%06ld",msg,(idindex<sip->qoffset)?'P':'Q',(long)id);
  for (j = 0; j < sip->nfields; ++j)
    fprintf(fdbg," %d=%ld",j,(long)codes[j]);
  fprintf(fdbg,"\n");
}
