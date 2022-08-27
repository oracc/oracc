#include <stdarg.h>
#include <string.h>
#include <ctype128.h>
#include "gx.h"
#include "xnn.h"
#include "rnvif.h"
#include "rnvval.h"
#include "npool.h"
#include "list.h"
#include "../rnv/erbit.h"
#include "../rnv/m.h"
#include "../rnv/rnl.h"
#include "../rnv/rnv.h"
#include "../rnv/rnx.h"

static struct npool *tgi_pool;
static List *tgi_stack;
static char tgi_flags[5];
static struct rnvval_atts *tgi_ratts;
static List *tgi_ch;

/* rnvtgi: Driver for rnv validation of T(ext) G(lossary) I(nfosets) */

extern char *rnv_xmsg(void);

extern int (*er_printf)(char *format,...);
extern int (*er_vprintf)(char *format,...);
static locator tgi_loc;
#define xvh_err(msg) msglist_averr(&tgi_loc,(msg),ap);
static void tgi_verror_handler(int erno,va_list ap)
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
	case RNV_ER_ELEM: xvh_err("tag %s^%s not allowed"); break;
	case RNV_ER_AKEY: xvh_err("tag modifier %s^%s not allowed"); break;
	case RNV_ER_AVAL: xvh_err("tag modifier %s^%s with invalid value \"%s\""); break;
	case RNV_ER_EMIS: xvh_err("incomplete content"); break;
	case RNV_ER_AMIS: xvh_err("missing attributes of %s^%s"); break;
	case RNV_ER_UFIN: xvh_err("unfinished content of tag %s^%s"); break;
	case RNV_ER_TEXT: xvh_err("invalid data or text not allowed"); break;
	case RNV_ER_NOTX: xvh_err("text not allowed"); break;
	default: assert(0);
	}
      xm = rnv_xmsg();
      msglist_append(npool_copy(xm, tgi_pool));
      free(xm);
      phase = xphase;
    }
}

void
rnvtgi_init_err(void)
{
  rnvval_init_err(tgi_verror_handler);
}

void
rnvtgi_init(struct xnn_data *xdp, const char *rncbase)
{
  char *fn = malloc(strlen(rncbase)+5);
  sprintf(fn, "%s.rnc", rncbase);
  rnvval_init(xdp, fn);
  free(fn);
  tgi_pool = npool_init();
  tgi_stack = list_create(LIST_LIFO);
  tgi_ch = list_create(LIST_SINGLE);
  tgi_flags[0] = '\0';
  rnv_validate_start();      
  rnvval_ea("cbd", NULL);
}

void
rnvtgi_term()
{
  rnvval_ee("cbd");
  rnv_validate_finish();
  list_free(tgi_ch, NULL);
  list_free(tgi_stack, NULL);
  npool_term(tgi_pool);
}

static void
tgi_flag_attr(void)
{
  int i;
  char *at[8];
  for (i = 0; tgi_flags[i]; ++i)
    {
      switch (tgi_flags[i])
	{
	case '+':
	  at[i*2] = "plus";
	  break;
	case '-':
	  at[i*2] = "minus";
	  break;
	case '!':
	  at[i*2] = "bang";
	  break;
	case '*':
	  at[i*2] = "star";
	  break;
	}
      at[(i*2)+1] = "yes";      
    }
  at[(i*2)] = NULL;
  tgi_ratts = rnvval_aa_qatts(at, i);
  tgi_flags[0] = '\0';
}

static void
rnvtgi_ch(void)
{
  if (list_len(tgi_ch))
    {
      unsigned char *s = list_concat(tgi_ch); 
      unsigned char *tok = NULL, *end;
      int i;      
      tok = s;
      while (isspace(*tok))
	++tok;
      end = tok + strlen(tok);
      while (end > tok && isspace(end[-1]))
	*--end = '\0';
      for (i = 0; tok[i]; ++i)
	if (!isspace(tok[i]))
	  break;
      if (tok[i])
	rnvval_ch(npool_copy(tok, tgi_pool));
      free(s);
      list_reset(tgi_ch);
    }
}

void
rnvtgi_token(const char *tfile, int lno, int sstate, char *tok)
{
  tgi_loc.file = (char*)tfile;
  tgi_loc.first_line = lno;
  if (tok[0] == '@')
    {
      char *tag = (char*)npool_copy((ucp)tok+1,tgi_pool);
      char *open = list_pop(tgi_stack);
      if (open)
	{
	  rnvtgi_ch();
	  rnvval_ee(open);
	  msglist_print(stderr);
	  msglist_init();
	}
      if (!strncmp(tok, "@end", 4))
	{
	  tok += 4;
	  while (isspace(*tok))
	    ++tok;
	  open = list_pop(tgi_stack); /* this should be entry */
	  rnvtgi_ch(); /* probably can't happen */
	  rnvval_ee(open);
	}
      else
	{
	  if (*tgi_flags)
	    tgi_flag_attr();
	  else
	    tgi_ratts = NULL;
	  list_push(tgi_stack, tag);
	  rnvtgi_ch();
	  rnvval_ea(tag,tgi_ratts);
	  if (!strcmp(tag, "entry"))
	    {
	      list_push(tgi_stack, "cgp");
	      rnvval_ea("cgp",NULL);
	    }	    
	}
    }
  else if (strlen(tok) == 1 && strchr("-+!*", *tok))
    {
      if (strlen(tgi_flags) < 4)
	strcat(tgi_flags, tok);
    }
  else
    {
      list_add(tgi_ch, npool_copy(tok,tgi_pool));
    }
  msglist_print(stderr);
  msglist_init();
}
