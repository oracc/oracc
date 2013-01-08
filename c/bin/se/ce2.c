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
#include <xmd.h>
#include "selib.h"
#include "ce.h"

/*FIXME: the i/o and id processing here needs to use dynamic memory */
#define CE_BUFSIZ 1024

#ifndef strdup
extern char *strdup(const char *);
#endif

extern const char *xmd_fields, *xmd_widths;

const char *id, *ce_index, *lang = NULL, *mode = NULL, *project, *fn_project, *xtr_n = NULL, *state;
const char *idattr = "xml:id";
int echoing = 0;
int item_offset;
int langmask; /* just for selib.c; not used by ce */
int selecting_w = 0;
int tabbed = 0;
int verbose = 0;
int xml_output = 0;

struct npool *ce_pool;

const char *project_en = NULL;

struct ce_config cfg;
List *files = NULL, *pqs = NULL;
Hash_table *ht, *wh, *xtf_headings;
Hash_table *seen_files;
char langindex[32];

/* FIXME: this needs to be redone using dynamic memory in blocks */
const char *idps[1000];
int curr_idp = 0, next_idp = 0;
int hashvals[1000];
int curr_hashval = 0;
int *idcountp = 0;

unsigned char *pending_heading = NULL;

static unsigned char *
chomp(unsigned char *s)
{
  if ('\n' == s[strlen((char*)s)-1])
    s[strlen((char*)s)-1] = '\0';
  return s;
}

static void
scan_heading(char *buf)
{
  buf = (char*)chomp((unsigned char *)buf);
  pending_heading = malloc(strlen(buf)+1);
  strcpy((char*)pending_heading, buf);
}

static void
ce_config()
{
  const char *fname = se_file(project,langindex,"ce.cfg");
  size_t cfgbytes;
  const char *cfgfile = (const char *)loadfile((unsigned const char *)fname,&cfgbytes);
  const char *p = cfgfile;
  cfg.f = atoi(p);
  if (cfg.f != ce_summary)
    {
      const char *sp = strchr(p,' ');
      if (sp)
	{
	  static char ext[10], *ep = ext;
	  ++sp;
	  cfg.ext = ext;
	  while (!isspace(*sp))
	    *ep++ = *sp++;
	  *ep = '\0';
	  p = (char*)sp;
	}
      else
	{
	  fprintf(stderr,"ce: bad config info (wanted ext in %s)\n", fname);
	  exit(1);
	}
    }
  cfg.tag = strchr(p,' ');
  if (cfg.tag && cfg.f >= 0 && cfg.f < ce_bad)
    ++cfg.tag;
  else
    {
      fprintf(stderr,"ce: bad config info (wanted tag in %s)\n", fname);
      exit(1);
    }
  p = cfg.tag + strlen(cfg.tag);
  ++p;
  /* Now p points at either the second '\0' after the tag, or at
     the start of a project name in a \0-delimited list of proxies */
  if (*p)
    {
      int nprox = 0;
      const char *proxp = p;
      while (*p)
	{
	  while (*p++)
	    ;
	  ++nprox;
	}
      cfg.proxies = malloc((1+nprox)*sizeof(const char *));
      p = proxp;
      nprox = 0;
      while (*p)
	{
	  cfg.proxies[nprox++] = p;
	  while (*p++)
	    ;
	}
      cfg.proxies[nprox] = NULL;
    }
  else
    {
      cfg.proxies = malloc(2*sizeof(const char *));
      cfg.proxies[0] = fn_project;
      cfg.proxies[1] = NULL;
    }
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
		   pqid,cfg.ext);
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
ce_file(const char *idp)
{
  static char proj[128];
  static char pqid[8];
  char *colon;
  const char *idp_on_entry = idp;

  if ((colon = strchr(idp, ':')))
    {
      strcpy(proj,idp);
      proj[colon-idp] = '\0';
      idp = colon+1;
    }
  else
    {
      *proj = '\0';
    }

  strncpy(pqid,idp,7);
  pqid[7] = '\0';

  if (idp[7] == '_' || project_en)
    {
      char base[128], *pd;
      pd = strchr(idp,'.');
      strncpy(base,idp,pd-idp);
      base[pd-idp] = '\0';

      if (!hash_find(seen_files,(unsigned char *)base))
	{
	  const char *fn = l2_findfile(proj,pqid,base);
	  if (fn)
	    list_add(files,strdup(fn));
	  hash_add(seen_files,(unsigned char *)strdup(base),(void*)1);
	}
    }
  else
    {
      if (!hash_find(seen_files,(unsigned char *)pqid))
	{
	  const char *fn = l2_findfile(proj,pqid,NULL);
	  if (fn)
	    list_add(files,strdup(fn));
	  /* add this to the hash regardless of found status;
	     we don't need to look for it repeatedly if it isn't there */
	  hash_add(seen_files,(unsigned char*)strdup(pqid),(void*)1);
	  list_add(pqs, npool_copy((unsigned char *)idp_on_entry, ce_pool));
	}
    }
}

