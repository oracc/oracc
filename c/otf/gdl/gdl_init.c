#include <string.h>
#include <stdlib.h>
#include "tree.h"

extern void atf_init(void);
extern void charsets_init(void);
extern void langtag_init(void);
extern void tokenize_init(void);
extern void tokcheck_init(void);
extern void graphemes_init(void);
extern void forms_init(void);
extern void inline_init(void);
extern void note_initialize(void);

void
c1_gdl_init(void)
{
  atf_init();
  charsets_init();
  langtag_init();
  tokenize_init();
  tokcheck_init();
  graphemes_init();
  forms_init();
  inline_init();
  note_initialize();
}
#if 0
void
gdl_term()
{
  atf_term();
  charsets_term();
  langtag_term();
  texttag_term();
  lang_term();
  forms_term();
  tokenize_term();
  graphemes_term();
}
#endif
