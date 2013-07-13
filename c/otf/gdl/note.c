#include <stdlib.h>
#include <string.h>
#include <wctype.h>
#include "list.h"
#include "cdf.h"
#include "memblock.h"
#include "npool.h"
#include "translate.h"
#include "tree.h"
#include "warning.h"
#include "note.h"
#include "xmlnames.h"

static List *notes_in_line = NULL;
static List *notes_in_text = NULL;
static struct mb *mb = NULL;
static struct npool *note_pool = NULL;

enum note_status
{
  NOTE_REGISTERED,
  NOTE_REFERENCED
};

struct note
{
  const unsigned char *mark;
  struct node *node;
  int status;
};


static const char *note_create_id(void);
static struct note *note_find(const unsigned char *n);

static int
note_cmp(struct note *np, const char *mark)
{
  return strcmp((char*)np->mark, mark);
}

/* cur here is the current node from block.c, which is the parent of
   the block in which the current #note: line which is being processed.

   We need to look back along the list for the first l node, which may
   be inside an lg.  If it is, we should look for an 'mts' line in the
   lg group, otherwise just use the first l child of lg for the attach
   point.
   
   If all that fails, we can return NULL for now and figure out the
   boundary cases later.
 */
static struct node *
note_attach_lg(struct node *lg)
{
  int i;
  struct nodelist *nlp = &lg->children;
  for (i = 0; i < nlp->lastused; ++i)
    {
      if (((struct node *)nlp->nodes[i])->etype == e_l)
	{
	  const unsigned char *type = getAttr(nlp->nodes[i], "type");
	  if ('\0' == *type)
	    return nlp->nodes[i];
	}      
    }
  return nlp->nodes[0];
}

static struct node *
note_attach_point(struct node *curr)
{
  int i;
  struct nodelist *nlp;

  if (curr)
    {
      nlp = &curr->children;
      for (i = nlp->lastused-1; i >= 0; --i)
	{
	  if (((struct node *)nlp->nodes[i])->etype == e_l)
	    return nlp->nodes[i];
	  else if (((struct node *)nlp->nodes[i])->etype == e_lg)
	    return note_attach_lg(nlp->nodes[i]);
	}
      /* there were no children: attach at curr--if it's not a legit
	 block point to attach at caller will complain */
      return curr;
    }
  else
    {
      warning("internal error: note_attach_point passed NULL curr");
      return NULL;
    }
}

static const char *
note_create_id(void)
{
  static int note_id = 0;
  char *nid = malloc(16);
  (void)sprintf(nid,"n%03d",++note_id);
  return nid;
}

static struct note *
note_find(const unsigned char *n)
{
  return notes_in_line 
    ? list_find(notes_in_line, n, (list_find_func*)note_cmp)
    : list_find(notes_in_text, n, (list_find_func*)note_cmp)
    ;
}

void
note_initialize(void)
{
  mb = mb_init(sizeof(struct node), 64);
  note_pool = npool_init();
}

void
note_initialize_line(void)
{
  if (notes_in_line)
    {
      list_free(notes_in_line, NULL);
      notes_in_line = NULL;
    }
  notes_in_line = list_create(LIST_SINGLE);
}

void
note_initialize_text(void)
{
  if (notes_in_text)
    list_free(notes_in_text, NULL);
  notes_in_text = list_create(LIST_SINGLE);
}

