#include <stdio.h>
#include <assert.h>
#include "rnvif.h"
#include "xmlify.h"
#include "list.h"
#include "mesg.h"
#include "pool.h"
#include "xnn.h"
#include "rnvval.h"
#include "rnv/erbit.h"
#include "rnv/m.h"
#include "rnv/rnv.h"
#include "rnv/rnx.h"
#include "rnvxml.h"

extern FILE *f_xml;
extern int rnx_n_exp;

static struct pool *xgi_pool;
static List *xgi_stack;
static char xgi_flags[5];

/*static struct rnvval_atts *xgi_ratts;*/

/*static int nexp,rnck;*/
/*static int current,previous;*/

static struct xnn_nstab *xmlns_atts;

extern int (*er_printf)(char *format,...);
extern int (*er_vprintf)(char *format,...);

extern Mloc *xo_loc;
#define xvh_err(msg) mesg_averr(xo_loc,(msg),ap);
static void xgi_verror_handler(int erno,va_list ap)
{
  if(erno&ERBIT_RNL)
    {
      rnl_verror_handler(erno&~ERBIT_RNL,ap);
    }
  else
    {
      const char *xphase = phase, *xm;
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
      xm = rnv_xmsg();
      mesg_append((char*)pool_copy((uccp)xm, xgi_pool));
      free((void*)xm);
      phase = xphase;
    }
}

void
rnvxml_init_err(void)
{
  rnvval_init_err(xgi_verror_handler);
}

void
rnvxml_init_tree(Tree *tp, struct xnn_data *xdp, const char *rncbase)
{
  rnvxml_init(xdp, rncbase);
}

void
rnvxml_init(struct xnn_data *xdp, const char *rncbase)
{
  if (f_xml == NULL)
    f_xml = stdout;
  rnvval_init(xdp, rncbase);
  xmlns_atts = xdp->nstab;
  xgi_pool = pool_init();
  xgi_stack = list_create(LIST_LIFO);
  xgi_flags[0] = '\0';
  rnv_validate_start();      
}

void
rnvxml_term(/*Tree *tp*/)
{
  /*rnvval_ee(tp->root->name);*/
}

void
rnvxml_ch(const char *ch)
{
  rnvval_ch(ch);
  fputs(ch,f_xml);
}

void
rnvxml_ea(const char *pname, struct rnvval_atts *ratts)
{
  int i;
  rnvval_ea(pname, ratts);
  fprintf(f_xml, "<%s", pname);
  if (xmlns_atts)
    {
      int i;
      for (i = 0; xmlns_atts[i].ns; ++i)
	fprintf(f_xml, " %s=\"%s\"", xmlns_atts[i].ns, xmlns_atts[i].uri);
      xmlns_atts = NULL;
    }

  if (ratts)
    for (i = 0; ratts->atts[i]; i+=2)
      fprintf(f_xml, " %s=\"%s\"", ratts->atts[i], ratts->atts[i+1]);

  fprintf(f_xml, ">");

  if (ratts)
    rnvval_free_atts(ratts);
}

void
rnvxml_ee(const char *pname)
{
  rnvval_ee(pname);
  fprintf(f_xml, "</%s>", pname);
}

void
rnvxml_ec(const char *pname, struct rnvval_atts *ratts)
{
  rnvxml_ea(pname, ratts);
  rnvxml_ee(pname);
}

void
rnvxml_et(const char *pname, struct rnvval_atts *ratts, const char *ch)
{
  rnvxml_ea(pname, ratts);
  rnvxml_ch(ch);
  rnvxml_ee(pname);
}
