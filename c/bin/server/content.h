#ifndef CONTENT_H_
#define CONTENT_H_
struct content_opts
{
  const char *chunk_id; /* extract the chunk with the given xml:id */
  const char *frag_id;  /* set the onload frag to the given xml:id */
  const char *hilite_id;/* add 'select' to @class on the element with given xml:id */
  const char *title;    /* title for HTML header */
  int echo;		/* echo input to output */
  int ga;		/* output google analytics header */
  int html;		/* input is in HTML: echo the header as well as the chunk */
  int sigs;		/* edit pop1sig to have project */
  int unwrap;		/* input is HTML page: return only the content of BODY */
  int wrap;		/* input is an HTML fragment: add a standard wrapper to it */
};

struct content_opts *content_new_options(void);
void content(struct content_opts *opts, const char *file);

#endif/*CONTENT_H_*/
