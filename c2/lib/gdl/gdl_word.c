#include <stdlib.h>
#include <stdio.h>
#include <tree.h>
#include <prop.h>
#include <memo.h>
#include "gdl_word.h"

#define STREAMS_MAX	20

Node *streams[STREAMS_MAX];

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
  Node *np = NULL;
  int stream = -1;
  
  if (container && container->kids)
    {
      for (np = container->kids; np; np = np->next)
	{
	  if (/*space*/)
	    {
	      /* end word in current stream */
	      endword(stream);
	    }
	  else if (/* {( or {{ */)
	    {
	      
	    }
	  else if (/* language switch */)
	    {
	      
	    }
	  addword(np, stream);
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