/* parent node is "current" node in block.c */
int
note_parse_tlit(struct node *parent, int current_level, unsigned char **lines)
{
  int nlines;
  struct node *n;
  char markbuf[8], *m = markbuf;
  unsigned char *notelabel = NULL, *notetext = NULL;
  const unsigned char *mark;

  *markbuf = '\0';
  lines[0] += 6;
  while (isspace(lines[0][0]))
    ++lines[0];
  if ('^' == lines[0][0])
    {
      ++lines[0];
      while (lines[0][0] && '^' != lines[0][0])
	{
	  *m++ = lines[0][0];
	  ++lines[0];
	}
      *m = '\0';
      ++lines[0];
      mark = (const unsigned char *)markbuf;
    }
  else
    {
      struct node *lastC = note_attach_point(parent);
      if (lastC)
	{
	  struct node *xmark = NULL;
	  enum e_type e;
	  enum block_levels l;
	  switch (lastC->etype)
	    {
	    case e_l:
	      {
		struct node *lastCchild = lastChild(lastC);
		if (lastCchild->etype == e_c)
		  {
		    /* the attach point is either the cell or its
		       chield field if there is one */
		    struct node *cField = lastChild(lastCchild);
		    if (cField->etype == e_f)
		      lastC = cField;
		    else
		      lastC = lastCchild;
		    l = WORD;
		  }
		else
		  l = LINE;
		e = e_g_nonw;
	      }
	      break;
	    case e_object:
	      l = OBJECT;
	      e = e_note_link;
	      break;
	    case e_surface:
	      l = SURFACE;
	      e = e_note_link;
	      break;
	    case e_column:
	      l = COLUMN;
	      e = e_note_link;
	      break;
	    default:
	      vwarning("unhandled note parent %s", lastC->names[0]);
	      break;
	    }
	  xmark = elem(e,NULL,lnum,l);
	  if (e == e_g_nonw)
	    appendAttr(xmark, attr(a_type, (unsigned char *)"notelink"));
	  appendChild(lastC, xmark);
	  mark = note_register_mark(NULL, xmark);
	}
      else
	{
	  warning("nowhere to attach note mark to; please provide context and mark");
	  mark = NULL;
	}
    }

  while (isspace(lines[0][0]))
    ++lines[0];
  if (!strncmp((char*)lines[0],"@notelabel{", 11))
    {
      lines[0] += 11;
      notelabel = lines[0];
      while (lines[0][0] != '}')
	++lines[0];
      lines[0][0] = '\0';
      ++lines[0];
      while (isspace(lines[0][0]))
	++lines[0];
    }

  n = elem(e_note_text,NULL,lnum,current_level);
  if (mark)
    {
      appendAttr(n, attr(a_note_mark, mark));
      note_register_note(mark, n);
    }

  if (notelabel)
    set_or_append_attr(n,a_note_label,"notelabel",notelabel);

  /* This is a bit weird, but the last character before the content is
     either a space after #note:, or a space or the closer character
     after a note mark or label, so we are safe to play this trick
     with the scan_comment routine */
  --lines[0];
  lines[0][0] = '#';
  notetext = npool_copy(scan_comment_sub(lines,&nlines,0), note_pool);
  (void)trans_inline(n,notetext,NULL,0);
  appendChild(parent,n);
  return nlines;
}

const unsigned char *
note_register_mark(const unsigned char *mark, struct node *parent)
{
  if (!mark)
    {
      if (notes_in_line)
	{
	  struct note *last_np = list_last(notes_in_line);
	  if (last_np)
	    {
	      int m = atoi((char*)last_np->mark);
	      if (m > 0)
		{
		  static char buf[10];
		  sprintf(buf, "%d", m+1);
		  return note_register_mark((const unsigned char *)buf, parent);
		}
	      else
		/* this is a stop-gap; it means that alpha notes can be done
		   explicitly, but they'll get mixed with numeric marks if
		   no mark is used in a #note: */
		return note_register_mark((const unsigned char *)"1", parent); 
	    }
	  else
	    return note_register_mark((const unsigned char *)"1", parent); 
	}
      else
	{
	  return note_register_mark((const unsigned char *)"1", parent);
	}
    }

  if (note_find(mark))
    {
      vwarning("note mark %s is used more than once in this line");
      return NULL;
    }
  else
    {
      struct note *np = mb_new(mb);
#if 1
      unsigned char *note_mark_text = npool_copy(mark, note_pool);
      struct node *note_mark_node = parent;
#else
      struct node *note_mark_node = elem(e_note_mark, NULL, lnum, WORD);
      unsigned char *note_mark_text = npool_copy(mark, note_pool);
      appendChild(parent, note_mark_node);
#endif
      appendChild(note_mark_node, textNode(note_mark_text));
      np->mark = note_mark_text;
      np->node = note_mark_node;
      np->status = NOTE_REGISTERED;
      if (notes_in_line)
	list_add(notes_in_line, np);
      list_add(notes_in_text, np);
      return mark;
    }
}

/* should we create the node here or let it be created by caller as now? */
void
note_register_note(const unsigned char *mark, struct node *node)
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
	      setAttr(node, a_xml_id, (const unsigned char *)nid);
	      setAttr(np->node, a_note_ref, (const unsigned char *)nid);
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