static void
process_ce_ids(char *buf)
{
  char *dot, *colon;

  static unsigned char id_with_w[CE_BUFSIZ];
  int *ip = NULL;
  buf[strlen(buf)-1] = '\0';
  dot = strchr(buf,'.');
  if (dot && (dot = strchr(++dot,'.')))
    {
      strcpy((char*)id_with_w,buf);
      *dot = '\0';

      while (1)
	{
	  char *plus = strchr((const char *)id_with_w,'+');
	  static unsigned char pluses_left[CE_BUFSIZ];
	  if (plus)
	    {
	      strcpy((char*)pluses_left,plus+1);
	      *plus = '\0';
	    }
	  else
	    *pluses_left = '\0';

	  while (1)
	    {
	      char *comma = strchr((const char *)id_with_w,',');
	      char *wdot = strrchr((const char *)id_with_w,'.')+1;
		      
	      colon = strrchr((char*)id_with_w,':');
		      
	      if (comma)
		*comma = '\0';
	      if (colon)
		++colon;
		      
	      if (!(ip = hash_find(wh,(unsigned char *)(colon ? colon : (char*)id_with_w))))
		{
		  hash_add(wh,
			   (const unsigned char*)strdup((const char *)
							(colon 
							 ? colon 
							 : (char*)id_with_w)),
			   &hashvals[curr_hashval]);
		  hashvals[curr_hashval++] = 1;
		}
	      else
		*ip += 1;
		      
	      if (comma)
		{
		  ++comma;
		  memmove(wdot,comma,strlen(comma));
		  wdot[strlen(comma)] = '\0';
		  comma = NULL;
		}
	      else
		break;		      
	    }

	  if (*pluses_left)
	    strcpy((char*)id_with_w,(char*)pluses_left);
	  else
	    break;
	}

      if (!selecting_w)
	selecting_w = 1;
    }
  else
    *id_with_w = '\0';
  colon = strchr(buf,':');
  if (colon)
    ++colon;
  if (!(ip = hash_find(ht,(unsigned char *)(colon ? colon : buf))))
    {
      idps[curr_idp] = strdup(colon ? colon : buf);
      hash_add(ht,(const unsigned char *)idps[curr_idp++],
	       &hashvals[curr_hashval]);
      hashvals[curr_hashval++] = 1;
      if (cfg.f != ce_summary)
	ce_file(buf);
      if (pending_heading)
	{
	  hash_add(xtf_headings, npool_copy((unsigned char *)colon, ce_pool), pending_heading);
	  pending_heading = NULL;
	}
    }
  else
    {
      *ip += 1;
    }
}

static void
ce_ids()
{
  char buf[CE_BUFSIZ];

  if (id)
    {
      if (strlen(id) < CE_BUFSIZ)
	{
	  strcpy(buf, id);
	  process_ce_ids(buf);
	}
      else
	{
	  fprintf(stderr,"ce: error reading IDs from command line; argument too long\n");
	  exit(1);
	}
    }
  else
    {
      while (fgets(buf,CE_BUFSIZ,stdin))
	{
	  if ('#' == *buf)
	    {
	      if (cfg.f == ce_xmd)
		list_add(pqs, npool_copy(chomp((unsigned char *)buf), ce_pool));
	      else
		scan_heading(buf);
	    }
	  else
	    process_ce_ids(buf);
	}
      if (!feof(stdin))
	{
	  fprintf(stderr,"ce: error reading IDs; line too long?\n");
	  exit(1);
	}
    }
  idps[curr_idp] = NULL;
}

static void
ce_data(const char *xid)
{
  char *dot = strchr(xid,'.');
  char text_id[32];

  if (dot)
    {
      strncpy(text_id, xid, dot-xid);
      text_id[dot-xid] = '\0';
    }
  
  fprintf(stdout, "<ce:data project=\"%s\" text-id=\"%s\" line-id=\"%s\">",
	  project, text_id, xid);
}

void
printText(const char *s)
{
  while (*s)
    {
      if (*s == '<')
	fputs("&lt;",stdout);
      else if (*s == '&')
	fputs("&amp;",stdout);
      else if (*s == '"')
	fputs("&quot;",stdout);
      else
	putchar(*s);
      ++s;
    }
}

void
printStart(const char *name, const char **atts)
{
  const char **ap = atts;
  printText((const char*)charData_retrieve());
  printf("<%s",name);
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
	      && !strcmp(ap[1],"w") 
	      && hash_find(wh, (const unsigned char*)id))
	    {
	      printf(" class=\"%s selected\"",*++ap);
	      ++ap;
	    }
	  else
	    {
	      printf(" %s=\"",*ap++);
	      printText(*ap++);
	      putchar('"');
	    }
	}
    }
  putchar('>');
}

