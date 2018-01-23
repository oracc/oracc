#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <string.h> /*strerror*/
#include <errno.h>
#include <assert.h>
#include "tree.h"
#include "../rnv/m.h"
#include "../rnv/s.h"
#include "../rnv/erbit.h"
#include "../rnv/drv.h"
#include "../rnv/rnl.h"
#include "../rnv/rnc.h"
#include "../rnv/rnv.h"
#include "../rnv/rnx.h"
#include "../rnv/ll.h"
#include "../rnv/dxl.h"
#include "../rnv/dsl.h"
#include "../rnv/er.h"
#include "pool.h"

extern int rn_notAllowed,rx_compact,drv_compact;

#define LEN_T XCL_LEN_T
#define LIM_T XCL_LIM_T

#define BUFSIZE 1024

/* maximum number of candidates to display */
#define NEXP 16

#define XCL_ER_IO 0
#define XCL_ER_XML 1
#define XCL_ER_XENT 2

#define PIXTF2FILE "atf-file"
#define PIXTF2LINE "atf-line"

const char *rnvif_text_id = NULL;

static int verbose,nexp,rnck;
int rnc_start = -1;
static int current,previous;
static int mixed=0;
static int lastline,level;
extern const char *file;
/*static char *xgpos=NULL;*/
static int ok;

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

#define err(msg) (*er_vprintf)(msg"\n",ap);
static void verror_handler(int erno,va_list ap) {
  if(erno&ERBIT_RNL) {
    rnl_default_verror_handler(erno&~ERBIT_RNL,ap);
  } else {
    int line = tree_get_line();
#if 0
    if(line!=lastline) {
      lastline=line;
#endif
      
      if (rnvif_text_id)
	(*er_printf)("%s:%d:%s: XML error: ",file,line,rnvif_text_id);
      else
	(*er_printf)("%s:%d: XML error: ",file,line);
      if(erno&ERBIT_RNV) {
	if ((erno&~ERBIT_RNV) == RNV_ER_TEXT)
	  print_error_text();
	rnv_default_verror_handler(erno&~ERBIT_RNV,ap);
	if(nexp) { int req=2, i=0; char *s;
	  while(req--) {
	    rnx_expected(previous,req);
	    if(i==rnx_n_exp) continue;
	    if(rnx_n_exp>nexp) break;
	    (*er_printf)((char*)(req?"required:\n":"allowed:\n"));
	    for(;i!=rnx_n_exp;++i) {
	      (*er_printf)("\t%s\n",s=rnx_p2str(rnx_exp[i]));
	      m_free(s);
	    }
	  }
	}
      } else {
	switch(erno) {
	case XCL_ER_IO: err("%s"); break;
	case XCL_ER_XML: err("%s"); break;
	case XCL_ER_XENT: err("pipe through xx to expand external entities"); break;
	default: assert(0);
	}
      }
#if 0
    }
#endif
  }
}

static void verror_handler_rnl(int erno,va_list ap) {verror_handler(erno|ERBIT_RNL,ap);}
static void verror_handler_rnv(int erno,va_list ap) {verror_handler(erno|ERBIT_RNV,ap);}

static void windup(void);
static int initialized=0;

static void
init(void)
{
  if(!initialized) 
    {
      initialized=1;
      rnl_init(); rnl_verror_handler=&verror_handler_rnl;
      rnv_init(); rnv_verror_handler=&verror_handler_rnv;
      rnx_init();
      drv_add_dtl(DXL_URL,&dxl_equal,&dxl_allows);
      drv_add_dtl(DSL_URL,&dsl_equal,&dsl_allows);
      windup();
    }
}

static void
clear(void) {
  windup();
}

static void
windup(void)
{
  level=0; lastline=-1;
  mytext = "";
}

#if 0
static void error_handler(int erno,...) {
  va_list ap; va_start(ap,erno); verror_handler(erno,ap); va_end(ap);
}
#endif

static void
flush_text(void)
{
  ok = rnv_text(&current,&previous,(char *)mytext,strlen(mytext),mixed) && ok;
  mytext = "";
}

void
rnv_start_element(void *userData,const char *name,const char **attrs)
{
  if(current!=rn_notAllowed)
    {
      mixed=1;
      flush_text();
      ok = rnv_start_tag(&current,&previous,(char*)name,(char**)attrs) && ok;
      mixed=0;
    }
  else
    {
      ++level;
    }
}

void
rnv_end_element(void *userData,const char *name)
{
  if(current!=rn_notAllowed)
    {
      flush_text();
      ok = rnv_end_tag(&current,&previous,(char*)name) && ok;
      mixed=1;
    } 
  else
    {
      if (level==0)
	current=previous;
      else
	--level;
    }
}

void
rnv_characters(void *userData,const char *s,int len)
{
  if(current!=rn_notAllowed)
    mytext = s;
}

int
rnv_validate(struct node *np)
{
  previous=current=rnc_start;
  ok = 1;
  tree_validate(np);
  return !ok;
}

void
rnvif_init()
{
  init();
  verbose = 0; nexp = 0; rnck=0;
}

void
rnv_reinit()
{
  clear();
}

void
rnvif_term()
{
  extern void rn_term(void);
  clear();
  rn_term();
  rnc_term();
  rnv_term();
  rnx_term();
}
