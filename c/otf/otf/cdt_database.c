#include <stdio.h>
#include <ctype128.h>
#include "cdt.h"
#include "cdt_foreign.h"
#include "xmlutil.h"
#include "c1_list.h"
#include "scan.h"

FILE *wfile;

struct cdt_database
{
  const char *type;
  List *records;
};

struct cdt_record
{
  List *fields;
};

struct cdt_field
{
  const char *name;
  unsigned char *text;
  struct cdt_node *inp;
};

static void *
db_init(const char *type)
{
  struct cdt_database *db = malloc(sizeof(struct cdt_database));
  db->type = type;
  db->records = list_create(LIST_SINGLE);
  return db;
}

static void
db_term(struct cdt_database *db)
{
  list_free(db->records,list_xfree);
  free(db);
}

static unsigned char *
scan_field(unsigned char *f,unsigned char **fp)
{
  unsigned char *ret = f;
  while (*f)
    {
      if ('\n' == *f)
	{
	  unsigned char *e = f;
	  while (isspace(*e))
	    ++e;
	  if (e[-1] == '\n' && '@' == *e && (':' == e[1] || !strncmp((const char*)&e[1],"end",3)))
	    {
	      unsigned char *zero = e;
	      while (isspace(zero[-1]))
		--zero;
	      *zero = '\0';
	      *fp = e;
	      return ret;
	    }
	}
      ++f;
    }
  *fp = f;
  return ret;
}

static void
db_reader(struct cdt_node *np)
{
  size_t local_lnum = np->lnum;
  unsigned char *ftext = np->text;
  struct cdt_record *rp = NULL;
  struct cdt_database *db = NULL;
  db = malloc(sizeof(struct cdt_database));
  db->type = np->name;
  db->records = list_create(LIST_SINGLE);
  np->children = list_create(LIST_SINGLE);
  list_add(np->children,db);
  while (*ftext)
    {
      ftext = skip_white(ftext,&local_lnum);
      if (*ftext == '@')
	{
	  unsigned char *tok, save;
	  tok = scan_token(ftext, &ftext, &save);
	  if (':' == *tok)
	    {
	      if (rp)
		{
		  struct cdt_field *fld;
		  list_add(rp->fields, fld = malloc(sizeof(struct cdt_field)));
		  *ftext = save;
		  tok = ftext;
		  fld->name = (const char *)tok;
		  while (!isspace(*tok))
		    ++tok;
		  if (*tok)
		    *tok++ = '\0';
		  while (isspace(*ftext))
		    ++ftext;
		  fld->text = scan_field(tok,&ftext);
		  if (fld->text && strchr(fld->text, '@'))
		    {
		      /* fprintf(stderr, "found @ in %s\n", fld->text); */
		      struct cdt_node *sp = cdt_string_node(NULL,fld->text,np->file,local_lnum);
		      cdt_inline(sp,NULL,NULL);
		      fld->inp = sp;
		    }
		  else
		    fld->inp = NULL;
		}
	    }
	  else if (!strcmp((const char *)tok,"record"))
	    {
	      list_add(db->records,rp = malloc(sizeof(struct cdt_record)));
	      rp->fields = list_create(LIST_SINGLE);
	      *ftext = save;
	    }
	  else if (!strcmp((const char *)tok,"end"))
	    {
	      *ftext = save;
	      tok = ++ftext;
	      while (' ' == *tok || '\t' == *tok)
		++tok;
	      if (!strncmp((const char *)tok,"record",6))
		{
		  if (rp)
		    rp = NULL;
		  else
		    cdt_warning(np->file,np->lnum,"misplaced '@end record'");
		}
	      else if (!strncmp((const char *)tok,np->name,strlen(np->name)))
		{
		  ftext = tok + strlen(np->name);
		  *ftext = '\0';
		}
	      else
		cdt_warning(np->file,np->lnum,"misplaced @end %s command", tok);
	    }
	  else
	    {
	      *ftext = save;
	      ++ftext;
	    }
	}
      else
	++ftext;
    }
}

static void
field_writer(struct cdt_field *fp)
{
  fprintf(wfile,"<oracc:field oracc:name=\"%s\">",fp->name);
  if (fp->inp)
    cdt_write_node_public(fp->inp);
  else
    fputs((const char *)xmlify(fp->text),wfile);
  fputs("</oracc:field>",wfile);
}

static void
record_writer(struct cdt_record *rp)
{
  fputs("<oracc:record>",wfile);
  list_exec(rp->fields,(list_exec_func*)field_writer);
  fputs("</oracc:record>",wfile);
}

static void
db_writer(FILE *fp, struct cdt_node *np)
{
  struct cdt_database *db = list_first(np->children);
  wfile = fp;
  fprintf(wfile,"<oracc:data oracc:type=\"%s\">",db->type);
  list_exec(db->records,(list_exec_func *)record_writer);
  fputs("</oracc:data>",wfile);
}

struct cdt_foreign cdt_database = { db_init , (cdt_foreign_term)db_term , db_reader , db_writer };
