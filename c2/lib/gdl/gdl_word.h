#ifndef GDL_WORD_H_
#define GDL_WORD_H_

/* GDL Words in the Tree
 *
 * Anything between spaces is a word or nonword (e.g., a
 * comment). Nodes for delims and parens are included in a parent word
 * node--if there is no text that can be formatted in the word node it
 * is renamed as a nonword.
 *
 * When the tree is built all words have at least one segment; when
 * serialized words with only one segment may have the segment
 * container omitted depending on the output mode.
 */ 

enum wordtype_e { GW_UNIT , GW_SEG_INITIAL, GW_SEG_MEDIAL, GW_SEG_FINAL };

/* For a word node, the void *user is a struct gdlword */
struct gdlword
{
  enum wordtype_e wordtype;
  short lang;
  char script;
  char stream;
  unsigned char *form;  /* canonicalized/aliased form */
  unsigned char *oform; /* original form */
  struct node *prevseg;
  struct node *nextseg;
};

#endif/*GDL_WORD_H_*/
