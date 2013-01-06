#include <unistd.h>
#include <ctype.h>
#include <psd_base.h>
#include <runexpat.h>
#include <fname.h>
#include <string.h>
#include <hash.h>
#include <npool.h>
/*#include "xff.h"*/
#include "xcl.h"
#include "ngram.h"

#include "hash.h"
#include "pool.h"
#include "ilem_form.h"
#include "props.h"

#define EXPAT_NS_CHAR '|'

#define cc(x) ((char *)(x))
#define xpool_copy(x,p) (char*)npool_copy((unsigned char*)x,p)
#define uxpool_copy(x,p) (const unsigned char *)npool_copy((unsigned char *)(x),p)
extern char *strdup(const char*);

/*static const char *xl_role = "http://www.w3.org/1999/xlink:role";*/

int curr_lnum = 0;
static int in_ll = 0;
static enum ll_types ll_type;
const char *curr_xml_id = NULL;
const char *curr_inst = NULL;
const char *curr_ref = NULL;
const char *curr_sig = NULL;
const char *psu_lang = NULL;
static int in_multi = 0;

const char *system_project = NULL;
extern int lem_autolem;
extern void lem_clear_cache(void);

Hash_table *known_langs;
Hash_table *curr_meta;
static struct ilem_form *curr_form;
const char *next_k = NULL;

static void
xcl_sH(void *userData, const char *name, const char **atts)
{
  static struct ilem_form *curr_form = NULL;
  const char *vbar = strchr(name,EXPAT_NS_CHAR);
  struct xcl_context *xcp = userData;

  if (!strncmp("http://oracc.org/ns/xcl/1.0",name,vbar-name))
    {
      curr_lnum = atoi(findAttr(atts,"lnum"));
      ++vbar;
      if (vbar[0] == 'l')
	{
	  if (!vbar[1])
	    {
	      curr_xml_id = xpool_copy(findAttr(atts,"xml:id"),xcp->pool);
	      curr_inst = xpool_copy(findAttr(atts,"inst"),xcp->pool);
	      curr_ref = findAttr(atts,"ref");
	      if (*curr_ref)
		curr_ref = xpool_copy(curr_ref,xcp->pool);
	      curr_sig = xpool_copy(findAttr(atts,"sig"),xcp->pool);
	    }
	  else if (vbar[1] == 'l' && !vbar[2])
	    {
	      in_ll = 1;
	    }
	  else if (!strcmp(vbar,"linkset"))
	    {
	    }
	  else if (!strcmp(vbar,"link"))
	    {
	    }
	}
      else if (!strcmp(vbar, "xcl"))
	{
	  xcp->project = xpool_copy(findAttr(atts,"project"),xcp->pool);
	  xcp->textid = xpool_copy(findAttr(atts,"textid"),xcp->pool);
	  xcp->file = xpool_copy(findAttr(atts,"file"),xcp->pool);
	  xcp->langs = xpool_copy(findAttr(atts,"langs"),xcp->pool);
	}
      else if (!strcmp(vbar, "c"))
	{
	  const char *strtok = findAttr(atts, "type");
	  struct xcl_tok_tab *ttp = xcltok(strtok,strlen(strtok));
	  if (ttp && ttp->node_type == xcl_node_c)
	    xcl_chunk(xcp, findAttr(atts, "xml:id"), ttp->subtype);
	  else
	    fprintf(stderr,"unknown xcl chunk token type '%s'\n",strtok);
	}
      else if (!strcmp(vbar, "psu"))
	psu_lang = findAttr(atts,"lang");
      else if (!strcmp(vbar, "d"))
	{
	  const char *strtok = findAttr(atts, "type");
	  struct xcl_tok_tab *ttp = xcltok(strtok,strlen(strtok));
	  if (ttp && ttp->node_type == xcl_node_d)
	    {
	      xcl_discontinuity(xcp, 
				xpool_copy(findAttr(atts,"ref"),xcp->pool),
				ttp->subtype,
				NULL);
	    }
	  else
	    fprintf(stderr,"unknown xcl discontinuity token type '%s'\n",strtok);
	}
      else if (!strcmp(vbar,"mds"))
	curr_meta = xcl_create_meta(xcp,(const char*)npool_copy((unsigned char *)get_xml_id(atts),
								xcp->pool));
      else if (!strcmp(vbar,"m"))
	next_k = findAttr(atts,"k");
      else if (!strcmp(vbar, "multi"))
	{
	  in_multi = 1;
	}      
      else if (!strcmp(vbar, "prop"))
	{
	  if (curr_form)
	    {
	      props_add_prop(curr_form,
			     uxpool_copy(findAttr(atts,"group"),xcp->pool),
			     uxpool_copy(findAttr(atts,"name"),xcp->pool),
			     uxpool_copy(findAttr(atts,"value"),xcp->pool),
			     xpool_copy(findAttr(atts,"ref"),xcp->pool),
			     xpool_copy(findAttr(atts,"xml:id"),xcp->pool),
			     xpool_copy(findAttr(atts,"p"),xcp->pool),
			     atoi(findAttr(atts,"ngram")));
	    }
	  else
	    {
	      fprintf(stderr,"xff loader: invalid XML: found prop outside form\n");
	      return;	  
	    }
	}
    }
}

