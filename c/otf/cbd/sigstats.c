#include "gx.h"
#include "sx.h"
#include <stdlib.h>

#define JOINER ''

void
stats_eb(unsigned char *cgp, struct f2*f2p, Hash_table *stats, char *iid)
{
  int len = strlen((ccp)cgp) + strlen((ccp)f2p->base) + 3;
  char *buf = NULL;
  buf = malloc(len);
  sprintf(buf, "%s%c%s", cgp, JOINER, f2p->base);
  printf("%s\teb\t%s\n", cgp, buf);
}

void
sigstat_print()
{
}

void
sigstats(const char *f)
{
  FILE *f_in = NULL;
  unsigned char *lp = NULL, *curr_cgp = NULL;
  char iid[16];
  struct npool *pool = NULL;
  Hash_table *stats = NULL, *iid_hash = NULL;
  int ninsts = 0, inst_id = 0, lnum = 0;
  struct sig_context *scp;
  struct f2 f2;

  if (strcmp(f, "-"))
    f_in = xfopen(f, "r");
  else
    f_in = stdin;

  scp = sig_context_init();
    
  while ((lp = xgetline(f_in)))
    {
      ++lnum;
      if ('@' == lp[0] && '@' == lp[1])
	{
          unsigned char *tmp = NULL;

          if (curr_cgp)
            {
              sigstat_print(curr_cgp, ninsts, stats, iid_hash);
              npool_term(pool);
              hash_free(stats, NULL);
            }

          tmp = (ucp)strchr((ccp)lp, '\t');
          if (tmp)
            *tmp++ = '\0';
	  /* tmp is now pointing at the number of sigs that follow this cgp; we don't use that atm */
	  tmp = (ucp)strchr((ccp)lp, '\t');
	  if (tmp)
	    {
	      *tmp++ = '\0';
	      /* now tmp is pointing at the instance count for the cgp */
	      ninsts = atoi((ccp)tmp);
	    }
          pool = npool_init();
          stats = hash_create(1024);
	  iid_hash = hash_create(1024);
          curr_cgp = npool_copy((ucp)lp, pool);
          ninsts = 0;
	}
      else
        {
          unsigned char *sig = lp, *freq, *inst = NULL;
          freq = (ucp)strchr((const char *)sig,'\t');
          if (freq)
            {
              *freq++ = '\0';
              inst = (ucp)strchr((ccp)freq, '\t');
	      if (inst)
		{
		  *inst++ = '\0';
		  if (strchr((ccp)inst, '\t'))
		    {
		      fprintf(stderr, "sx -m : input lines only allowed <SIG><TAB><INSTS>\n");
		      exit(1);
		    }
		}
	    }
	  if (inst)
	    {
	      sprintf(iid, "i%05x", inst_id++);
	      hash_add(iid_hash, npool_copy((ucp)iid,pool), npool_copy(inst,pool));
	    }
	  if (f2_parse((ucp)f,lnum,lp,&f2,NULL,scp) > 0)
	    {
	      stats_eb(curr_cgp, &f2, stats, iid);
	    }
	}
    }
  if (curr_cgp)
    {
      sigstat_print(curr_cgp, ninsts, stats);
      npool_term(pool);
      hash_free(stats, NULL);
      hash_free(iid_hash, NULL);
    }
  xgetline(NULL);
  xfclose(f,f_in);
  sig_context_term();
}
