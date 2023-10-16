#include <stdio.h>
#include <unistd.h>
#include <oraccsys.h>
#include "rnvif.h"
#include "xmlify.h"
#include "hash.h"
#include "pool.h"
#include "xnn.h"
#include "rnv/erbit.h"
#include "rnv/m.h"
#include "rnv/rnl.h"
#include "rnv/rnv.h"
#include "rnv/rnx.h"
#include "rnvval.h"

extern int status;
extern int verbose;

extern int rnvtrace;
extern int rnx_n_exp;

/*static int nexp,rnck;*/
/*static int current,previous;*/

static Hash *rnv_qnames = NULL;
static Hash *rnv_qanames = NULL;

static Pool *rnv_pool;

static char *
rncfile_path(const char *r)
{
  char *p = malloc(strlen(oracc_builds())+strlen("/lib/rnc/")+strlen(r)+5);
  sprintf(p, "%s/lib/rnc/%s.rnc", oracc_builds(), r);
  return p;
}

void
rnvval_init_err(void (*eh)(int erno,va_list ap))
{
  rnl_set_verror_handler(eh);
  rnv_set_verror_handler(eh);
}

void
rnvval_init(struct xnn_data *xdp, const char *rncfile)
{
  int i;
  char *p = rncfile_path(rncfile);
  
  if (!access(p, R_OK)) /*!xaccess(rncfile, R_OK, 0)*/
    {
      if (verbose)
	fprintf(stderr, "rnvval_init: using rnc schema %s\n", p);
      rnc_start = rnl_fn(p);
      status = !rnc_start;
    }
  else
    {
      fprintf(stderr, "rnvval_fn: no such schema %s\n", p);
      return;
    }

  rnv_qnames = hash_create(1024);
  for (i = 0; xdp->enames[i].pname; ++i)
    hash_add(rnv_qnames, (ucp)xdp->enames[i].pname, xdp->enames[i].qname);
  rnv_qanames = hash_create(1024);
  for (i = 0; xdp->anames[i].pname; ++i)
    hash_add(rnv_qanames, (ucp)xdp->anames[i].pname, xdp->anames[i].qname);

  rnv_pool = pool_init();
}

void
rnvval_term(void)
{
  hash_free(rnv_qnames, NULL);
  rnv_qnames = NULL;
  hash_free(rnv_qnames, NULL);
  rnv_qanames = NULL;
  pool_term(rnv_pool);
}

void
rnvval_free_atts(struct rnvval_atts *ratts)
{
  free(ratts->atts);
  free(ratts->qatts);
  free(ratts);
}

/* Attributes are passed to rnv validation in an rnvval_atts
   structure.  This routine creates an rnvval_atts structure from a
   lengthed list of char * where even numbered (from 0) are names and
   odd numbered are values.

   natts should be the number of attributes, not the number of char *
   in atts.
 */
struct rnvval_atts *
rnvval_aa_qatts(char **atts, int natts)
{
  int i;
  char **qatts = malloc((1+(2*natts)) * sizeof(char*));
  struct rnvval_atts *ratts = malloc(sizeof(struct rnvval_atts));
  pool_reset(rnv_pool);

  for (i = 0; i < natts; ++i)
    {
      /* must copy rnv_qanames because they are in const storage */
      qatts[i*2] = (char*)pool_copy(hash_find(rnv_qanames, (ucp)atts[i*2]), rnv_pool);
      qatts[1+(i*2)] = atts[1+(i*2)];
    }
  qatts[i*2] = NULL;
  ratts->atts = (const char **)atts;
  ratts->qatts = (const char **)qatts;
  return ratts;
}

/* This function creates an rnvval_atts structure from a
   NULL-terminated list of char * which are alternating name/value
   pairs */
