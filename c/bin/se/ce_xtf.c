#include <stdio.h>
#include <stdlib.h>
#include <sys/unistd.h>
#include <unistd.h>
#include <ctype128.h>
#include <psd_base.h>
#include <messages.h>
#include <options.h>
#include <fname.h>
#include <hash.h>
#include <list.h>
#include <runexpat.h>
#include <loadfile.h>
#include <xpd2.h>
#include <npool.h>
#include "selib.h"
#include "xmlutil.h"
#include "wm.h"
#include "ce.h"

enum wm_format cetype;

const char *wm_names[5];

unsigned char *buf = NULL;
int buf_len = 0;

char curr_label[1024], label[1024], last_label[1024], 
  text_name[1024], text_id[128], text_project[1024], text_desc[1024];

const char *ce_l_tag = NULL;
const char *content_tago = "content";
const char *content_tagc = "content";

FILE *ce_out_fp = NULL;
struct npool *xtf_pool;
unsigned char *pending_heading = NULL;
int ce_use_colon_sep = 0;
int kwic_select_pending = 0;
int kwic_pivot_pending = 0;
int kwic_select_is_end = 0;
int l2 = 1;
int line_context = 0;
int line_count = 0;
int p3 = 1;
int xml_output = 1;
int xtf_context = 1;
int xtf_selecting = 0;
int this_node_terminates = 0;
enum rd_state { RD_START , RD_END , RD_SELECT };

const char *arg_fields;
const char *id, *ce_index, *lang = NULL, *mode = NULL, *project, *fn_project, *xtr_n = NULL;
const char *idattr = "xml:id";
const char *kwic_attr = "";
int echoing = 0;
int echoing_suspended = 0;
int langmask; /* just for selib.c; not used by ce */
int selecting_w = 0;
int tabbed = 0;
int verbose = 0;
int xtf_l_open = 0;

const char *norm_ns_uri = "http://oracc.org/ns/norm/1.0";
const char *xcl_ns_uri = "http://oracc.org/ns/xcl/1.0";
const char *gdl_ns_uri = "http://oracc.org/ns/gdl/1.0";
const char *xtf_ns_uri = "http://oracc.org/ns/xtf/1.0";

const char *gdl_w_name = "http://oracc.org/ns/gdl/1.0|w";
const char *norm_w_name = "http://oracc.org/ns/norm/1.0|w";
const char *xtf_l_name = "http://oracc.org/ns/xtf/1.0|l";
const char *xtf_lg_name = "http://oracc.org/ns/xtf/1.0|lg";
const char *xtf_note_name = "http://oracc.org/ns/xtf/1.0|note";

const char *project_en = NULL;

struct ce_config cfg;
List *files = NULL;
Hash_table *ht, *wh;
Hash_table *seen_files;
char langindex[32];

Hash_table *xtf_lines, *xtf_start, *xtf_end, *xtf_select, *xtf_files, *xtf_headings;

/* FIXME: this needs to be redone using dynamic memory in blocks */
const char *idps[1000];
int curr_idp = 0, next_idp = 0;
int hashvals[1000];
int curr_hashval = 0;
int *idcountp = 0;

static void eH_sub(const char *name);

static int
llg_match(const char *name)
{
  if (ce_l_tag && ((ce_l_tag[1] && !strcmp(&name[strlen(name)-3], "|lg"))
		   || (!ce_l_tag[1] && !strcmp(&name[strlen(name)-2], "|l"))))
    return 1;
  return 0;
}

