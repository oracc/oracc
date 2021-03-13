#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include "charsets.h"
#include "gdl.h"
#include "atffile.h"
#include "cdf.h"
#include "cdt.h"
#include "cdt_foreign.h"

extern void xtf_init(void);
extern void xtf_term(void);

extern const char *file;
extern int lnum;

extern FILE *f_xml;
extern int no_destfile, no_pi;

struct node;

struct cdt_atf
{
  const char *type;
  struct node *atf_tree;
};

static char tempname[16];

static void *
cdt_atf_init(const char *type)
{
  static struct cdt_atf *atfp = NULL;
  if (!atfp)
    {
      atfp = malloc(sizeof(struct cdt_atf));
      atfp->type = type;
      /*xtf_init();*/
      /*current_state = set_state(s_global,s_text);*/
      curr_lang = text_lang = global_lang;
      if (!cdt_project)
	cdt_project = "cdli";
#if 0
      load_lang_in_project(current_state.lang);
      charset_init_lang(curr_data->this);
      curr_data->cset = curr_data->this->cset[current_state.mode];
#endif
      /*      strcpy(tempname,"cdt_atf_XXXXXX"); */
      f_xml = tmpfile();
#if 0
      if (tmpfd >= 0)
	f_xml = fdopen(tmpfd,"r+");
#endif
      no_destfile = no_pi = 1;
    }
  else if (!type)
    {
      free(atfp);
      atfp = NULL;
    }
  return atfp;
}

static void
cdt_atf_term(struct cdt_atf *atfp)
{
  int fno = -1;
  /*atf free routine atfp->atf_tree*/
  /*xtf_term();*/
  (void)cdt_atf_init(NULL);
  fno = fileno(f_xml);
  fclose(f_xml);
  close(fno);
  unlink(tempname);
}

static void
finish_atf(struct cdt_node *np)
{
  long flen, actual;
  unsigned char *res;
  extern int verbose;

  fflush(f_xml);
  fseek(f_xml,0L,SEEK_END);
  flen = ftell(f_xml);
  if (verbose)
    fprintf(stderr, "cdt_atf: ATF flen = %ld\n", flen);
  fseek(f_xml,0L,SEEK_SET);
  res = malloc(flen+1);

  if (flen != (actual = fread(res,1,flen,f_xml)))
    {
      fprintf(stderr, "cdt_atf: ATF call returned %ld bytes but I only read %ld\n",
	      flen, actual);
    }
  res[flen] = '\0';
  np->children = list_create(LIST_SINGLE);
  list_add(np->children,res);

  ftruncate(fileno(f_xml),0);
  fseek(f_xml,0L,SEEK_SET);
}

static void
atf_reader(struct cdt_node *np)
{
  process_string(cdt_run, np->text,strlen((const char *)np->text));
  finish_atf(np);
}

static void
atf_writer(FILE *fp, struct cdt_node *np)
{
  if (np->children)
    {
      fputs("<xtf:xtf>",fp);
      fputs(list_first(np->children),fp);
      fputs("</xtf:xtf>",fp);
    }
}

void
gdl_reader(struct cdt_node *np, unsigned char *gdlinput)
{
  (void)cdt_atf_init("gdl");
  file = np->file;
  lnum = np->lnum;
  gdl_string(gdlinput, GDL_FRAG_OK);
  finish_atf(np);
}

void
gdl_writer(FILE *fp, struct cdt_node *np)
{
  if (np->children)
    {
      fputs("<gdl:gdl>",fp);
      fputs(list_first(np->children),fp);
      fputs("</gdl:gdl>",fp);
    }
}

struct cdt_foreign cdt_atf = { cdt_atf_init , (cdt_foreign_term)cdt_atf_term , atf_reader , atf_writer };
