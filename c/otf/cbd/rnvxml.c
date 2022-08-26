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

extern int rnx_n_exp;
/*static int nexp,rnck;*/
/*static int current,previous;*/

static Hash_table *cbd_qnames = NULL;
static Hash_table *cbd_qanames = NULL;

static const char **xmlns_atts;

extern int (*er_printf)(char *format,...);
extern int (*er_vprintf)(char *format,...);

#if 0
static const char *mytext;
static void
print_error_text()
{
  if (mytext && *mytext)
    {
      char buf[32];
      int i;
      strcpy(buf,"near '");
      for (i = 0; i < 10 && mytext[i]; ++i)
	buf[i+6] = mytext[i];
      if (mytext[i])
	strcpy(buf+i+6,"'...");
      else
	strcpy(buf+i+6, "'");
      (*er_printf)("%s: ", buf);
    }
  else
    (*er_printf)("near '': ");
}
#endif

extern locator *xo_loc;
#define xvh_err(msg) msglist_averr(xo_loc,(msg),ap);
static void xo_verror_handler(int erno,va_list ap)
{
  if(erno&ERBIT_RNL)
    {
      rnl_verror_handler(erno&~ERBIT_RNL,ap);
    }
  else
    {
      const char *xphase = phase;
      phase = "rnv";
      switch(erno)
	{
	case RNV_ER_ELEM: xvh_err("element %s^%s not allowed"); break;
	case RNV_ER_AKEY: xvh_err("attribute %s^%s not allowed"); break;
	case RNV_ER_AVAL: xvh_err("attribute %s^%s with invalid value \"%s\""); break;
	case RNV_ER_EMIS: xvh_err("incomplete content"); break;
	case RNV_ER_AMIS: xvh_err("missing attributes of %s^%s"); break;
	case RNV_ER_UFIN: xvh_err("unfinished content of element %s^%s"); break;
	case RNV_ER_TEXT: xvh_err("invalid data or text not allowed"); break;
	case RNV_ER_NOTX: xvh_err("text not allowed"); break;
	default: assert(0);
	}
      phase = xphase;
    }
}

void
rnvxml_rnvif_init()
{
  rnl_set_verror_handler(xo_verror_handler);
  rnv_set_verror_handler(xo_verror_handler);
}

void
rnvxml_init(struct xnn_xname *enames, struct xnn_xname *anames, const char *ns_atts[])
{
  int i;

  rnvxml_rnvif_init(); /* replacement for rnvif_init() */

  xmlns_atts = ns_atts;
  
  cbd_qnames = hash_create(1024);
  for (i = 0; enames[i].pname[0]; ++i)
    hash_add(cbd_qnames, (ucp)enames[i].pname, enames[i].qname);
  cbd_qanames = hash_create(1024);
  for (i = 0; anames[i].pname[0]; ++i)
    hash_add(cbd_qanames, (ucp)anames[i].pname, anames[i].qname);
}

void
rnvxml_term()
{
  hash_free(cbd_qnames, NULL);
  cbd_qnames = NULL;
}

void
rnvxml_ch(const char *ch)
{
  rnv_characters(NULL, ch, strlen(ch));
  fputs(ch,f_xml);
}

/* To add xmlns info set xmlns_atts to point to a const char *[]
 * consisting of the normal name/value pairs for an atts array
 */
void
rnvxml_ea(const char *pname, ...)
{
  char **atts = NULL, **qatts = NULL, *arg;
  char *qname;
  int nargs = 0, atts_alloced = 32;
  va_list ap;
  struct npool *rnvxml_pool = NULL;

  if (!(qname = hash_find(cbd_qnames, (ucp)pname)))
    {
      fprintf(stderr, "rnvxml: internal error: pname %s not found in qname table\n", pname);
      return;
    }

  atts = malloc(atts_alloced);
  qatts = malloc(atts_alloced);
  rnvxml_pool = npool_init();

  npool_copy((ucp)qname, rnvxml_pool);

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
	  char *qarg = hash_find(cbd_qanames, (ucp)arg);
	  atts[nargs] = (char*)npool_copy((ucp)arg, rnvxml_pool);
	  if (qarg)
	    qatts[nargs] = (char*)npool_copy((ucp)qarg, rnvxml_pool);
	  else
	    qatts[nargs] = (char*)npool_copy((ucp)arg, rnvxml_pool);
	}
      else /* odd numbered args are values */
	{
	  qatts[nargs] = atts[nargs] = (char*)npool_copy(xmlify((ucp)arg), rnvxml_pool);
	}
      ++nargs;
    }
  va_end(ap);
  atts[nargs] = qatts[nargs] = NULL;

  rnv_start_element(NULL,qname,(const char **)qatts);

  fprintf(f_xml, "<%s", pname);
  if (xmlns_atts)
    {
      int i;
      for (i = 0; xmlns_atts[i]; i += 2)
	fprintf(f_xml, " %s=\"%s\"", xmlns_atts[i], xmlns_atts[i+1]);
      xmlns_atts = NULL;
    }
  if (nargs > 1)
    {
      for (nargs = 0; atts[nargs]; nargs += 2)
	fprintf(f_xml, " %s=\"%s\"", atts[nargs], atts[nargs+1]);
    }
  fprintf(f_xml, ">");

  free(atts);
  npool_term(rnvxml_pool);
}

void
rnvxml_ee(const char *pname)
{
  char *qname = hash_find(cbd_qnames, (ucp)pname);
  if (qname)
    rnv_end_element(NULL,qname);
  fprintf(f_xml, "</%s>", pname);
}

