#include <stdlib.h>
#include <ctype128.h>
#include <string.h>
#include <stdio.h>
#include "psd_base.h"
#include "fname.h"
#include "list.h"
#include "loadfile.h"
#include "npool.h"
#include "cdt.h"
#include "scan.h"
#include "run.h"
#include "proj_context.h"

extern const char *output_dir;

struct run_context *cdt_run;

static List *curr_section_stack = NULL;
static const char *curr_section = NULL, *curr_title = NULL;
static int in_document = 0;
List *inputs = NULL;
static int status;

static int figure_index, graphics_index;

static struct cdt_node *doc_sec_node = NULL;

static FILE*f_pictures;
static char *pictures_fname = NULL;

static struct npool *reader_pool;

const char *cdt_project = NULL;
/*static const char *curr_section = NULL;*/
static const char *section_first_p = NULL;
const char *h_names[] = { "h0",
			  "Heading_20_1",
			  "Heading_20_2",
			  "h3","h4","h5","h6","h7","h8","h9" };

static void image_attr(struct image *i, struct cdt_node *attr);
static const char *image_value(const char *s, const char *param, struct cdt_node *np, 
			       const char **s_end);
static void process_cdt_list(struct run_context *run,
			     unsigned char *ftext, struct cdt_node *parent, 
			     List *nodelist, List *metalist,
			     const char *end_tag, unsigned char **endp,
			     const char *local_file, size_t*local_lnump);
static struct cdt_node *add_node(List *nodes, struct cdt_node *tokp, 
				 struct cdt_node *parent,
				 const char *file, size_t lnum);
static int block_tester(const unsigned char *tokp);
static void coerce_document_section(struct cdt_node *np);

void
cdt_reader_init(void)
{
  if (!output_dir)
    output_dir = "01tmp";
  pictures_fname = malloc(strlen(output_dir)+strlen("odtpictures.lst") + 2);
  sprintf(pictures_fname, "%s/%s", output_dir, "odtpictures.lst");
  if (!curr_section_stack)
    curr_section_stack = list_create(LIST_DOUBLE);
  if (!(f_pictures = fopen(pictures_fname,"w")))
    {
      fprintf(stderr,"ox: can't open '%s'\n", pictures_fname);
      exit(1);
    }
  reader_pool = npool_init();
}

void
cdt_reader_term(void)
{
  if (curr_section_stack)
    {
      list_free(curr_section_stack,NULL);
      curr_section_stack = NULL;
    }
  fclose(f_pictures);
  free(pictures_fname);
  npool_term(reader_pool);
}

int
process_cdt_input(struct run_context *run, const char *fn, List *nodelist, List *metalist, struct cdt_node *np)
{
  unsigned char *ftext;
  const char *local_file = fn;
  size_t local_lnum = 1;

  if (xaccess(fn,R_OK,0))
    {
      char *newfile;
      if (!((newfile = cdt_findfile(fn))))
	{
	  if (np)
	    cdt_warning(np->file,np->lnum,"file `%s' not found",fn);
	  else
	    cdt_warning("(commandline)",0,"file `%s' not found",fn);
	  return 1;
	}
      local_file = (char*)npool_copy((unsigned char*)newfile,reader_pool);
    }
  if (!inputs)
    inputs = list_create(LIST_SINGLE);
  list_add(inputs, ftext = loadfile((unsigned const char *)local_file,NULL));
  process_cdt_list(run,ftext,NULL,nodelist,metalist,NULL,NULL,local_file,&local_lnum);
  return status;
}

