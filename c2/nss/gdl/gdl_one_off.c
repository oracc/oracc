#include <gdl.h>
/* This routine does single-shot processing of a single grapheme;
 * don't use it if you need access to error messages or if you need to
 * process multiple graphemes.
 *
 * Returns NULL on failure, the resultant GDL output on success.
 *
 * The routine unconditionally switches gdl_unicode off so the atf2utf
 * conversion is performed--this is the main reason to use it.
 *
 * Caller must either initialize sll library appropropriately or give
 * non-zero voidsl arg.
 */
char *
gdl_one_off(const char *file, int line, const char *g, int voidsl)
{
  Tree *tp = NULL;
  Mloc loc;
  char *res = NULL;

  gdl_flex_debug = gdldebug = 0;

  if (voidsl)
    gvl_setup(NULL,NULL);
  gdlparse_init();
  loc.file = file;
  loc.line = line;
  mesg_init();
  gdl_unicode = 0;
  char *gs = strdup(g);
  tp = gdlparse_string(&loc, gs);
  free(gs);
  mesg_term();
  if (tp && tp->root && tp->root->kids)
    res = strdup(tp->root->kids->text);
  gdlparse_term();
  tree_term();
  if (voidsl)
    gvl_wrapup(NULL);
  return res;
}