static const char *
l2_findfile(const char *proj, const char *pqid, const char *base)
{
  const char *found;
  if (base)
    {
      found = expand_xtr((proj && *proj) ? proj : project,
			 pqid,strchr(base,'_')+1,NULL);
      if (!access(found,R_OK))
	{
	  if (verbose)
	    fprintf(stderr,"ce_xtf: using %s\n",found);
      	  return found;
	}
    }
  else
    {
      if (xtf_context)
	{
	  found = expand((proj && *proj) ? proj : project, pqid, "xsf");
	  if (!access(found,R_OK))
	    {
	      if (verbose)
		fprintf(stderr,"ce_xtf: using %s\n",found);
	      return found;
	    }
	  found = expand((proj && *proj) ? proj : project, pqid, "xtf");
	  if (!access(found,R_OK))
	    {
	      if (verbose)
		fprintf(stderr,"ce_xtf: using %s\n",found);
	      return found;
	    }
	}
      else
	{
	  found = expand((proj && *proj) ? proj : project, pqid, cfg.ext);
	  if (!access(found,R_OK))
	    {
	      if (verbose)
		fprintf(stderr,"ce_xtf: using %s\n",found);
	      return found;
	    }
	}
    }

  if (verbose)
    fprintf(stderr,"ce_xtf: %s not found\n", found);
  return NULL;
}

static void
ce_file(const unsigned char *idp)
{
  static char proj[128];
  static char pqid[8];
  unsigned char *colon;

  if (l2)
    {
      if ((colon = (unsigned char *)strchr((char*)idp, ':')))
	{
	  strcpy(proj,(char*)idp);
	  proj[colon-idp] = '\0';
	  idp = colon+1;
	}
      else
	*proj = '\0';
    }
  else
    *proj = '\0';

  strncpy(pqid,(char*)idp,7);
  pqid[7] = '\0';

  if (idp[7] == '_' || (l2 && project_en))
    {
      unsigned char base[128], *pd;
      pd = (unsigned char *)strchr((char*)idp,'.');
      strncpy((char*)base,(char*)idp,pd-idp);
      base[pd-idp] = '\0';
      if (!hash_find(seen_files,base))
	{
	  const char *fn = l2_findfile(proj,pqid,(char*)base);
	  if (fn)
	    list_add(files,npool_copy((unsigned char *)fn, xtf_pool));
	  /* In KWIC mode we re-read the file for each entry */
	  if (cetype != KU_KWIC)
	    hash_add(seen_files,npool_copy(base, xtf_pool),(void*)1);
	}
    }
  else
    {
      if (!hash_find(seen_files,(unsigned char *)pqid))
	{
	  const char *fn = l2_findfile(proj,pqid,NULL);
	  if (fn)
	    list_add(files,npool_copy((unsigned char *)fn,xtf_pool));
	  /* add this to the hash regardless of found status;
	     we don't need to look for it repeatedly if it isn't there */
	  hash_add(seen_files,npool_copy((unsigned char *)pqid,xtf_pool),(void*)1);
	}
    }
}

static void
ce_xtf_save_id(unsigned char *buf, enum rd_state state)
{
  static int one = 1;
  char *dot = NULL;
  char *start_id = NULL, *colon, *file;
  buf = npool_copy(buf, xtf_pool);

  if ((colon = strchr((char*)buf, ':')))
    {
      ++colon;
      start_id = (char*)npool_copy((unsigned char*)colon, xtf_pool);
    }
  else
    start_id = (char*)buf;

  file = (char*)npool_copy((unsigned char*)buf, xtf_pool);

  switch (state)
    {
    case RD_START:
      /* WATCH ME: DOES NOT WORK WITH COLON-IDS */
      if ((dot = strchr((char*)file, '.')))
	*dot = '\0';
      if (!hash_find(xtf_files, (unsigned char *)file))
	{
	  hash_add(xtf_files, npool_copy((unsigned char*)file, xtf_pool), &one);
	  ce_file((unsigned char *)file);
	}
      if (dot)
	{
	  if ((dot = strchr(dot+1, '.')))
	    {
	      *dot = '\0';
	      hash_add(xtf_lines, npool_copy((unsigned char *)start_id, xtf_pool), &one);
	      *dot = '.';
	    }
	}
      if (verbose)
	fprintf(stderr,"ce_xtf: adding ID %s to xtf_start\n", start_id);
      hash_add(xtf_start, (unsigned char *)start_id, &one);
      if (pending_heading)
	{
	  hash_add(xtf_headings, (unsigned char *)start_id, pending_heading);
	  pending_heading = NULL;
	}
      break;
    case RD_END:
      if (verbose)
	fprintf(stderr,"ce_xtf: adding ID %s to xtf_end\n", start_id);
      hash_add(xtf_end, (unsigned char *)start_id, &one);
      break;
    case RD_SELECT:
      hash_add(xtf_select, (unsigned char *)start_id, &one);
      break;
    }
}