static void
process_cdt_list(struct run_context *run,
		 unsigned char *ftext, struct cdt_node*parent, 
		 List *nodelist, List *metalist,
		 const char *end_tag, unsigned char **endp,
		 const char *local_file, size_t*local_lnump)
{
  size_t local_lnum = *local_lnump;
  while (*ftext)
    {
      struct cdt_node *tokp;
      unsigned char *tok, *endtok, save;
      ftext = skip_white(ftext,&local_lnum);
      if (*ftext == '@')
	{
	  tok = scan_token(ftext, &endtok, &save);
	  if ((tokp = cdttoks((const char *)tok,endtok-tok)))
	    {
	      struct cdt_node *np;
	    p_entry:
	      if (tokp->class == odt_meta && in_document < 2)
		np = add_node(metalist,tokp,parent,local_file,local_lnum);
	      else
		np = add_node(nodelist,tokp,parent,local_file,local_lnum);
	      np->attr = NULL;
	      if (save)
		*endtok = save;
	      if (np->code == odt_document)
		{
		  if (in_document)
		    coerce_document_section(np);
		  else
		    process_cdt_input(run, "/usr/local/oracc/lib/config/default.sty", 
				      nodelist, metalist, np);
		}
	      if (np->code == odt_h && isdigit(*endtok))
		np->name = h_names[*endtok++-'0'];
	      ftext = endtok;
	      if (np->code != cdt_end_tag)
		scan_square(np, endtok, &ftext, &local_lnum);
	      if (np->term != cdt_self)
		scan_text(np,ftext,&ftext,&local_lnum,block_tester);

	      switch (np->code)
		{
		case odt_title:
		  curr_title = (const char *)np->text;
		  if (doc_sec_node)
		    {
		      doc_sec_node->user = (unsigned char *)curr_title;
		      doc_sec_node = NULL;
		    }
		  break;
		case odt_section:
		  list_push(curr_section_stack, (void*)curr_section);
		  curr_section = (char*)np->text;
		  section_first_p = "1";
		  if (!np->attr)
		    np->attr = npool_copy(np->text,reader_pool);
		  np->user = (unsigned char *)curr_title;
		  break;
		case cdt_insert:
		  curr_section = "insert";
		  list_push(curr_section_stack, (void*)curr_section);
		  section_first_p = "1";
		  if (!np->attr)
		    np->attr = (unsigned char*)"here";
		  break;
		case cdt_figure:
		  {
		    struct figure *f = calloc(1,sizeof(struct figure));
		    np->user = f;
		    curr_section = "figure";
		    list_push(curr_section_stack, (void*)curr_section);
		    section_first_p = "1";
		    f->figure_num = ++figure_index;
		  }
		  break;
		case cdt_image:
		  {
		    struct image *i = calloc(1,sizeof(struct image));
		    static char *ftext_ret = NULL;
		    if (np->attr)
		      image_attr(i,np);
		    i->href = (const char *)npool_copy((unsigned char*)scan_curly((char*)ftext, &ftext_ret),
						       reader_pool);
		    ftext = (unsigned char *)ftext_ret;
		    i->alt = (const char *)npool_copy((unsigned char*)scan_curly((char*)ftext, &ftext_ret),
						      reader_pool);
		    ftext = (unsigned char *)ftext_ret;
		    i->path = (const char *)cdt_findfile((const char *)i->href);
		    fprintf(f_pictures,"%s\t%s\n",i->href,i->path ? i->path : "");
		    i->graphics_num = ++graphics_index;
		    np->user = i;
		  }
		  break;
		case odt_p:
		  if (!np->attr && curr_section)
		    {
		      np->attr = malloc(strlen(curr_section)+
					strlen(section_first_p)
					+3);
		      (void)sprintf((char*)np->attr,"%s.p%s",
				    curr_section,section_first_p);
		      section_first_p = "";
		    }
		  break;
		default:
		  break;
		}

	      switch (np->storage)
		{
		case cdt_none:
		  if (np->code == cdt_end_tag)
		    {
		      struct cdt_node *endnode;
		      if (in_document && !strcmp((char*)np->text, "document")
			  && !strcmp(end_tag,"section"))
			np->text = (unsigned char *)"section";
		      if (end_tag && strcmp((const char *)np->text, 
					    (const char *)end_tag))
			cdt_warning(np->file,np->lnum,
				    "mismatched end tag (expected %s found %s)", 
				    end_tag, np->text);
		      else if (!end_tag)
			cdt_warning(np->file,np->lnum,"junk after @end tag");
		      end_tag = NULL;
		      endnode = list_pop(nodelist);
		      free(endnode);
		      endnode = NULL;
		      curr_section = list_pop(curr_section_stack);
		      goto ret;
		    }
#if 0
		  else if (np->code == cdt_xml_file)
		    {
		      (void)list_pop(nodelist); /* first discard the @xmlfile node */
		      cdt_xinclude(run, (const char *)np->text, nodelist, metalist,np);
		    }
#endif
		  else if (np->code == cdt_include)
		    {
		      (void)list_pop(nodelist); /* first discard the @include node */
		      process_cdt_input(run, (const char *)np->text, nodelist, metalist,np);
		    }
		  else if (np->code == cdt_atf_file)
		    {
		      np->name = "atf";
		      np->class = odt_foreign;
		      np->code = foreign_atf;
		      np->file = (const char *)npool_copy((unsigned char *)cdt_findfile((const char*)np->text),reader_pool);
		      if (np->file)
			{
			  np->lnum = 1;
			  np->text = loadfile((unsigned char*)np->file,NULL);
			  cdt_data_handler(np, op_read, NULL);
			}
		      else
			{
			  cdt_warning(parent->file,np->lnum,"unable to find input file `%s'", np->text);
			}
		    }
		  else if (np->code == cdt_project_c)
		    {
		      char projstyles[_MAX_PATH];
		      proj_init(run, cdt_project = (const char *)np->text);
		      set_project(run->proj, cdt_project);
		      sprintf(projstyles,"/usr/local/oracc/pub/%s/project.sty",
			      project);
		      if (!xaccess(projstyles,R_OK,0))
			process_cdt_input(run, projstyles, nodelist, metalist,np);
		    }
		  else if (np->code == cdt_image
			   || np->code == cdt_newline
			   || np->code == cdt_newpage
			   || np->code == cdt_newoddpage
			   || np->code == cdt_pageno
			   || np->code == cdt_figno)
		    ;
		  else
		    cdt_warning(np->file,np->lnum,"unhandled tag with no storage type");
		  break;
		case cdt_list:
		  np->children = list_create(LIST_SINGLE);
		  if (np->code == odt_document)
		    ++in_document;
		  process_cdt_list(run,ftext, np, 
				   np->children, metalist, 
				   np->name, &ftext,
				   local_file, &local_lnum);
		  if (np->code == odt_document)
		    --in_document;
		  break;
		case cdt_style:
		  (void)list_pop(nodelist); /* drop the @style node */
		  cdt_style_handler(np);
		  break;
		case cdt_text:
		  cdt_inline(np,NULL,NULL);
		  if (np->code == cdt_caption)
		    {
		      if (!np->attr)
			np->attr = (unsigned char *)"below";
		      if (np->parent->code == cdt_figure)
			{
			  struct figure *f = np->parent->user;
			  f->caption = list_pop(np->parent->children);
			}
		      else
			cdt_warning(np->file,np->lnum,"@caption used without @figure");
		    }
		  break;
		case cdt_data:
		  cdt_data_handler(np,op_read,NULL);
		  break;
		default:
		  cdt_warning(np->file,np->lnum,"%s has unhandled storage type",np->name);
		  break;
		}
	    }
	  else
	    {
	      cdt_warning(local_file,local_lnum,"%s: unknown top-level @-command",tok);
	      ++ftext;
	      while (isalnum(*ftext))
		++ftext;
	    }
	}
#if 1
      else if (*ftext && !isspace(*ftext))
	{
	  tokp = cdttoks("p",1);
	  save = '\0';
	  endtok = tok = ftext;
	  goto p_entry;
	}
#else
      else if (*ftext && !isspace(*ftext))
	{
	  cdt_warning(local_file,local_lnum,"syntax error: expected @-command");
	  break;
	}
#endif
    }
 ret:
  if (end_tag)
    cdt_warning(local_file,local_lnum,"missing required '@end %s'", end_tag);
  *local_lnump = local_lnum;
  if (endp)
    *endp = ftext;
  return;
}

