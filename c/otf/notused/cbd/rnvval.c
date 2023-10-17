#include <stdio.h>
#include <unistd.h>
#include <psd_base.h>
#include "rnvif.h"
#include "xmlutil.h"
#include "npool.h"
#include "gx.h"
#include "xnn.h"
#include "../rnv/erbit.h"
#include "../rnv/m.h"
#include "../rnv/rnl.h"
#include "../rnv/rnv.h"
#include "../rnv/rnx.h"
#include "rnvval.h"

extern int rnvtrace;
extern int rnx_n_exp;

/*static int nexp,rnck;*/
/*static int current,previous;*/

static Hash_table *rnv_qnames = NULL;
static Hash_table *rnv_qanames = NULL;

static struct npool *rnv_pool;

void
rnvval_init_err(void (*eh)(int erno,va_list ap))
{
  rnl_set_verror_handler(eh);
  rnv_set_verror_handler(eh);
}
void
rnvval_init(struct xnn_data *xdp, char *rncfile)
{
  int i;

  if (!xaccess(rncfile, R_OK, 0))
    {
      if (verbose)
	fprintf(stderr, "rnvval_init: using rnc schema %s\n", rncfile);
      rnc_start = rnl_fn(rncfile);
      status = !rnc_start;
    }
  else
    {
      fprintf(stderr, "rnvval_fn: no such schema %s\n", rncfile);
    }

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
  if (rnvtrace)
    fprintf(stderr, "rnv-ch: ::%s::\n", ch);
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
rnvval_aa_qatts(char **atts, int natts)
{
  int i;
  char **qatts = malloc((2*natts) * sizeof(char*));
  struct rnvval_atts *ratts = malloc(sizeof(struct rnvval_atts));
  for (i = 0; i < natts; ++i)
    {
      qatts[i*2] = hash_find(rnv_qanames, (ucp)atts[i*2]);
      qatts[1+(i*2)] = atts[1+(i*2)];
    }
  ratts->atts = (const char **)atts;
  ratts->qatts = (const char **)qatts;
  return ratts;
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
  char *qatts[] = { NULL };
  if (!(qname = hash_find(rnv_qnames, (ucp)pname)))
    {
      fprintf(stderr, "rnvval: internal error: pname %s not found in qname table\n", pname);
      return;
    }
  if (rnvtrace)
    fprintf(stderr, "rnv-ea: %s\n", qname);
  rnv_start_element(NULL,(char*)npool_copy((ucp)qname, rnv_pool),ratts ? ratts->qatts : (const char**)qatts);
}

void
rnvval_ee(const char *pname)
{
  char *qname = hash_find(rnv_qnames, (ucp)pname);
  if (qname)
    {
      if (rnvtrace)
	fprintf(stderr, "rnv-ee: %s\n", qname);
      rnv_end_element(NULL,(char*)npool_copy((ucp)qname,rnv_pool));
    }
}
