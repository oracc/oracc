#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <psd_base.h>
#include <messages.h>
#include <options.h>
#include <fname.h>
#include <hash.h>
#include <list.h>
#include <runexpat.h>
#include <loadfile.h>
#include <npool.h>
#include "selib.h"
#include "wm.h"
#include "ce.h"

enum wm_format cetype;

const char *wm_names[4];

unsigned char *buf = NULL;
int buf_len = 0;

char label[128], last_label[128], 
  text_name[128], text_id[16], text_project[128];

const char *content_tago = "content";
const char *content_tagc = "content";

FILE *ce_out_fp = NULL;
struct npool *xtf_pool;
unsigned char *pending_heading = NULL;
int kwic_pivot_pending = 0;
int l2 = 1;
int line_context = 0;
int line_count = 0;
int xml_output = 1;
int xtf_context = 1;
int xtf_selecting = 0;
int this_node_terminates = 0;
enum rd_state { RD_START , RD_END , RD_SELECT };

const char *arg_fields;
const char *id, *index, *lang = NULL, *mode = NULL, *project, *fn_project, *xtr_n = NULL;
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
const char *xtf_l_name = "http://oracc.org/ns/xtf/1.0|l";
const char *xtf_l_note = "http://oracc.org/ns/xtf/1.0|note";

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

static const char *
findfile(const char *pqid, const char *base)
{
  const char **projects = cfg.proxies;
  while (*projects)
    {
      const char *found;
      if (base)
	found = expand_xtr(*projects,pqid,strchr(base,'_')+1,NULL);
      else
	found = expand(*projects,pqid,cfg.ext);
      if (verbose)
	fprintf(stderr,"trying %s...",found);
      if (!access(found,R_OK))
	{
	  if (verbose)
	    fprintf(stderr,"found\n");
	  return found;
	}
      if (verbose)
	fprintf(stderr,"not found\n");
      ++projects;
    }
  return NULL;
}

