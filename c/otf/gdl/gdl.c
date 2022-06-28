#include <string.h>
#include <stdlib.h>
#include "tree.h"
#include "note.h"
#include "gdl.h"
#include "cuneify.h"

int gdl_fragment_ok = 0;

static int lid_base = 0;
static int lid = 0;

int gdl_grapheme_sigs = 0;
int gdl_grapheme_sign_names;
List *gdl_sig_deep;
List *gdl_sig_list;
List *gdl_sign_names;

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

unsigned char *
gdl_signnames(unsigned char *atf, int frag_ok)
{
  static char l_id_buf[32];
  struct node *res = elem(e_l,NULL,1,LINE);
  int saved_frag_ok = gdl_fragment_ok;
  const unsigned char **sigbits = NULL;
  unsigned char *buf;
  int i, len;

  gdl_sign_names = list_create(LIST_SINGLE);
  gdl_grapheme_sign_names = 1;
  gdl_fragment_ok = frag_ok;
  sprintf(l_id_buf,"gdl.%08x.%04x",lid_base,lid++);
  tlit_parse_inline(atf,atf+strlen((const char*)atf),res,0,0,
		    (unsigned char*)l_id_buf);
  sigbits = (const unsigned char **)list2array(gdl_sign_names);
  list_free(gdl_sign_names, NULL);
  gdl_sign_names = NULL;
  gdl_grapheme_sign_names = 0;

  for (len = i = 0; sigbits[i]; ++i)
    {
      len += strlen((char*)sigbits[i]);
      ++len;
    }
  buf = malloc(len);
  *buf = '\0';
  for (i = 0; sigbits[i]; ++i)
    {
      if (i)
	strcat((char*)buf, "-");
      strcat((char*)buf, (char*)sigbits[i]);
    }
  /*printf("%s => %s\n", atf, buf);*/
  gdl_fragment_ok = saved_frag_ok;
  return buf;
}

unsigned char *
gdl_sig(unsigned char *atf, int frag_ok, int deep)
{
  static char l_id_buf[32];
  struct node *res = elem(e_l,NULL,1,LINE);
  int saved_frag_ok = gdl_fragment_ok;
  const unsigned char **sigbits = NULL;
  unsigned char *buf, *snbuf = NULL, *parseme;
  int i, len;

  if (deep)
    {
      parseme = snbuf = gdl_signnames(atf, frag_ok);
      /*fprintf(stderr, "%s => %s\n", atf, buf);*/
    }
  else
    parseme = atf;
  
  gdl_grapheme_sigs = 1;
  gdl_sig_list = list_create(LIST_SINGLE);
  gdl_sig_deep = list_create(LIST_SINGLE);

  gdl_fragment_ok = frag_ok;
  sprintf(l_id_buf,"gdl.%08x.%04x",lid_base,lid++);
  /* fprintf(stderr,"gdl_string: %s\n",atf); */
  tlit_parse_inline(parseme,parseme+strlen((const char*)parseme),res,0,0,
		    (unsigned char*)l_id_buf);

  if (deep)
    sigbits = (const unsigned char **)list2array(gdl_sig_deep);
  else
    sigbits = (const unsigned char **)list2array(gdl_sig_list);

  list_free(gdl_sig_deep, NULL);
  list_free(gdl_sig_list, NULL);
  gdl_sig_deep = gdl_sig_list = NULL;
  gdl_grapheme_sigs = 0;
  if (snbuf)
    free(snbuf);

  for (len = i = 0; sigbits[i]; ++i)
    {
      len += strlen((char*)sigbits[i]);
      ++len;
    }
  buf = malloc(len);
  *buf = '\0';
  for (i = 0; sigbits[i]; ++i)
    {
      if (i)
	strcat((char*)buf, ".");
      strcat((char*)buf, (char*)sigbits[i]);
    }
  /*printf("%s => %s\n", atf, buf);*/
  gdl_fragment_ok = saved_frag_ok;
  return buf;
}