static void
scan_heading(void)
{
  int m_size = 32, m_used = 0, ch;
  pending_heading = malloc(m_size);
  while ((ch = getchar()) != EOF)
    {
      if (m_used == m_size)
	pending_heading = realloc(pending_heading, m_size += m_size);
      if (ch == '\n')
	break;
      pending_heading[m_used++] = ch;
    }
  pending_heading[m_used] = '\0';
}

static void
ce_ids(int STOPPER)
{
  enum rd_state state = RD_START;
  int ch, i = 0;
  while ((ch = getchar()) != EOF)
    {
      switch (ch)
	{
	case '#':
	  scan_heading();
	  state = RD_START;
	  break;
	case '+': /* a+b for compounds; check format and test for state=select? */
	case ' ':
	  buf[i] = '\0';
	  ce_xtf_save_id(buf, state);
	  i = 0;
	  switch (state)
	    {
	    case RD_START:
	      state = RD_END;
	      break;
	    case RD_END:
	      state = RD_SELECT;
	      break;
	    default:
	      break;
	    }
	  break;
	case '\n':
	  buf[i] = '\0';
	  ce_xtf_save_id(buf, state);
	  i = 0;
	  if (cetype == KU_KWIC)
	    goto jumpout;
	  state = RD_START;
	  break;
	default:
	  if (i == buf_len)
	    buf = realloc(buf, buf_len += 32);
	  buf[i++] = ch;
	  break;
	}
    }
 jumpout:
  if (i) /* file did not end with a newline or space */
    ce_xtf_save_id(buf, state);
}

void
printText(const char *s)
{
  while (*s)
    {
      if (*s == '<')
	fputs("&lt;",ce_out_fp);
      else if (*s == '&')
	fputs("&amp;",ce_out_fp);
      else if (*s == '"')
	fputs("&quot;",ce_out_fp);
      else
	fputc(*s,ce_out_fp);
      ++s;
    }
}

static const char *
prefix(const char *name)
{
  static char buf[32], *ns, *dest;
  if ((ns = strstr(name, "/ns/"))) /* all Oracc namespaces */
    {
      ns += 4;
      dest = buf;
      while ('/' != *ns)
	*dest++ = *ns++;
      *dest++ = ':';
      ns = strchr(ns, '|');
      strcpy(dest, ns+1);
      return buf;
    }
  else
    {
      const char *vbar = strchr(name, '|');
      if (vbar)
	{
	  if (!strstr(name, "xhtml") && !strstr(name, "XML/1998"))
	    fprintf(stderr, "ce_xtf: non-Oracc namespace in %s\n", name);
	  return vbar+1;
	}
      else
	return name;
    }
}

