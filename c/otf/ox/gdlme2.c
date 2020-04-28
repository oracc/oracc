#include <stdio.h>
#include <unistd.h>
#include <psd_base.h>
#include "options.h"
#include "runexpat.h"
#include "xmlutil.h"
#include "atf.h"
#include "cdf.h"
#include "gdl.h"
#include "lang.h"
#include "pool.h"
#include "npool.h"
#include "globals.h"

static int gdlme;
static int gdlme_debug = 0;
const char *outfile = NULL;
FILE *outfp = NULL;
extern int backslash_is_formvar;
extern int cbd_rules;
extern int do_signnames;
extern int math_mode;
extern int use_legacy;
extern int use_unicode;
const char *project;
static int stdin_input = 0;

char *gdl_file = NULL;
int gdl_file_len = 0;
int gdl_line = 0;

struct npool *gdl_pool = NULL;
struct xpd *xpd = NULL;

static unsigned char *
cbd_strip_backslash(unsigned char *in)
{
  static unsigned char *ret = NULL, *r;
  if (in)
    {
      if (strchr((const char *)in,'\\'))
	{
	  if (ret)
	    free(ret);
	  r = ret = malloc(strlen((const char *)in)+1);
	  while (*in)
	    {
	      if ('\\' == *in)
		{
		  while (*in && ' ' != *in)
		    ++in;
		  if (*in)
		    *r++ = *in++;
		}
	      else
		*r++ = *in++;
	    }
	  *r = '\0';
	  return ret;
	}
      else
	{
	  return in;
	}
    }
  else
    {
      if (ret)
	free(ret);
      ret = NULL;
      return NULL;
    }
}

static void
gdlme_sH(void *userData, const char *name, const char **atts)
{
  fprintf(f_xml,"%s",charData_retrieve());
  if (atts[0])
    {
      const char **attsp = atts;
      fprintf(f_xml,"<%s",name);
      while (attsp[0])
	{
	  if (!strcmp(attsp[0],"project"))
	    {
	      if (!xpd)
		xpd = xpd_init(attsp[1],gdl_pool);
	      fprintf(f_xml," %s=\"%s\"",attsp[0],xmlify((unsigned char *)attsp[1]));
	    }
	  else if (!strcmp(attsp[0],"g:me"))
	    gdlme = 1;
	  else if (!strcmp(attsp[0],"g:file"))
	    {
	      if (gdl_file_len <= strlen(attsp[1])+1)
		{
		  gdl_file_len = 2*strlen(attsp[1]);
		  gdl_file = realloc(gdl_file, gdl_file_len);
		}
	      file = strcpy(gdl_file,attsp[1]);
	    }
	  else if (!strcmp(attsp[0],"g:line"))
	    lnum = gdl_line = atoi(attsp[1]);
	  else
	    fprintf(f_xml," %s=\"%s\"",attsp[0],xmlify((unsigned char *)attsp[1]));
	  attsp += 2;
	}
      fputc('>',f_xml);
    }
  else
    {
      if (*name != 'r' || strcmp(name,"rp-wrap"))
	fprintf(f_xml,"<%s>",name);
    }
}

void
gdlme_eH(void *userData, const char *name)
{
  if (gdlme)
    {
      unsigned char *gdlinput = (unsigned char *)charData_retrieve();
      struct node *res = NULL;
      if (pi_file)
	{
	  file = (char*)pi_file;
	  lnum = pi_line;
	}

      if (cbd_rules)
	gdlinput = cbd_strip_backslash(gdlinput);
      
      if (gdlme_debug)
	fprintf(f_log, "%s:%d: gdlme processing `%s'\n", pi_file, pi_line, gdlinput);
      
      reset_forms_insertp();

      res = gdl(gdlinput, GDL_FRAG_OK);
      if (res && res->children.lastused)
	{
	  int i;
	  for (i = 0; i < res->children.lastused; ++i)
	    serialize(res->children.nodes[i],0);
	}
      else
	fprintf(f_log,"%s:%d: conversion of '%s' failed\n", pi_file, pi_line, gdlinput);
      gdlme = 0;
    }
  else
    {
      const char *s = charData_retrieve();
      fprintf(f_xml,"%s",(const char*)xmlify((unsigned char *)s));
    }
  if (*name != 'r' || strcmp(name,"rp-wrap"))
    fprintf(f_xml,"</%s>",name);
}

int
main(int argc, char **argv)
{
  const char *fname[2];

  options(argc,argv,"bcdgno:p:sux");

  if (!stdin_input)
    {
      fname[0] = file = argv[optind];
      fname[1] = NULL;
    }

  if (outfile)
    f_xml = outfp = xfopen(outfile,"w");
  else
    f_xml = stdout;
  f_log = stderr;
  
  gdl_pool = npool_init();
  galloc_init();
  pool_init();
  tree_init();
  gdl_init();
  cuneify_init(xpd);
  curr_lang = global_lang = lang_switch(NULL,"sux",NULL,NULL,0);

  /*current_state = set_state(s_global,s_text);*/
#if 0
  if (!project)
    {
      project = "cdli";
      load_lang_in_project(current_state.lang);
      charset_init_lang(curr_data->this);
      curr_data->cset = curr_data->this->cset[current_state.mode];
    }
#endif

  with_textid = 0;
  math_mode = no_pi = do_cuneify = use_unicode = 1;
  fputs("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n",f_xml);
  if (stdin_input)
    {
      runexpat(i_stdin,NULL,gdlme_sH,gdlme_eH);
    }
  else
    {
      if (!access(fname[0],R_OK))
	runexpat(i_list,fname,gdlme_sH,gdlme_eH);
      else
	fprintf(f_log,"gdlme: can't open %s for input\n",fname[0]);
    }

  (void)cbd_strip_backslash(NULL);
  npool_term(gdl_pool);
  lang_term();
  gdl_term();
  pool_term();
  tree_term(1);
  cuneify_term();
  galloc_term();
  if (gdl_file)
    free(gdl_file);
  if (outfp)
    xfclose(outfile,outfp);
  return 1;
}

int major_version = 1; int minor_version = 0;
const char *prog = "gdlme2";
const char *usage_string = "";
void help() { ; }
int opts(int och,char *oarg)
{
  switch (och)
    {
    case 'b':
      gdl_bootstrap = 1;
      break;
    case 'c':
      cbd_rules = 1;
      backslash_is_formvar = 1;
      break;
    case 'd':
      gdlme_debug = 1;
      break;
    case 'g':
      backslash_is_formvar = 0;
      break;
    case 'n':
      do_signnames = 1;
      break;
    case 'o':
      outfile = oarg;
      break;
    case 'p':
      project = oarg;
      break;
    case 's':
      stdin_input = 1;
      break;
    case 'u':
      use_unicode = 1;
      break;
    case 'x':
      no_gdl_xids = 0;
      break;
    default:
      return 1;
    }
  return 0;
}