void
xcl_eH(void *userData, const char *name)
{
  struct xcl_context *xcp = userData;
  const char *vbar = strchr(name,EXPAT_NS_CHAR);
  if (!strncmp("http://oracc.org/ns/xcl/1.0",name,vbar-name))
    {
      ++vbar;
      if (next_k && !strcmp(vbar,"m"))
	{
	  hash_add(curr_meta,
		   npool_copy((unsigned char *)next_k,xcp->pool),
		   npool_copy((unsigned char *)charData_retrieve(),xcp->pool));
	  next_k = NULL;
	}
      else if (!strcmp(vbar,"c"))
	xcl_chunk_end(xcp);
      else if (!strcmp(vbar,"l"))
	{
	  if (!in_ll)
	    {
	      struct xcl_l *lp = xcl_lemma(xcp,
					   curr_xml_id,
					   curr_ref,
					   curr_form,
					   NULL, ll_type);
	      lp->inst = curr_inst;
	      lp->sig = curr_sig;
	      lp->lnum = curr_lnum;
	    }
	}
      else if (!strcmp(vbar,"ll"))
	{
	  ll_type = ll_none;
	  in_ll = 0;
	}
      else if (!strcmp(vbar,"psu"))
	{
	  unsigned char *tmp = (unsigned char*)charData_retrieve(), *etmp;
	  List *lp;
	  while (isspace(*tmp))
	    ++tmp;
	  etmp = tmp+strlen((char*)tmp);
	  while (isspace(etmp[-1]))
	    --etmp;
	  *etmp = '\0';
	  if (!(lp = hash_find(xcp->psus,(unsigned char*)psu_lang)))
	    {
	      lp = list_create(LIST_SINGLE);
	      hash_add(xcp->psus,npool_copy((unsigned char*)psu_lang,xcp->pool),lp);
	    }
	  list_add(lp,npool_copy(tmp,xcp->pool));
	}
    }
  else
    charData_discard();
}

/* Load the XML serialization of an XCL tree back into memory */
struct xcl_context *
xcl_load(const char *xcl_fn, int setup_formsets)
{
  const char *fname[2];
  struct xcl_context *xcp = xcl_create();
  char ns_char[2];
  ns_char[0] = EXPAT_NS_CHAR; ns_char[1] = '\0';
  fname[0] = xcl_fn;
  fname[1] = NULL;
  curr_meta = NULL;
  if (!access(fname[0],R_OK))
    runexpatNSuD(i_list, fname, xcl_sH, xcl_eH, ns_char, xcp);
  else
    fprintf(stderr,"xcl_load: can't open XCL input %s\n", fname[0]);
  return xcp;
}