void
printStart(const char *name, const char **atts)
{
  const char **ap = atts;
  int xtf_did_selected = 0;
  const char *xid = xml_id(atts);
  int xtf_lg =  !strcmp(name, xtf_lg_name);
  int xtf_l = !strcmp(name, xtf_l_name);

  if (echoing_suspended)
    return;

  if (!ce_l_tag)
    {
      if (xtf_lg)
	{
	  ce_l_tag = "lg";
	  /* xtf_l = 1; */
	}
      else if (xtf_l)
	ce_l_tag = "l";
    }

  if (xid && xtf_context)
    {
      if (hash_find(xtf_select, (unsigned char *)xid))
	{
	  if (cetype == KU_KWIC)
	    {
	      if (kwic_select_pending)
		{
		  kwic_pivot_pending = 1;
		  kwic_select_pending = 0;
		  xtf_selecting = 1;
		  if (hash_find(xtf_end, (unsigned char *)xid))
		    kwic_select_is_end = 1;
		  else
		    kwic_select_is_end = 0;
		}
	    }
	  else
	    xtf_selecting = 1;
	}
    }

  printText((const char*)charData_retrieve());

  if (xtf_selecting && cetype == KU_KWIC 
      && (!strcmp(name, gdl_w_name)
	  || !strcmp(name, norm_w_name)))
    fputs("</ce:kwic1><ce:kwic2>", ce_out_fp);

  if (xtf_l && llg_match(name))
    fprintf(ce_out_fp, "<ce:%s", ce_l_tag);
  else
    fprintf(ce_out_fp, "<%s", prefix(name));

  if (atts)
    {
      const char *id = "";
      if (selecting_w)
	{
	  while (*ap)
	    if (ap[0][strlen(ap[0])-1]=='d' 
		&& (!strcmp(*ap,"id") || !strcmp(*ap,"xml:id")))
	      {
		id = *++ap;
		break;
	      }
	    else
	      ++ap;
	  ap = atts;
	}
      while (*ap)
	{
	  if (!strcmp(*ap,"class")
	      && (xtf_selecting
		  || (!strcmp(ap[1],"w") 
		      && hash_find(wh, (const unsigned char*)id))))
	    {
	      fprintf(ce_out_fp, " class=\"%s selected\"",*++ap);
	      xtf_did_selected = 1;
	      ++ap;
	    }
	  else if (!strcmp(*ap, "xml:id"))
	    {
	      fprintf(ce_out_fp, " xml-id=\"");
	      ++ap;
	      printText(*ap++);
	      fputc('"', ce_out_fp);	      
	    }
	  else
	    {
	      fprintf(ce_out_fp, " %s=\"",prefix(*ap++));
	      printText(*ap++);
	      fputc('"', ce_out_fp);
	    }
	}
      if (xtf_selecting && !xtf_did_selected)
	fprintf(ce_out_fp, " class=\"selected\"");
    }

  if (xtf_l)
    {
      if (*label)
	strcpy(last_label, findAttr(atts, "label"));
      else
	strcpy(label, findAttr(atts, "label"));
      fputs("/>", ce_out_fp);
    }
  else if (xtf_lg)
    fputs("/>", ce_out_fp);
  else
    fputc('>', ce_out_fp);

  xtf_selecting = 0;
}

void
printEnd(const char *name)
{
  if (echoing_suspended
      || llg_match(name))
    return;

  printText((const char *)charData_retrieve());
  fprintf(ce_out_fp, "</%s>", prefix(name));

  if (kwic_pivot_pending 
      && (!strcmp(name, gdl_w_name)
	  || !strcmp(name, norm_w_name)))
    {
      fputs("</ce:kwic2><ce:kwic3>", ce_out_fp);
      kwic_pivot_pending = this_node_terminates = 0;
      if (kwic_select_is_end)
	{
	  fputs("<ce:end/>", ce_out_fp);
	  fprintf(ce_out_fp, "</ce:%s>", content_tagc);
	  fprintf(ce_out_fp, "<ce:label>%s: ",
		  (const char *)xmlify((const unsigned char*)text_name)
		  );
	  fprintf(ce_out_fp, "%s</ce:label></ce:data>", 
		  (const char *)xmlify((const unsigned char*)(*label ? label : curr_label))
		  );
	  *last_label = '\0';
	  *label = '\0';
	  echoing = 0;
	  echoing_suspended = 0;
	  kwic_select_is_end = 0;
	  ce_l_tag = NULL;
	}
    }
}

#define is_xcl(s) (!strncmp(s, xcl_ns_uri, strlen(xcl_ns_uri)))
#define is_xtf(s) (!strncmp(s, xtf_ns_uri, strlen(xtf_ns_uri)))

typedef char context_id_buf[128];
context_id_buf context_id_ring[5];
context_id_buf *curr_context_id_dest, *curr_context_id_used;

#if 0
static void
add_context_id(const char *id)
{
  if (!curr_context_id_dest)
    curr_context_id_dest = context_id_ring;
  else if (++curr_context_id_dest - context_id_ring > 5)
    curr_context_id_dest = context_id_ring;
  strcpy(*curr_context_id_dest, id);
}
static const char *
use_context_id(void)
{
  if (!curr_context_id_used)
    curr_context_id_used = context_id_ring;
  return *curr_context_id_used++;
}
#endif