void
printEnd(const char *name)
{
  printText((const char *)charData_retrieve());
  printf("</%s>",name);
}

void
sH(void *userData, const char *name, const char **atts)
{
  if (!strcmp(name,cfg.tag))
    {
      const char *id = attr_by_name(atts,idattr);
#if 0
      if (id && project_en)
	strcpy(strchr(id,'_'),strchr(id,'.'));
#endif
      if (id && (idcountp = hash_find(ht,(const unsigned char*)id)))
	{
	  unsigned char *h = hash_find(xtf_headings, (unsigned char *)id);
	  if (idps[next_idp] && strcmp(idps[next_idp],id))
	    {
	      do
		fputs("<ce:data/>",stdout);
	      while (idps[++next_idp] && strcmp(idps[next_idp],id));
	    }
	  ++next_idp;
	  if (h)
	    fprintf(stdout, "<ce:heading>%s</ce:heading>", ++h);
	  if (echoing)
	    ++echoing;
	  else
	    {
	      charData_discard();
	      echoing = 1;
	      ce_data(id);
	    }

	  printStart(name,atts);
	}
    }
  else if (echoing)
    printStart(name,atts);
  else
    charData_discard();
}

void
eH(void *userData, const char *name)
{
  if (!strcmp(name,cfg.tag))
    {
      if (echoing)
	{
	  printEnd(name);
	  fputs("</ce:data>",stdout);
	  if (*idcountp > 1)
	    printf("<ids count=\"%%%d\"/>",*idcountp);
	  --echoing;
	  if (echoing < 0)
	    {
	      fprintf(stderr, "ce: echo stack underflow with tag %s\n", name);
	      exit(1);
	    }
	}
      else
	charData_discard();
    }
  else if (echoing)
    printEnd(name);
  else
    charData_discard();
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

int
main(int argc, char * const*argv)
{
  exit_on_error = TRUE;
  options(argc, argv, "2c:f:i:l:m:o:p:S:tvx");

  if (!project || !ce_index)
    {
      fprintf(stderr,"ce: must give -p and -i on command line\n");
      usage();
    }

  if (lang)
    sprintf(langindex,"%s/%s",ce_index,lang);
  else
    strcpy(langindex,ce_index);

  ce_pool = npool_init();
  files = list_create(LIST_SINGLE);
  pqs = list_create(LIST_SINGLE);
  ht = hash_create(1024);
  wh = hash_create(1024);
  xtf_headings = hash_create(1024);
  seen_files = hash_create(1024);
  ce_config();
  ce_ids();
  if (cfg.f == ce_summary)
    {
      static char fn[_MAX_PATH];
      if (lang)
	{
	  sprintf(fn,"/var/local/oracc/www/%s/%s/%s/summaries.html",
		  project,ce_index,lang);
	}
      else
	sprintf(fn,"/var/local/oracc/www/%s/%s/summaries.html",project,ce_index);
      list_add(files,fn);
      idattr = "id";
    }
  else if (cfg.f == ce_xmd)
    if (xmdinit2(project))
      exit(1);

  if (cfg.f == ce_xmd)
    {
      fprintf(stdout, "<ce:ce xmlns:ce=\"http://oracc.org/ns/ce/1.0\" xmlns:xh=\"http://www.w3.org/1999/xhtml\" cetype=\"xmd\" ncols=\"%d\">",xmd_field_count()+1);
      list_exec(pqs, (void(*)(void*))xmdprinter2);
      xmd_term();
    }
  else if (cfg.f == ce_byid)
    {
      fprintf(stdout, "<ce:ce xmlns:ce=\"http://oracc.org/ns/ce/1.0\" xmlns:xh=\"http://www.w3.org/1999/xhtml\" cetype=\"%s\">", ce_index);
      runexpat(i_list, list2charstarstar(files), sH, eH);
    }
  else
    {
      fprintf(stdout, "<ce:ce xmlns:ce=\"http://oracc.org/ns/ce/1.0\" xmlns:xh=\"http://www.w3.org/1999/xhtml\" cetype=\"summ\">");
      runexpat(i_list, list2charstarstar(files), sH, eH);
    }
  fputs("</ce:ce>",stdout);
  return 0;
}

int
opts(int argc, char *arg)
{
  switch (argc)
    {
    case 'c':
      id = arg;
      break;
    case 'f':
      xmd_fields = arg;
      break;
    case 'i':
      ce_index = arg;
      break;
    case 'l':
      lang = arg;
      break;
    case 'm':
      mode = arg;
      break;
    case 'o':
      item_offset = atoi(arg);
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
    case 'S':
      state = arg;
      break;
    case 'v':
      verbose = 1;
      break;
    case 'w':
      xmd_widths = arg;
      break;
    case 'x':
      ++xml_output;
      break;
    default:
      return 1;
    }
  return 0;
}

const char *prog = "ce";
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