static const char *
l2_findfile(const char *proj, const char *pqid, const char *base)
{
  const char *found;
  if (base)
    found = expand_xtr((proj && *proj) ? proj : project,
		       pqid,strchr(base,'_')+1,NULL);
  else
    found = expand((proj && *proj) ? proj : project,
		   pqid, xtf_context ? "xtf" : cfg.ext);
  if (verbose)
    fprintf(stderr,"trying %s...",found);
  if (!access(found,R_OK))
    {
      if (verbose)
	fprintf(stderr,"found\n");
      return found;
    }
  if (verbose)
    fprintf(stderr,"not found\n");
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
#if 0
      if (l2)
	strcat(base,project_en);
#endif

      if (!hash_find(seen_files,base))
	{
	  const char *fn = (l2 
			    ? l2_findfile(proj,pqid,(char*)base) 
			    : findfile(pqid,(char*)base));
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
	  const char *fn = (l2
			    ? l2_findfile(proj,pqid,NULL)
			    : findfile(pqid,NULL));
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
  char *start_id = NULL, *colon;
  buf = npool_copy(buf, xtf_pool);

  if ((colon = strchr(buf, ':')))
    {
      ++colon;
      start_id = npool_copy(colon, xtf_pool);
    }
  else
    start_id = buf;

  switch (state)
    {
    case RD_START:
      /* WATCH ME: DOES NOT WORK WITH COLON-IDS */
      if ((dot = strchr((char*)buf, '.')))
	*dot = '\0';
      if (!hash_find(xtf_files, buf))
	{
	  hash_add(xtf_files, npool_copy(buf, xtf_pool), &one);
	  ce_file(buf);
	}
      if (dot)
	{
	  *dot = '.';
	  if ((dot = strchr(dot+1, '.')))
	    {
	      *dot = '\0';
	      hash_add(xtf_lines, npool_copy(buf, xtf_pool), &one);
	      *dot = '.';
	    }
	}
      hash_add(xtf_start, start_id, &one);
      if (pending_heading)
	{
	  hash_add(xtf_headings, buf, pending_heading);
	  pending_heading = NULL;
	}
      break;
    case RD_END:
      hash_add(xtf_end, buf, &one);
      break;
    case RD_SELECT:
      hash_add(xtf_select, buf, &one);
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
	  if (!strstr(name, "xhtml"))
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
  int xtf_l = !strcmp(name, xtf_l_name);

  if (echoing_suspended)
    return;

  if (xid && xtf_context)
    {
      if (hash_find(xtf_select, (unsigned char *)xid))
	{
	  xtf_selecting = 1;
	  if (cetype == KU_KWIC)
	    kwic_pivot_pending = 1;
	}
    }

  printText((const char*)charData_retrieve());

  if (xtf_selecting && cetype == KU_KWIC && !strcmp(name, gdl_w_name))
    fputs("</ce:kwic1><ce:kwic2>", ce_out_fp);

  fprintf(ce_out_fp, "<%s", xtf_l ? "ce:l" : prefix(name));

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
  if (!strcmp(name, xtf_l_name))
    {
      if (*label)
	strcpy(last_label, findAttr(atts, "label"));
      else
	strcpy(label, findAttr(atts, "label"));
      fputs("/>", ce_out_fp);
    }
  else
    fputc('>', ce_out_fp);
  xtf_selecting = 0;
}

void
printEnd(const char *name)
{
  if (echoing_suspended
      || !strcmp(name, xtf_l_name))
    return;

  printText((const char *)charData_retrieve());
  fprintf(ce_out_fp, "</%s>", prefix(name));

  if (kwic_pivot_pending && !strcmp(name, gdl_w_name))
    {
      fputs("</ce:kwic2><ce:kwic3>", ce_out_fp);
      kwic_pivot_pending = 0;
    }
}

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
  char cid[32];

  if (dot)
    {
      linenum = atoi(dot+1);
      if (linenum > 4)
	{
	  linenum -= 4;
	  sprintf(cid,xid);
	  dot = strchr(cid,'.');
	  sprintf(dot+1,"%d",linenum);
	}
      else
	{
	  *cid = '\0';
	}
    }
  
  fprintf(ce_out_fp, "<ce:data project=\"%s\" text-id=\"%s\" line-id=\"%s\" context-id=\"%s\"%s><ce:%s>", 
	  text_project, text_id, xid, cid,
	  kwic_attr, content_tago);
}

void
ce_xtf_sH(void *userData, const char *name, const char **atts)
{
  const char *xid = xml_id(atts);

  if (xid && hash_find(xtf_start, (unsigned char *)xid))
    {
      unsigned char *h = hash_find(xtf_headings, (unsigned char *)xid);
      /* This can be gdl:w or xtf:l */
      charData_discard();
      
      /* if it is xtf:l we may not have dumped ce:data yet;
	 this can only happen here if we are doing a line 
	 range
       */
      if (!echoing)
	ce_data(xid);

      if (h)
	fprintf(ce_out_fp, "<ce:start ref=\"%s\" h=\"%s\"/>", 
		xid, h);
      else
	fprintf(ce_out_fp, "<ce:start ref=\"%s\"/>", xid);

      /* always print the name and atts */
      printStart(name, atts);

      /* and now echo text nodes as well as structure */
      echoing = 2;
    } 
  else if (echoing)
    {
      if (!is_xtf(name) || !strcmp(&name[strlen(name)-2], "|l"))
	{
	  /* this can be gdl:* or xtf:l (we've ruled out all other 
	     xtf nodes in the condition above)
	  */
	  printStart(name, atts);
	}
      else if (!strcmp(name, xtf_l_note))
	echoing_suspended = 1;
      else
	charData_discard();
    }
  else
    {
      /* If this is an xtf:l node which contains a g:w that
	 starts a range, we need to start echoing structure
       */
      if (!strcmp(name, xtf_l_name) && hash_find(xtf_lines, (unsigned char *)xid))
	{
	  ce_data(xid);
	  printStart(name, atts);
	  echoing = 1;
	}
      else if (!strncmp(name, xtf_ns_uri, strlen(xtf_ns_uri)))
	{
	  const char *localName = name + strlen(xtf_ns_uri) + 1;
	  if (!strcmp(localName, "transliteration")
	      || !strcmp(localName, "composite"))
	    {
	      strcpy(text_name, findAttr(atts, "n"));
	      strcpy(text_id, findAttr(atts, "xml:id"));
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

void
ce_xtf_eH(void *userData, const char *name)
{
  if (echoing
      && (!is_xtf(name) || !strcmp(&name[strlen(name)-2], "|l")))
    {
      if (this_node_terminates)
	{
	  /* always print the tag */
	  printEnd(name);
	  if (!strcmp(&name[strlen(name)-2], "|w"))
	    {
	      
	      /* If this is a terminating g:w we need to
		 continue echoing structure to balance 
		 the fragment 
	       */
	      if (echoing == 2)
		fputs("<ce:end/>", ce_out_fp);
	      echoing = 1;
	    }
	  else if (!strcmp(&name[strlen(name)-2], "|l"))
	    {
	      if (echoing == 2)
		fputs("<ce:end/>", ce_out_fp);
	      /* if we are at the terminating xtf:l
		 we are done with this ce:data frag
	       */
	      fprintf(ce_out_fp, "</ce:%s>", content_tagc);
	      if (*last_label && cetype == KU_UNIT) /* we must be in unit context */
		sprintf(label+strlen(label), " - %s", last_label);
	      fprintf(ce_out_fp, "<ce:label>%s: %s</ce:label></ce:data>", 
		      text_name, label);
	      *last_label = '\0';
	      *label = '\0';
	      echoing = 0;
	      this_node_terminates = 0;
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
      else
	printEnd(name);
    }
  else
    charData_discard();

  if (!strcmp(name, xtf_l_note) && echoing)
    {
      echoing_suspended = 0;
      charData_discard();
    }
}

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
  exit_on_error = TRUE;

  init_wm_names();

  options(argc, argv, "2c:f:i:lkm:p:tuvx");

  if (!project || (!index && !xtf_context))
    {
      fprintf(stderr,"ce: must give -p and -i on command line\n");
      usage();
    }

  if (!xtf_context)
    {
      if (lang)
	sprintf(langindex,"%s/%s",index,lang);
      else
	strcpy(langindex,index);
    }

  ce_out_fp = stdout;
  buf = malloc(buf_len = 32);
  if (xtf_context)
    fprintf(ce_out_fp, "<ce:ce xmlns:ce=\"http://oracc.org/ns/ce/1.0\" xmlns:gdl=\"http://oracc.org/ns/gdl/1.0\" xmlns:xtf=\"http://oracc.org/ns/xtf/1.0\" xmlns:norm=\"http://oracc.org/ns/norm/1.0\" xmlns:syntax=\"http://oracc.org/ns/syntax/1.0\" cetype=\"%s\">", wm_names[cetype]);
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
    case 'c':
      id = arg;
      break;
    case 'f':
      arg_fields = arg;
      break;
    case 'i':
      index = arg;
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
