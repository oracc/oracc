#include "gx.h"
#include "sx.h"
#include <stdlib.h>

#define JOINER ''

static Hash_table *h_eb = NULL;

void
stats_eb(unsigned char *cgp, struct f2*f2p, const char *iid, struct npool *pool)
{
  int len = strlen((ccp)cgp) + strlen((ccp)f2p->base) + 3;
  char *buf = NULL;
  List *lp = NULL;
  buf = malloc(len);
  sprintf(buf, "%s%c%s", cgp, JOINER, f2p->base);
  if (!(lp = hash_find(h_eb, (ucp)buf)))
    {
      lp = list_create(LIST_SINGLE);
      hash_add(h_eb, npool_copy((ucp)buf, pool), lp);
    }
  list_add(lp, (void*)iid);
  /*printf("%s\teb\t%s\n", cgp, buf);*/
}

void
sigstat_print(unsigned char *curr_cgp, int ninsts, Hash_table *iid_hash)
{
  const char **k = NULL;
  int nk = 0, i;
  printf("@@%s\t%d\n", curr_cgp, ninsts);
  k = hash_keys2(h_eb, &nk);
  for (i = 0; i < nk; ++i)
    {
      List *lp = hash_find(h_eb, (ucp)k[i]);
      List_node *rover = NULL;
      printf("%s\t", k[i]);
      for (rover = lp->first; rover; rover = rover->next)
	{
	  printf("%s [%s] ", (char*)(rover->data), (char*)hash_find(iid_hash, (const unsigned char*)(rover->data)));
	}
      printf("\n");
      list_free(lp, NULL);
    }
}

void
sigstats(const char *f)
{
  FILE *f_in = NULL;
  unsigned char *lp = NULL, *curr_cgp = NULL;
  char iid[16];
  const char *iid_copy;
  struct npool *pool = NULL;
  Hash_table *iid_hash = NULL;
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
              sigstat_print(curr_cgp, ninsts, iid_hash);
              npool_term(pool);
	      hash_free(h_eb, NULL);
              hash_free(iid_hash, NULL);
            }

          tmp = (ucp)strchr((ccp)lp, '\t');
          if (tmp)
            *tmp++ = '\0';
	  /* tmp is now pointing at the number of sigs that follow this cgp; we don't use that atm */
	  tmp = (ucp)strchr((ccp)tmp, '\t');
	  if (tmp)
	    {
	      *tmp++ = '\0';
	      /* now tmp is pointing at the instance count for the cgp */
	      ninsts = atoi((ccp)tmp);
	    }
          pool = npool_init();
          h_eb = hash_create(1024);
	  iid_hash = hash_create(1024);
          curr_cgp = npool_copy((ucp)lp+2, pool);
	  inst_id = 0;
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
	      hash_add(iid_hash, (ucp)(iid_copy = (ccp)npool_copy((ucp)iid,pool)), npool_copy(inst,pool));
	      /* fprintf(stderr, "added inst %s with iid %s\n", inst, iid); */
	      memset(&f2, '\0', sizeof(struct f2));
	      if (f2_parse((ucp)f,lnum,lp,&f2,NULL,scp) > 0)
		{
		  stats_eb(curr_cgp, &f2, iid_copy, pool);
		}
	    }
	}
    }
  if (curr_cgp)
    {
      sigstat_print(curr_cgp, ninsts, iid_hash);
      npool_term(pool);
      hash_free(h_eb, NULL);
      hash_free(iid_hash, NULL);
    }
  xgetline(NULL);
  xfclose(f,f_in);
  sig_context_term();
}
