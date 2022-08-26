#include <stdio.h>
#include "rnvif.h"
#include "xmlutil.h"
#include "npool.h"
#include "gx.h"
#include "xnn.h"
#include "../rnv/erbit.h"
#include "../rnv/m.h"
#include "../rnv/rnv.h"
#include "../rnv/rnx.h"
#include "rnvval.h"

extern int rnx_n_exp;
/*static int nexp,rnck;*/
/*static int current,previous;*/

static Hash_table *rnv_qnames = NULL;
static Hash_table *rnv_qanames = NULL;

static struct npool *rnv_pool;

void
rnvval_init(void (*eh)(int erno,va_list ap), struct xnn_data *xdp)
{
  int i;

  /* rnvxml_rnvif_init(); */ /* replacement for rnvif_init() */

  rnl_set_verror_handler(eh);
  rnv_set_verror_handler(eh);

  rnv_qnames = hash_create(1024);
  for (i = 0; xdp->enames[i].pname; ++i)
    hash_add(rnv_qnames, (ucp)xdp->enames[i].pname, xdp->enames[i].qname);
  rnv_qanames = hash_create(1024);
  for (i = 0; xdp->anames[i].pname; ++i)
    hash_add(rnv_qanames, (ucp)xdp->anames[i].pname, xdp->anames[i].qname);

  rnv_pool = npool_init();
}

void
rnvval_term()
{
  hash_free(rnv_qnames, NULL);
  rnv_qnames = NULL;
  hash_free(rnv_qnames, NULL);
  rnv_qanames = NULL;
  npool_term(rnv_pool);
}

void
rnvval_ch(const char *ch)
{
  rnv_characters(NULL, ch, strlen(ch));
}

void
rnvval_free_atts(struct rnvval_atts *ratts)
{
  free(ratts->atts);
  free(ratts->qatts);
  free(ratts);
}

struct rnvval_atts *
rnvval_aa(const char *pname, ...)
{
  char **atts = NULL, **qatts = NULL, *arg;
  int nargs = 0, atts_alloced = 32;
  va_list ap;
  struct rnvval_atts *ratts = malloc(sizeof(struct rnvval_atts));

  atts = malloc(atts_alloced);
  qatts = malloc(atts_alloced);
  npool_reset(rnv_pool);

  va_start(ap, pname);
  while ((arg = (char*)va_arg(ap, const char*)))
    {
      if (NULL == arg)
	break;

      if (atts_alloced - nargs < 3)
	{
	  atts = realloc(atts, atts_alloced *= 2);
	  qatts = realloc(qatts, atts_alloced);
	}

      if ((nargs%2) == 0) /* even numbered args are names */
	{
	  char *qarg = hash_find(rnv_qanames, (ucp)arg);
	  atts[nargs] = (char*)npool_copy((ucp)arg, rnv_pool);
	  if (qarg)
	    qatts[nargs] = (char*)npool_copy((ucp)qarg, rnv_pool);
	  else
	    qatts[nargs] = (char*)npool_copy((ucp)arg, rnv_pool);
	}
      else /* odd numbered args are values */
	{
	  qatts[nargs] = atts[nargs] = (char*)npool_copy(xmlify((ucp)arg), rnv_pool);
	}
      ++nargs;
    }
  va_end(ap);
  atts[nargs] = qatts[nargs] = NULL;
  ratts->atts = (const char **)atts;
  ratts->qatts = (const char **)qatts;
  return ratts;
}

void
rnvval_ea(const char *pname, struct rnvval_atts *ratts)
{
  char *qname = NULL;
  if (!(qname = hash_find(rnv_qnames, (ucp)pname)))
    {
      fprintf(stderr, "rnvval: internal error: pname %s not found in qname table\n", pname);
      return;
    }

  rnv_start_element(NULL,qname,ratts->qatts);
}

void
rnvval_ee(const char *pname)
{
  char *qname = hash_find(rnv_qnames, (ucp)pname);
  if (qname)
    rnv_end_element(NULL,qname);
}
