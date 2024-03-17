/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: index.h,v 0.5 1997/09/08 14:50:18 sjt Exp sjt $
*/
#ifndef INDEX_H_
#define INDEX_H_ 1

#include <stdio.h>
#include <stdlib.h>
#include "../se.h"

struct indexed
{
  struct node *nodes;
  struct indexed *next;
  int id;
};
struct node 
{
  struct node *left, *right;
  struct node *down, *up;
  struct parallel *parallel_info;
  int boundary;
  Char *branch_name;
  int used_flag;
  int end_of_branch_flag;
  struct location16 l;
};
struct parallel
{
  struct parallel *parent;
  struct parallel *child;
  struct node *start_node;
  struct node *curr_node;
  int start_column;
  int end_column;
  int boundary;
  int word_id;
  int empty_branch_flag;
};
struct grapheme
{
  Char *text;
  struct node *node;
  struct grapheme *next;
};
struct node_list 
{
  struct node *node;
  struct node_list *next;
};

enum pending_boundary_types { pb_none , pb_space , pb_hyphen };

extern Four_bytes curr_label_id;
extern Four_bytes record_id;
extern int debug_flag, index_flag, words_flag;
extern int found_label_yet;
extern int interchangeable_flag;
extern int option_flag;
extern int curr_text_id;
extern int curr_unit_id;
extern int curr_word_id;
extern int branch_id;
extern int parallels_depth;
extern struct parallel *curr_parallel;
extern struct grapheme *grapheme_list_base, *grapheme_list;
extern int start_column;
extern int word_id;
extern struct indexed *curr_indexed;
extern struct node *curr_node;
extern Four_bytes curr_line;
extern Char *curr_text;
extern Char **filelist;

extern struct any_type *indexed_mm;
extern struct any_type *parallels_mm;
extern struct any_type *grapheme_mm;
extern struct any_type *node_mm;

extern void doctype (char *yytext);
extern void begin_indexed (void);
extern void dump_graphemes (void);
extern void add_graphemes (void);
extern void end_indexed (void);
extern void begin_branch (void);
extern void end_branch (void);
extern void grapheme (const char *text);
extern void grapheme_boundary (int btype);
extern void begin_insert (void);
extern void end_insert (void);
extern struct node * new_node (void);
extern void begin_parallels (void);
extern void end_parallels (void);
extern void begin_option (void);
extern void end_option (void);
extern void begin_interchangeable (void);
extern void end_interchangeable (void);
extern void label (Char *text);
extern Char *savestr (Char *text);
extern void text (Char *text);
extern void text_add_label (Char *label);
#include "mm_any.h"
#endif
