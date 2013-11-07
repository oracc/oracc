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

/* DOCO:
   
   Change language in doco because number in ^...^ is now note tag not note mark.
   
   Note mark is always assigned by Oracc and notes can be sequential within a text
   (the default) or sequential over an entire run (useful for books).

   Need to implement note xrefs--P123456.o_i_1^3 should resolve to the appropriate
   link (or page number if using TeX or OTF).  [Possibly implement general 
   target/ref system instead?]
 */

static int notes_by_text = 1;
static int note_index = 1;
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
  const unsigned char *tag; 	/* the tag in a text, e.g., 1 in
				   ^1^--this is optional */
  const unsigned char *mark; 	/* the note number in the text or run;
				   this is what is output as the note
				   number */
  struct node *node; 		/* where to attach the note mark to */
  enum note_status status;	/* registered and or referenced */
};


static const char *note_create_id(void);
static struct note *note_find_in_line(const unsigned char *n);

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

static int
note_cmp(struct note *np, const char *tag)
{
  return strcmp((char*)np->tag, tag);
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
note_find_in_line(const unsigned char *n)
{
  return list_find(notes_in_line, n, (list_find_func*)note_cmp);
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
    list_free(notes_in_line, NULL);
  notes_in_line = list_create(LIST_SINGLE);
  /*note_index = 1;*/
}

void
note_initialize_text(void)
{
  if (notes_by_text)
    {
      if (notes_in_text)
	list_free(notes_in_text, NULL);
      notes_in_text = list_create(LIST_SINGLE);
      note_index = 1;
    }
  else
    notes_in_text = list_create(LIST_SINGLE);
}

void
note_number_by_run(void)
{
  notes_by_text = 0;
}

/* parent node is "current" node in block.c */
int
note_parse_tlit(struct node *parent, int current_level, unsigned char **lines)
{
  int nlines;
  struct node *n;
  char tagbuf[8], *m = tagbuf;
  unsigned char *notelabel = NULL, *notetext = NULL;
  const unsigned char *tag = NULL, *mark = NULL;

  *tagbuf = '\0';
  lines[0] += 6;
  while (isspace(lines[0][0]))
    ++lines[0];
  
  if ('^' == lines[0][0])
    {
      struct note *np;
      /* the note should already be registered at the tag-point in the line */
      ++lines[0];
      while (lines[0][0] && '^' != lines[0][0])
	{
	  *m++ = lines[0][0];
	  ++lines[0];
	}
      *m = '\0';
      ++lines[0];
      tag = (const unsigned char *)tagbuf;
      np = note_find_in_line(tag);
      if (np)
	{
	  mark = np->mark;
	}
      else
	{
	  warning("tag in note does not have corresponding tag in preceding line");
	  return 1;
	}
    }
  else
    {
      if (list_len(notes_in_line))
	{
	  warning("tagged notes cannot be mixed with untagged ones");
	  return 1;
	}
      else
	{
	  struct node *lastC = note_attach_point(parent);
	  
	  /* If there is no note tag we have to do two things: fix the attach point and set the tag to "1" */
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
	      tag = "1";
	      mark = note_register_tag(tag, xmark);
	    }
	  else
	    {
	      warning("nowhere to attach note mark to; please provide context and mark");
	      tag = NULL;
	    }
	}
    }

  if (tag)
    {
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
      appendAttr(n, attr(a_note_mark, mark));
      note_register_note(tag, n);

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
    }

  return nlines;
}

const unsigned char *
note_register_tag(const unsigned char *tag, struct node *parent)
{
  if (!tag)
    {
      if (notes_in_line)
	{
	  struct note *last_np = list_last(notes_in_line);
	  if (last_np)
	    {
	      int m = atoi((char*)last_np->tag);
	      if (m > 0)
		{
		  static char buf[10];
		  sprintf(buf, "%d", m+1);
		  return note_register_tag((const unsigned char *)buf, parent);
		}
	      else
		/* this is a stop-gap; it means that alpha notes can be done
		   explicitly, but they'll get mixed with numeric marks if
		   no mark is used in a #note: */
		return note_register_tag((const unsigned char *)"1", parent);
	    }
	  else
	    return note_register_tag((const unsigned char *)"1", parent);
	}
      else
	{
	  return note_register_tag((const unsigned char *)"1", parent);
	}
    }

  if (note_find_in_line(tag))
    {
      vwarning("note tag %s is used more than once in this line", tag);
      return NULL;
    }
  else
    {
      struct note *np = mb_new(mb);
      unsigned char *note_mark_text = NULL;
      struct node *note_mark_node = parent;
      if (note_index < 1000000)
	{
	  unsigned char markbuf[8];
	  sprintf((char*)markbuf,"%d",note_index++);
	  note_mark_text = npool_copy(markbuf, note_pool);
	}
      /* If there was a ^1^ tag in the line we need to replace the text
	 content of the parent element here; otherwise, we have a fresh
	 parent element and just need to append the text node */
      if (note_mark_node->children.lastused)
	((struct node*)(note_mark_node->children.nodes[0]))->data = note_mark_text;
      else
	appendChild(note_mark_node, textNode(note_mark_text));
      np->tag = tag;
      np->mark = note_mark_text;
      np->node = note_mark_node;
      np->status = NOTE_REGISTERED;
      if (notes_in_line)
	list_add(notes_in_line, np);
      /* list_add(notes_in_text, np); */
      return tag;
    }
}

/* should we create the node here or let it be created by caller as now? */
void
note_register_note(const unsigned char *tag, struct node *node)
{
  struct note *np = NULL;

  if (tag)
    {
      np = note_find_in_line(tag);
      if (np)
	{
	  if (np->status == NOTE_REGISTERED)
	    {
	      const char *nid = note_create_id();
	      setAttr(node, a_xml_id, (const unsigned char *)nid);
	      setAttr(np->node, a_note_ref, (const unsigned char *)nid);
	      np->status = NOTE_REFERENCED;
	    }
	  else
	    {
	      vwarning("note tag %s has multiple notes attached to it", tag);
	    }
	}
      else
	{
	  vwarning("note tag %s was not used in the preceding line", tag);
	}
    }
  else
    {
      warning("note_register_note passed NULL tag");
    }
}

int
note_validate_line(void)
{
  int status = 0;
  if (notes_in_line)
    {
      struct note *np;
      for (np = list_first(notes_in_line); np; np = list_next(notes_in_line))
	{
	  if (np->status == NOTE_REGISTERED)
	    {
	      warning("note tag %s has no note belonging to it");
	      ++status;
	    }
	}
    }
  return status;
}
