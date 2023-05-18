#include <stdlib.h>
#include <stdio.h>
#include <tree.h>
#include <prop.h>
#include <memo.h>
#include "gdlstream.h"
#include "gdl_word.h"

#define STREAMS_MAX	20

extern int gdl_bilingual;

int curr_stream = -1;

Node *streams[STREAMS_MAX];

static void
gdl_endword(int stream)
{
}

static Node *
gdl_wordtok(Node *tok, Node *wrd)
{
  return wrd;
}

static void
gdl_stream_push()
{
  /* save curr_stream to stack */
}

static int
gdl_stream_pop()
{
  return 0;
}

static void gdl_stream_set(int stream)
{
  curr_stream = stream;
}

/* The argument should be a Node * which is a container for the lowest
 * level of tokens at the line/cell/field level in the tree; wordify
 * collects the children of the container into a sequence of words and
 * nonwords.  The contained tokens may also have children--if they are
 * compound signs, for example-- but wordify does not recurse into
 * those children.
 *
 * For each token in the list there is a simple question: does this
 * token belong in the current word?
 *
 *  - If the token is a space, the answer is no--it ends the word.
 *
 *  - If the token is in the same stream, the answer is yes--the node
 *    is added to the word's children
 *
 *  - If the token is in a different stream, the answer is no
 *
 *    - this case always starts a new word
 *    
 *    - if the stream has a currently open word, then the new word and
 *      the currently open word are linked as segments of each other
 *
 * This has to be done in two phases, because a space finishes the
 * word in the current stream but may not finish the word in an
 * interrupted stream.
 *
 * An EOL ends the words in all open streams, unless the stream has an
 * EOL-continuation joiner, ';-'.
 *
 */
void
gdl_wordify(Node *container)
{
  Node *np = NULL, *wnp = NULL;
  int stream = -1;
  
  if (container && container->kids)
    {
      for (np = container->kids; np; np = np->next)
	{
	  if (np->name[2] == 'd' && np->text[0] == ' ' /*space*/)
	    {
	      /* end word in current stream */
	      if (wnp)
		{
		  gdl_endword(stream);
		  /*gdl_addword(np, stream);*/
		  wnp = NULL;
		}
	    }
	  else if (0 /* {( */)
	    {
	      gdl_stream_push();
	      gdl_stream_set(GDL_S7);
	    }
	  else if (0 /* }) */)
	    {
	      gdl_stream_pop();
	    }
	  else if (0 /* {{ */)
	    {
	      int sprop = -1;
	      gdl_stream_push();
	      if ((sprop = 0 /* has_stream_prop */))
		gdl_stream_set(sprop);
	      else
		gdl_stream_set(GDL_S8);
	    }
	  else if (0 /* }} */)
	    {
	      gdl_stream_pop();
	    }	  
	  else if (0 /* language switch */ && gdl_bilingual)
	    {
	      if (gdl_bilingual)
		{
		  gdl_stream_push();
		  /*gdl_stream_set(gdl_stream_of(lang));*/
		}
	      else
		gdl_wordtok(np, wnp);
	    }
	  else
	    wnp = gdl_wordtok(np, wnp);
	}
    }
}

#if 0
int
main(int argc, const char **argv)
{
  int x = atoi("13:");
  int y = -1;
  void *v = (void*)(uintptr_t)x;
  y = (int)(long)v;
  
  fprintf(stderr, "sizeof(struct gdlword) = %lu; int=%lu; stream=%d\n", sizeof(struct gdlword), sizeof(int), y);
}
#endif

