/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: list.h,v 0.5 1997/09/08 14:49:57 sjt Exp $
*/

#ifndef LIST_H_
#define LIST_H_ 1

#include <psd_base.h>
#include <array.h>
#include <dra.h>

struct List_node
{
  void *data;
  struct List_node *next;
  struct List_node *prev;
};
typedef struct List_node List_node;

enum List_types { LIST_SINGLE, LIST_DOUBLE, LIST_CIRCULAR, LIST_LIFO, LIST_FIFO };
typedef enum List_types List_types_e;

typedef void list_exec_func(void*);
typedef int list_find_func(const void*,const void*);
typedef void list_free_func(void*);

struct List_hdr
{
  List_types_e type;
  List_node *first;
  List_node *rover;
  List_node *last;
  size_t count;
};
typedef struct List_hdr List;

#define list_last(lp)     ((lp)->last ? (lp)->last->data : NULL)
#define list_has_next(lp) ((lp)->rover->next)
#define list_unnext(lp)   (lp->rover && (lp->rover=lp->rover->prev))
#define list_curr(lp) 	(lp)->rover->data
#define list_push	list_add
#define list_len(lp)	((lp) == NULL ? 0 : (lp)->count)
#define list_at_beginning(lp) ((lp)->rover == (lp)->first)

#define list2array 	c1_list2array
#define list_add    	c1_list_add
#define list_append 	c1_list_append
#define list_concat 	c1_list_concat
#define list_create 	c1_list_create
#define list_delete 	c1_list_delete
#define list_detach 	c1_list_detach
#define list_exec   	c1_list_exec
#define list_find   	c1_list_find
#define list_first  	c1_list_first
#define list_free   	c1_list_free
#define list_from_str  	c1_list_from_str
#define list_next   	c1_list_next
#define list_pop    	c1_list_pop
#define list_reset  	c1_list_reset
#define list_to_str 	c1_list_to_str
#define list_to_str2 	c1_list_to_str2
#define list_xfree	c1_list_xfree

extern unsigned char *list_concat(List *l);
extern List * list_create (List_types_e);
extern void list_add (List *, void *);
extern List * list_append(List*lp1,List*lp2);
extern void *list_detach (List *, List_node *);
extern void *list_pop (List *);
extern List *list_reset (List *);
extern void list_delete (List *, List_node *, void (*)(void *));
extern void list_free (List *, void (*)(void *));
extern void *list_reduce (Boolean, List *, 
			  int (*)(const void *,const void *), void (*)(void *));
extern void list_exec (List *, void (*)(void *));
extern void *list_first (List *);
extern void *list_next (List *);
extern void list_xfree (void *);
extern void *list_find (List *lp, const void *eltp, 
			int (*cmp)(const void*lelt, const void*celt));
extern List *list_from_str (char *str, char *(*tok)(char *), List_types_e type);
extern unsigned char *list_to_str(List *l);
extern unsigned char *list_to_str2(List *l, const char *sep);
extern void **list2array(List *l);
extern const char *vec_sep_str;
#endif /* LIST_H_ */
