#include "tree.h"
#include "list.h"
#include "npool.h"

static int note_id = 0;
static List *notes_in_line = NULL;
static node *note_line_node;
static npool *note_pool = NULL;

enum note_status
{
  NOTE_REGISTERED,
  NOTE_REFERENCED,
};

struct note
{
  const unsigned char *mark;
  struct node *node;
  int status;
};

struct note *
note_find(void)
{
  
}

void
note_initialize(void)
{
  note_pool = npool_init();
}

void
note_initialize_text(void)
{
  note_check_registered();
  note_id = note_markless_mark = 0;
  list_free(notes_in_line);
  notes_in_line = NULL;
  notes_in_line = list_create(LIST_SINGLE);
}

void
note_initialize_line(void)
{
  note_check_registered();
  if (notes_in_line)
    list_free(notes_in_line);
  notes_in_line = list_create(LIST_SINGLE);
}

void
note_register_mark(const unsigned char *mark, struct node *parent)
{
  if (!mark)
    {
      struct note *last_np = list_last(notes_in_line);
      if (last_np)
	{
	  int m = atoi(last_np->mark);
	  if (m > 0)
	    {
	      static char buf[10];
	      sprintf(buf, "%d", m+1);
	      note_register_mark(buf, parent);
	    }
	  else
	    note_register_mark("1", parent); /* this is a stop-gap; it means that alpha notes can be done explicitly, but they'll get mixed with numeric marks
						if no mark is used in a #note: */
	}
      else
	{
	  note_register_mark("1", parent);
	}
    }

  if (note_find(mark))
    {
      vwarning("note mark %s is used more than once in this line");
    }
  else
    {
      struct note *np = note_new_note();
      struct node *note_mark_node = elem(e_note_mark);
      unsigned char *note_mark_text = npool_copy(note_pool, mark);
      appendChild(parent, note_mark_node);
      setAttr(note_mark_node, a_note_mark, note_mark_text);
      np->mark = note_mark_text;
      np->node = note_mark_node;
      np->status = NOTE_REGISTERED;
      list_add(notes_in_line, np);
    }
}

/* should we create the node here or let it be created by caller as now? */
void
note_register_text(const unsigned char *mark, struct node *node)
{
  struct note *np = NULL;

  if (mark)
    {
      np = note_find(mark);
      if (np)
	{
	  if (np->status == NOTE_REGISTERED)
	    {
	      const char *nid = note_create_id();
	      setAttr(node, a_xml_id, nid);
	      setAttr(np->node, a_note_ref, nid);
	    }
	  else
	    {
	      vwarning("note mark %s has multiple notes attached to it", mark);
	    }
	}
      else
	{
	  vwarning("note mark %s was not used in the preceding line", mark);
	}
    }
  else
    {
      warning("note_register_text passed NULL mark");
    }
}
