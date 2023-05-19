#include <ctype128.h>
#include "cdt.h"
extern const char *file;
extern size_t lnum;

static struct cdt_node *cdt_char_node(struct cdt_node *parent,
				      unsigned char *text, size_t lnum);
static struct cdt_node *cdt_span_node(struct cdt_node *parent,
				      const unsigned char *name, size_t lnum);
static struct cdt_node *cdt_ctag_node(struct cdt_node *parent,const unsigned char *name,
				      size_t lnum);

static unsigned char *discretionary = (unsigned char *)"\xc2\xad";

void
cdt_inline(struct cdt_node *np, struct cdt_node *parent, unsigned char *p)
{
  size_t local_lnum = np->lnum;
  if (!p)
    {
      p = np->text;
      parent = np;
    }
  if (!np->children)
    np->children = list_create(LIST_SINGLE);
  while (*p)
    {
      unsigned char *start = p;
      int found_at = 0;

      while (*p && '@' != *p)
	{
	  if ('\n' == *p)
	    ++local_lnum;
	  else if (('{' == *p || '}' == *p) && '\\' != p[-1])
	    cdt_warning(np->file,local_lnum,"misplaced '%c' or missing '\\'", *p);
	  ++p;
	}

      if (*p)
	{
	  found_at = 1;
	  *p = '\0';
	}

      if (p - start)
	{
	  list_add(np->children, cdt_char_node(np,start,local_lnum));
	  start = NULL;
	}

      if (found_at)
	{
	  ++p;
	  if (isalpha(*p))
	    {
	      const unsigned char *name = p;
	      while (*p && '{' != *p && !isspace(*p))
		++p;
	      if (!isspace(*p))
		{
		  struct cdt_node *span;
		  int nesting = 0;
		  *p++ = '\0';
		  start = p;
		  while (*p)
		    {
		      if ('{' == *p && '\\' != p[-1])
			++nesting;
		      else if ('}' == *p && '\\' != p[-1])
			{
			  if (nesting > 0)
			    --nesting;
			  else
			    {
			      *p++ = '\0';
			      break;
			    }
			}
		      ++p;
		    }
		  list_add(np->children, span = cdt_span_node(np,name,local_lnum));
		  if (!strcmp(span->name,"gdl"))
		    gdl_reader(span,start);
		  else if (!strcmp(span->name,"akk") || !strcmp(span->name,"sux") || !strcmp(span->name,"es"))
		    {
		      char *buf = malloc(strlen((char*)start)+6);
		      strcpy(buf,"%");
		      strcat(buf,span-name);
		      strcat(buf," ");
		      strcat(buf,(char*)start);
		      span->name = "gdl";
		      gdl_reader(span,(unsigned char*)buf);
		      free(buf);
		    }
		  else
		    cdt_inline(span,np,start);
		}
	      else if (!strncmp((const char *)name,"newline",strlen("newline")))
		{
		  struct cdt_node *cp = cdt_ctag_node(np,
						      (const unsigned char *)"line-break",
						      local_lnum);
		  cp->code = cdt_newline;
		  list_add(np->children, cp);
		  while (isspace(*p))
		    ++p;
		}
	      else
		{
		  unsigned char save = *p;
		  *p = '\0';
		  cdt_warning(np->file, np->lnum, "@%s: @-command without '{'", name);
		  *p = save;
		}
	    }
	  else if ('-' == *p)
	    {
	      list_add(np->children, cdt_char_node(np,discretionary,local_lnum));	      
	      ++p;
	    }
	  else
	    {
	      if (start)
		list_add(np->children, cdt_ctag_node(np,start,local_lnum));
	      else
		{
		  if (*p == '@')
		    list_add(np->children, cdt_char_node(np,"@",local_lnum));
		  else
		    cdt_warning(np->file,local_lnum,"orphan @-sign");
		}
	      ++p;
	    }
	}
    }
}

struct cdt_node *
cdt_string_node(struct cdt_node *parent,unsigned char *text,const char *file, size_t lnum)
{
  struct cdt_node *np = calloc(1,sizeof(struct cdt_node));
  np->name = "inline";
  np->file = file;
  np->lnum = lnum;
  np->parent = parent;
  np->text = text;
  np->class = cdt_none;
  np->code = odt_span;
  np->term = cdt_self;
  return np;
}

static struct cdt_node *
cdt_char_node(struct cdt_node *parent,unsigned char *text,size_t lnum)
{
  struct cdt_node *np = calloc(1,sizeof(struct cdt_node));
  np->name = "";
  np->file = parent->file;
  np->lnum = lnum;
  np->parent = parent;
  np->text = text;
  np->class = cdt_none;
  np->code = odt_char;
  np->term = cdt_self;
  return np;
}

static struct cdt_node *
cdt_span_node(struct cdt_node *parent,const unsigned char *name,size_t lnum)
{
  struct cdt_node *np = calloc(1,sizeof(struct cdt_node));
  np->name = (const char *)name;
  np->file = parent->file;
  np->lnum = lnum;
  np->parent = parent;
  np->class = cdt_none;
  np->code = odt_span;
  np->term = cdt_self;
  return np;
}

static int
cdt_bad_tag_name(const char *t)
{
  while (*t)
    if (*t < 128 && !isalnum(*t))
      return 1;
  return 0;
}

static struct cdt_node *
cdt_ctag_node(struct cdt_node *parent,const unsigned char *name,size_t lnum)
{
  struct cdt_node *np = calloc(1,sizeof(struct cdt_node));
  if (cdt_bad_tag_name(name))
    {
      cdt_warning(np->file,lnum,"bad tag name '%s'", name);
      name="BAD";
    }
  np->name = (const char *)name;
  np->file = parent->file;
  np->lnum = lnum;
  np->parent = parent;
  np->class = cdt_none;
  np->code = odt_ctag;
  np->term = cdt_self;
  return np;
}
