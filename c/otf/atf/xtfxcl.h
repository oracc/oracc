#ifndef _Oracc_H
#define _Oracc_H

#include <psd_base.h>
#include <list.h>

#if 0
enum cdl_node_types { cdl_node_c , cdl_node_d , cdl_node_l };
enum cdl_c_types { cdl_c_text , cdl_c_discourse , cdl_c_chunk , 
		   cdl_c_sentence , cdl_c_clause , cdl_c_phrase ,
		   cdl_c_line_var , cdl_c_word_var };
enum cdl_d_types { cdl_d_line_start , cdl_d_break , 
		   cdl_d_cell_start , cdl_d_cell_end ,
		   cdl_d_gloss_start , cdl_d_gloss_end ,
		   cdl_d_punct };

enum cdl_sb_status	 { cdl_no_sb , cdl_sb };

struct cdl_c
{
  enum cdl_node_types node_type;
  enum cdl_c_types type;
  const char *id;
  struct cdl_c *parent;
  const char *const *meta;
  int nchildren;
  int children_alloced;
  union cdl_u*children;
};

struct cdl_d
{
  enum cdl_node_types node_type;
  enum cdl_d_types type;
  const char *subtype;
  enum cdl_sb_status sb;
  char *ref;
  const char *label;
};

#include "form.h"

struct cdl_l
{
  enum cdl_node_types node_type;
  char *ref;
  struct form *f;
  /* The syntax support probably needs improvement */
  const char *syn_ub_before;
  const char *syn_ub_after;
  const char *syn_brk_before;
  const char *syn_brk_after;
  /* Used by the ngram processor */
  int ro;
  struct cdl_l*alt; /* ambiguous forms are a singly-linked list */
};

union cdl_u
{
  struct cdl_c *c;
  struct cdl_d *d;
  struct cdl_l *l;
};
#endif

extern int do_cdl;
extern void cdl_serialize(void);
extern struct xcl_context *xcl_process(struct run_context *run, struct node *text);
#endif /*_Oracc_H*/