static struct cdt_node *
add_node(List *nodes, struct cdt_node *tokp, struct cdt_node *parent, const char *file, size_t lnum)
{
  struct cdt_node *np = malloc(sizeof(struct cdt_node));
  list_add(nodes, np);
  *np = *tokp;
  np->parent = parent;
  np->file = file;
  np->lnum = lnum;
  return np;
}

static int
block_tester(const unsigned char *tokp)
{
  return NULL != cdttoks((const char *)tokp,strlen((const char *)tokp));
}

static void
coerce_document_section(struct cdt_node *np)
{
  static const char *sec = "section";
  struct cdt_node tmp,  *tokp;
  tokp = cdttoks(sec,strlen(sec));
  tmp = *tokp;
  tmp.parent = np->parent;
  tmp.file = np->file;
  tmp.lnum = np->lnum;
  *np = tmp;
  np->text = (unsigned char *)"document";
  doc_sec_node = np;
}

static void
image_attr(struct image *i, struct cdt_node *np)
{
  const char *s = (const char *)np->attr, *s_end;

  while (*s)
    {
      while (isspace(*s))
	++s;
      if (!strncmp(s,"height",strlen("height")))
	i->height = image_value(s, "height", np, &s_end);
      else if (!strncmp(s,"width",strlen("width")))
	i->width = image_value(s, "width", np, &s_end);
      else if (!strncmp(s,"scale_x",strlen("scale_x")))
	i->scale_x = image_value(s, "scale_x", np, &s_end);
      else if (!strncmp(s,"scale",strlen("scale")))
	i->scale_x = image_value(s, "scale", np, &s_end);
      else if (!strncmp(s,"scale_y",strlen("scale_y")))
	i->scale_y = image_value(s, "scale_y", np, &s_end);
      else
	cdt_warning(np->file,np->lnum,
		    "bad attribute in @imgfile: expected width/height/scale_x/scale_y");
      s = s_end;
      if (!s)
	break;
      if (*s)
	++s;
    }
}
static const char *
image_value(const char *s, const char *param, struct cdt_node *np, const char **s_end)
{
  s += strlen(param);
  while (isspace(*s))
    ++s;
  if ('=' == *s)
    {
      char val[16], *vp;
      ++s;
      while (isspace(*s))
	++s;
      vp = val;
      while (*s && ',' != *s)
	{
	  *vp++ = *s++;
	  if (vp - val > 15)
	    {
	      cdt_warning(np->file,np->lnum,"value in @imgfile/%s too long (max 15)",param);
	      while (*s && ',' != *s)
		++s;
	      *s_end = s;
	      return NULL;
	    }
	}
      *vp = '\0';
      *s_end = s;
      return (char *)npool_copy((unsigned char *)val,reader_pool);
    }
  else
    cdt_warning(np->file,np->lnum,
		"bad value in @imgfile/%s: expected '='",param);
  return NULL;
}
