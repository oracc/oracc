#include <string.h>
#include <stdlib.h>
#include "tree.h"
#include "gdl.h"
#include "warning.h"

int gdl_fragment_ok = 0;
static int lid_base = -1;
static int lid = 0;

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
  if (lid_base < 0)
    lid_base = rand();
  gdl_fragment_ok = frag_ok;
  sprintf(l_id_buf,"gdl.%08x.%04x",lid_base,lid++);
  /* fprintf(stderr,"gdl_string: %s\n",atf); */
  tlit_parse_inline(atf,atf+strlen((const char*)atf),res,0,0,
                    (unsigned char*)l_id_buf);
  serialize(firstChild(res),0);
  gdl_fragment_ok = saved_frag_ok;
}