struct rnvval_atts *
rnvval_aa(const char *pname, ...)
{
  char **atts = NULL, **qatts = NULL, *arg;
  int nargs = 0, atts_alloced = 32;
  va_list ap;
  struct rnvval_atts *ratts = malloc(sizeof(struct rnvval_atts));

  atts = malloc(atts_alloced * sizeof(char*));
  qatts = malloc(atts_alloced * sizeof(char*));
  pool_reset(rnv_pool);

  va_start(ap, pname);
  while ((arg = (char*)va_arg(ap, const char*)))
    {
      if (atts_alloced - nargs < 3)
	{
	  atts_alloced *= 2;
	  atts = realloc(atts, atts_alloced * sizeof(char*));
	  qatts = realloc(qatts, atts_alloced * sizeof(char*));
	}

      if ((nargs%2) == 0) /* even numbered args are names */
	{
	  char *qarg = hash_find(rnv_qanames, (ucp)arg);
	  atts[nargs] = (char*)pool_copy((ucp)arg, rnv_pool);
	  if (qarg)
	    qatts[nargs] = (char*)pool_copy((ucp)qarg, rnv_pool);
	  else
	    qatts[nargs] = (char*)pool_copy((ucp)arg, rnv_pool);
	}
      else /* odd numbered args are values */
	{
	  qatts[nargs] = atts[nargs] = (char*)pool_copy(xmlify((ucp)arg), rnv_pool);
	}
      ++nargs;
    }
  if (NULL == arg)
    {
      if (nargs%2)
	{
	  fprintf(stderr, "rnvval: NULL value in rnvval_aa; returning NULL\n");
	  free(atts);
	  free(qatts);
	  free(ratts);
	  return NULL;
	}
    }
  va_end(ap);
  atts[nargs] = qatts[nargs] = NULL;
  ratts->atts = (const char **)atts;
  ratts->qatts = (const char **)qatts;
  return ratts;
}

/**Make an rnvval_atts from a NULL terminated array of char *
 *
 */
struct rnvval_atts *
rnvval_aa_ccpp(const char **a)
{
  char **atts = NULL, **qatts = NULL;
  int nargs = 0, atts_alloced = 32;
  struct rnvval_atts *ratts = NULL;

  if (!a || !*a)
    return NULL;
  
  ratts = malloc(sizeof(struct rnvval_atts));

  atts = malloc(atts_alloced * sizeof(char*));
  qatts = malloc(atts_alloced * sizeof(char*));
  pool_reset(rnv_pool);
  
  while (a[nargs])
    {
      const char *arg = a[nargs];
      if (atts_alloced - nargs < 3)
	{
	  atts_alloced *= 2;
	  atts = realloc(atts, atts_alloced * sizeof(char*));
	  qatts = realloc(qatts, atts_alloced * sizeof(char*));
	}
      if ((nargs%2) == 0) /* even numbered args are names */
	{
	  if (a[nargs+1])
	    {
	      char *qarg = hash_find(rnv_qanames, (ucp)arg);
	      atts[nargs] = (char*)pool_copy((ucp)arg, rnv_pool);
	      if (qarg)
		qatts[nargs] = (char*)pool_copy((ucp)qarg, rnv_pool);
	      else
		qatts[nargs] = (char*)pool_copy((ucp)arg, rnv_pool);
	    }
	  else
	    {
	      fprintf(stderr, "rnvval: NULL value in rnvval_aa_ccpp; returning NULL\n");
	      free(atts);
	      free(qatts);
	      free(ratts);
	      return NULL;
	    }
	}
      else /* odd numbered args are values */
	{
	  qatts[nargs] = atts[nargs] = (char*)pool_copy(xmlify((ucp)arg), rnv_pool);
	}
      ++nargs;
    }
  atts[nargs] = qatts[nargs] = NULL;
  ratts->atts = (const char **)atts;
  ratts->qatts = (const char **)qatts;
  return ratts;
}

/* rnvval_ea (element-attributes), rnvval_ee (end-element) , and
   rnvval_ch (characters) are the wrapper functions used for passing
   XML data to the rnv validator */
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
  rnv_start_element(NULL,(char*)pool_copy((ucp)qname, rnv_pool),
		    ratts ? ratts->qatts : (const char**)qatts);
}

void
rnvval_ee(const char *pname)
{
  char *qname = hash_find(rnv_qnames, (ucp)pname);
  if (qname)
    {
      if (rnvtrace)
	fprintf(stderr, "rnv-ee: %s\n", qname);
      rnv_end_element(NULL,(char*)pool_copy((ucp)qname,rnv_pool));
    }
}

void
rnvval_ch(const char *ch)
{
  if (rnvtrace)
    fprintf(stderr, "rnv-ch: ::%s::\n", ch);
  rnv_characters(NULL, ch, strlen(ch));
}