static void
ce_data(const char *xid)
{
  int linenum = 0;
  char *dot = strchr(xid,'.');
  char cid[128];

  if (verbose)
    fprintf(stderr, "ce_xtf: ce_data called with xid=%s\n", xid);
  
  if (dot)
    {
      linenum = atoi(dot+1);
      if (linenum > 4)
	{
	  linenum -= 4;
	  strcpy(cid,xid);
	  dot = strchr(cid,'.');
	  sprintf(dot+1,"%d",linenum);
	}
      else
	{
	  *cid = '\0';
	}
    }
  else
    {
      *cid = '\0';      
    }
  
  fprintf(ce_out_fp, "<ce:data project=\"%s\" text-id=\"%s\" line-id=\"%s\" context-id=\"%s\"%s><ce:%s>", 
	  text_project, text_id, xid, cid,
	  kwic_attr, content_tago);

  if (cetype == KU_KWIC)
    kwic_select_pending = 1;
}

void
ce_xtf_sH(void *userData, const char *name, const char **atts)
{
  const char *xid = NULL;

  if (is_xcl(name))
    {
      echoing_suspended = 1;
      return;
    }
  
  xid = xml_id(atts);

  if (xid && verbose)
    fprintf(stderr,"ce_xtf: ce_xtf_sH found ID %s\n", xid);

  if (is_xtf(name) 
      && (!strcmp(&name[strlen(name)-2], "|l") 
	  || !strcmp(&name[strlen(name)-3], "|lg")
	  || !strcmp(&name[strlen(name)-2], "|v")
	  || !strcmp(&name[strlen(name)-2], "|m")
	  ))
    {
      if ('m' == name[strlen(name)-1])
	{
	  const char *mtype = findAttr(atts, "type");
	  if (!strcmp((const char *)mtype, "textname"))
	    strcpy(text_name, findAttr(atts, "n"));
	  if (!strcmp((const char*)mtype, "textdesc"))
	    strcpy(text_desc, findAttr(atts, "n"));
	}
      else
	strcpy(curr_label,findAttr(atts,"label"));
    }

  if (xid && *xid && hash_find(xtf_start, (unsigned char *)xid))
    {
      unsigned char *h = hash_find(xtf_headings, (unsigned char *)xid);

      if (verbose)
	fprintf(stderr, "ce_xtf: found xid %s for ce_data\n", xid);
      
      /* This can be gdl:w or xtf:l(g) */
      charData_discard();
      
      /* if it is xtf:l(g) we may not have dumped ce:data yet;
	 this can only happen here if we are doing a line 
	 range
       */
      if (!echoing)
	ce_data(xid);

      if (h)
	fprintf(ce_out_fp, "<ce:start ref=\"%s\" h=\"%s\"/>", 
		xid, (const char *)xmlify((const unsigned char *)h));
      else
	fprintf(ce_out_fp, "<ce:start ref=\"%s\"/>", xid);

      /* always print the name and atts */
      echoing_suspended = 0;
      printStart(name, atts);

      /* and now echo text nodes as well as structure */
      echoing = 2;
    } 
  else if (echoing)
    {
      if (!is_xtf(name) 
	  || !strcmp(&name[strlen(name)-2], "|l") 
	  || !strcmp(&name[strlen(name)-3], "|lg")
	  || !strcmp(&name[strlen(name)-2], "|v")
	  )
	{
	  /* this can be gdl:* or xtf:l(g) (we've ruled out all other 
	     xtf nodes in the condition above)
	  */
	  printStart(name, atts);
	}
      else if (!strcmp(name, xtf_note_name))
	echoing_suspended = 1;
      else
	charData_discard();
    }
  else
    {
      /* If this is an xtf:l(g) node which contains a g:w that
	 starts a range, we need to start echoing structure
       */
      if (!strcmp(name, xtf_l_name) && xid && hash_find(xtf_lines, (unsigned char *)xid))
	{
	  ce_data(xid);
	  printStart(name, atts);
	  echoing = 1;
	}
      else if (!strncmp(name, xtf_ns_uri, strlen(xtf_ns_uri)))
	{
	  const char *localName = name + strlen(xtf_ns_uri) + 1;
	  if (!strcmp(localName, "transliteration")
	      || !strcmp(localName, "composite")
	      || !strcmp(localName, "score"))
	    {
	      strcpy(text_name, findAttr(atts, "textname"));
	      if (!*text_name)
		strcpy(text_name, findAttr(atts, "n"));
	      strcpy(text_id, xml_id(atts));
	      strcpy(text_project, findAttr(atts, "project"));
	    }
	}
      charData_discard();
    }

  /* We need to do this test separately from the 
     tag-printing block because the start and end IDs
     can be the same
   */
  if (xid && hash_find(xtf_end, (unsigned char *)xid))
    this_node_terminates = 1;
}

