#include <string.h>
#include <stdlib.h>
#include "tree.h"
#include "note.h"
#include "gdl.h"
#include "cuneify.h"

int gdl_fragment_ok = 0;

static int lid_base = 0;
static int lid = 0;

extern void note_term(void);

void
gdl_init()
{
  atf_init();
  charsets_init();
  langtag_init();
  if (!use_unicode)
    atf2utf_init();
  tokenize_init();
  tokcheck_init();
  graphemes_init();
  forms_init();
  inline_init();
  note_initialize();
  lid_base = rand();
}

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
  note_term();
}

struct node *
gdl(unsigned char *atf, int frag_ok)
{
  static char l_id_buf[32];
  struct node *res = elem(e_l,NULL,1,LINE);
  int saved_frag_ok = gdl_fragment_ok;
  gdl_fragment_ok = frag_ok;
  sprintf(l_id_buf,"gdl.%08x.%04x",lid_base,lid++);
  tlit_parse_inline(atf,atf+strlen((const char*)atf),res,0,0,
		    (unsigned char*)l_id_buf);
  gdl_fragment_ok = saved_frag_ok;
  return res;
}

void
gdl_string(unsigned char *atf, int frag_ok)
{
  static char l_id_buf[32];
  struct node *res = elem(e_l,NULL,1,LINE);
  int saved_frag_ok = gdl_fragment_ok;
  gdl_fragment_ok = frag_ok;
  sprintf(l_id_buf,"gdl.%08x.%04x",lid_base,lid++);
  /* fprintf(stderr,"gdl_string: %s\n",atf); */
  tlit_parse_inline(atf,atf+strlen((const char*)atf),res,0,0,
		    (unsigned char*)l_id_buf);
  serialize(firstChild(res),0);
  gdl_fragment_ok = saved_frag_ok;
}
