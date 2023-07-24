/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: list.h,v 0.5 1997/09/08 14:49:57 sjt Exp $
*/

#ifndef LIST_H_
#define LIST_H_ 1

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <c2types.h>

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
#define list_more(lp)	  ((lp)->rover != (lp)->last)
#define list_has_next(lp) ((lp)->rover->next)
#define list_unnext(lp)   (lp->rover && (lp->rover=lp->rover->prev))

extern unsigned char *list_concat(List *l);
extern unsigned char *list_join(List *l, const char *j);
extern List * list_create (List_types_e);
extern void list_add (List *, void *);
extern List * list_append(List*lp1,List*lp2);
extern void *list_detach (List *, List_node *);
extern void *list_pop (List *);
extern List *list_reset (List *);
extern void list_delete (List *, List_node *, void (*)(void *));
extern void list_free (List *, void (*)(void *));
#if 0
extern void *list_reduce (Boolean, List *, 
			  int (*)(const void *,const void *), void (*)(void *));
#endif
extern void list_exec (List *, void (*)(void *));
extern void *list_first (List *);
extern void *list_next (List *);
extern void list_xfree (void *);
extern void *list_find (List *lp, const void *eltp, 
			int (*cmp)(const void*lelt, const void*celt));
extern List *list_from_str (char *str, char *(*tok)(char *), List_types_e type);
extern unsigned char *list_to_str(List *l);
unsigned char *list_to_str2(List *l, const char *sep);
#define list_curr(lp) 	(lp)->rover->data
#define list_push	list_add
#define list_len(lp)	((lp) == NULL ? 0 : (lp)->count)
#define list_at_beginning(lp) ((lp)->rover == (lp)->first)
extern const char *vec_sep_str;
extern void **list2array(List *l);
extern const char **list2chars(List *l);

#endif /* LIST_H_ */