static void
eH_sub(const char *name)
{
  if (!strcmp(&name[strlen(name)-2], "|w"))
    {      
      /* If this is a terminating g:w we need to
	 continue echoing structure to balance 
	 the fragment unless we are in KWIC mode
      */
      if (echoing == 2)
	fputs("<ce:end/>", ce_out_fp);
      if (cetype != KU_KWIC)
	echoing = 1;
      else
	echoing = 0;
    }
  else if (llg_match(name))
    {
      if (echoing == 2)
	fputs("<ce:end/>", ce_out_fp);
      /* if we are at the terminating xtf:l
	 we are done with this ce:data frag
      */
      fprintf(ce_out_fp, "</ce:%s>", content_tagc);
      if (*last_label && cetype == KU_UNIT) /* we must be in unit context */
	sprintf(label+strlen(label), " - %s", last_label);
      fprintf(ce_out_fp, "<ce:label>%s%s ",
	      (const char *)xmlify((const unsigned char*)text_name),
	      /*ce_use_colon_sep ? ":" : ""*/ ""
	      );
      fprintf(ce_out_fp, "%s",
	      (const char *)xmlify((const unsigned char*)label)
	      );
      if (*text_desc)
	fprintf(ce_out_fp, " [%s]",
		(const char *)xmlify((const unsigned char*)text_desc)
		);
      fputs("</ce:label></ce:data>", ce_out_fp);
      *last_label = '\0';
      *label = '\0';
      echoing = 0;
      echoing_suspended = 0;
      this_node_terminates = 0;
      ce_l_tag = NULL;
    }
  else
    {
      /* this is a node which is being printed
	 to balance the structure; the tag got
	 printed on entry to this block so down
	 here we're a no-op.
      */
    }
}

void
ce_xtf_eH(void *userData, const char *name)
{
  if (echoing && (!is_xtf(name) || llg_match(name) || !strcmp(&name[strlen(name)-2], "|v")))
    {
      if (this_node_terminates && (!ce_l_tag || (llg_match(name))))
	{
	  /* always print the tag */
	  printEnd(name);
	  eH_sub(name);
	}
      else
	printEnd(name);
    }
  else
    charData_discard();

  if (!strcmp(name, xtf_note_name) && echoing)
    {
      echoing_suspended = 0;
      charData_discard();
    }
}

/* r -3 -l -p cmawro </tmp/p3.c1xvvW/pgwrap.out */

const char **
list2charstarstar(List *l)
{
  char **c = malloc((1+list_len(l))*sizeof(char*));
  int cindex = 0;
  List_node *lnp;
  for (lnp = l->first; lnp; lnp = lnp->next)
    c[cindex++] = lnp->data;
  c[cindex] = NULL;
  return (const char**) c;
}

static void
ce_xtf_init(void)
{
  seen_files = hash_create(1024);
  xtf_lines = hash_create(1024);
  xtf_start = hash_create(1024);
  xtf_end = hash_create(1024);
  xtf_select = hash_create(1024);
  xtf_files = hash_create(1024);
  xtf_headings = hash_create(1024);
  xtf_pool = npool_init();
  files = list_create(LIST_SINGLE);
}

