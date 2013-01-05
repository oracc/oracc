#ifndef _CDT_H
#define _CDT_H
#include <stdlib.h>
#include "hash.h"
#include "list.h"
#include "run.h"
#include "cdt_foreign.h"
enum cdt_codes { odt_foreign, odt_meta, odt_text, odt_char,
		 odt_document, odt_title, odt_section, odt_h, 
		 odt_p, odt_span, odt_note, odt_ctag, cdt_project_c,
		 odt_style_set, odt_style,
		 cdt_atf_file, cdt_include, cdt_end_tag,
		 cdt_insert, cdt_figure, cdt_caption, 
		 cdt_image, cdt_newline, cdt_newpage, cdt_newoddpage,
		 cdt_pageno, cdt_figno,
		 foreign_atf, 
		 foreign_bibliography, 
		 foreign_catalog,
		 odt_none
};

enum cdt_terms { cdt_white, cdt_line, cdt_para, cdt_end, cdt_self };

enum cdt_store { cdt_list, cdt_text, cdt_data, cdt_style, cdt_none };

enum stylenodes { style_face , style_default , style_style };

struct cdt_node
{
  const char *name;
  enum cdt_codes class;
  enum cdt_codes code;
  enum cdt_terms term;
  enum cdt_store storage;
  unsigned char *attr;
  unsigned char *text;
  const char *file;
  size_t lnum;
  struct cdt_node*parent;
  List *children;
  void *user;
};

#include "hash.h"
struct styles
{
  List *faces;
  List *defaults;
  List *styles;
  Hash_table *faces_hash;
  Hash_table *defaults_hash;
  Hash_table *styles_hash;
};

struct style_node
{
  enum stylenodes type;
  const unsigned char *name;
  const unsigned char *family;
  List *props;
  List *xfams;
};

struct style_prop
{
  const unsigned char *name;
  const unsigned char *value;
};

enum style_props { 
  sp_column , sp_paragraph , sp_section , 
  sp_tabstop, sp_text, sp_unknown
};
struct style_prop_fam
{
  const char *name;
  enum style_props type;
};
extern struct style_prop_fam *style_props(const char *,unsigned int);

struct style_xfam
{
  const unsigned char *name;
  List *props;
};

struct figure
{
  const char *style;
  struct cdt_node *caption;
  struct image *image;
  int figure_num;
};

struct image
{
  const char *href; /* as given in the input */
  const char *path; /* as located by cdt_findfile */
  const char *alt;  /* for XHTML's alt attribute */
  const char *height;
  const char *width;
  const char *scale_x;
  const char *scale_y;
  int graphics_num;
};

extern struct run_context *cdt_run;

extern const char *automatic_styles, *cdt_project, *master_styles, *project, *styles_styles;
extern void cdt_data_handler(struct cdt_node *np, enum foreign_data_ops op, FILE*fp);
extern void cdt_style_handler(struct cdt_node *np);
extern void cdt_inline(struct cdt_node *np, struct cdt_node *parent,unsigned char *p);
extern int process_cdt_input(struct run_context *run,
			     const char *fn, List *nodelist, List *metalist, struct cdt_node *np);
extern void cdt_reader_init(void);
extern void cdt_reader_term(void);
extern void cdt_styles_init(void);
extern void cdt_styles_term(void);
extern void cdt_style_writer(FILE *wfile);
extern void cdt_warning(const char *myfile, size_t mylnum, const char *fmt,...);
extern void cdt_writer(FILE*wout, List *master, List *metalist);
extern struct cdt_node*cdttoks(register const char *str,register unsigned int len);
extern void gdl_reader(struct cdt_node *np, unsigned char *gdlinput);
extern void gdl_writer(FILE *f,struct cdt_node *np);
extern void set_project(struct proj_context *p, const char *proj);
extern char *cdt_findfile(const char *file);

#endif/*_CDT_H*/