static void
ce_xtf_term(void)
{
  hash_free(seen_files, NULL);
  hash_free(xtf_lines, NULL);
  hash_free(xtf_start, NULL);
  hash_free(xtf_end, NULL);
  hash_free(xtf_select, NULL);
  hash_free(xtf_files, NULL);
  hash_free(xtf_headings, NULL);
  npool_term(xtf_pool);
  list_free(files, NULL);
}

int
main(int argc, char * const*argv)
{
  struct xpd *cfg = NULL;
  exit_on_error = TRUE;

  init_wm_names();
  cetype = KU_LINE;
  options(argc, argv, "23c:f:i:lkm:p:tuvx");

  if (!project || (!ce_index && !xtf_context))
    {
      fprintf(stderr,"ce: must give -p and -i on command line\n");
      usage();
    }

  if (!xtf_context)
    {
      if (lang)
	sprintf(langindex,"%s/%s",ce_index,lang);
      else
	strcpy(langindex,ce_index);
    }

  ce_out_fp = stdout;
  buf = malloc(buf_len = 32);
  if (xtf_context)
    fprintf(ce_out_fp, "<ce:ce xmlns:ce=\"http://oracc.org/ns/ce/1.0\" xmlns:gdl=\"http://oracc.org/ns/gdl/1.0\" xmlns:xtf=\"http://oracc.org/ns/xtf/1.0\" xmlns:norm=\"http://oracc.org/ns/norm/1.0\" xmlns:note=\"http://oracc.org/ns/note/1.0\" xmlns:syntax=\"http://oracc.org/ns/syntax/1.0\" cetype=\"%s\">", wm_names[cetype]);
  else if (xml_output)
    fputs("<ce:ce xmlns:ce=\"http://oracc.org/ns/ce/1.0\">",
	  ce_out_fp);

  if (xtf_context)
    {
      if (cetype == KU_KWIC)
	{
	  while (1)
	    {
	      ce_xtf_init();
	      ce_ids('\n');
	      if (list_len(files))
		runexpatNS(i_list, list2charstarstar(files), ce_xtf_sH, ce_xtf_eH, "|");
	      ce_xtf_term();
	      if (feof(stdin))
		break;
	    }
	}
      else
	{
	  ce_xtf_init();
#if 0
	  cfg = xpd_init(project, xtf_pool);
	  if (!strcmp(xpd_option(cfg, "ce-use-colon-sep"), "no"))
	    ce_use_colon_sep = 0;
#endif
	  ce_ids(EOF);
	  runexpatNS(i_list, list2charstarstar(files), ce_xtf_sH, ce_xtf_eH, "|");
	  ce_xtf_term();
	}
    }
  if (xml_output)
    fputs("</ce:ce>",ce_out_fp);
  return 0;
}

int
opts(int argc, char *arg)
{
  switch (argc)
    {
    case '2':
      l2 = 1;
      break;
    case '3':
      p3 = 1;
      break;
    case 'c':
      id = arg;
      break;
    case 'f':
      arg_fields = arg;
      break;
    case 'i':
      ce_index = arg;
      break;
    case 'k':
      cetype = KU_KWIC;
      kwic_attr = " kwic=\"yes\"";
      content_tago = "kwic1";
      content_tagc = "kwic3";
      break;
    case 'l':
      cetype = KU_LINE;
      break;
    case 'm':
      mode = arg;
      break;
    case 'p':
      project = arg;
      /*
      if (!strcmp(project,"cdli"))
	fn_project = NULL;
      else
      */
	fn_project = project;
      break;
    case 't':
      tabbed = 1;
      break;
    case 'u':
      cetype = KU_UNIT;
      break;
    case 'v':
      verbose = 1;
      break;
    case 'x':
      xml_output = 1;
      break;
    default:
      return 1;
    }
  return 0;
}

const char *prog = "ce_xtf";
int major_version = 1, minor_version = 0, verbose;
const char *usage_string = " [-c ID] -p <PROJECT> -i <INDEX> [-x]";
void
help ()
{
  printf("  -c = read an ID from arg (default is stdin)\n");
  printf("  -i = specify index\n");
  printf("  -p = specify project\n");
  printf("  -v = verbose mode\n");
  printf("  -x = xml output\n");
}
