#include <stdlib.h>
#include <stdio.h>
#include "gdl.tab.h"

int gdl_lexfld[128];

void
gdl_lexfld_init(void)
{
  gdl_lexfld['@'] = LF_AT;
  gdl_lexfld['^'] = LF_CARET;
  gdl_lexfld['='] = LF_EQUALS;
  gdl_lexfld['#'] = LF_HASH;
  gdl_lexfld['"'] = LF_QUOTE;
  gdl_lexfld['~'] = LF_TILDE;
  gdl_lexfld['|'] = LF_VBAR;
}

const char *
gdl_lexfld_name(const char *lftok)
{
  const char *fld = NULL;
  switch (*lftok)
    {
#if 1
    case '=':
      fld = "!eq";
      break;
    case '#':
      fld = "!sv";
      break;
    case '"':
      fld = "!pr";
      break;
    case '~':
      fld = "!sg";
      break;
    case '|':
      fld = "!sn";
      break;
    case '^':
      fld = "!wp";
      break;
    case '@':
      fld = "!cs"; /* contained sign */
      break;
    default:
      /* can't happen */
      fprintf(stderr, "gdl_lexfld_name: internal error: field '%c' unknown; returning 'default'\n", *lftok);
      fld = "default";
      break;
#else
    case LF_EQUALS: /*'='*/
      fld = "!eq";
      break;
    case LF_HASH:   /*'#'*/
      fld = "!sv";
      break;
    case LF_QUOTE:  /*'"'*/
      fld = "!pr";
      break;
    case LF_TILDE:  /*'~'*/
      fld = "!sg";
      break;
    case LF_VBAR:   /*'|'*/
      fld = "!sn";
      break;
    case LF_CARET:  /*'^'*/
      fld = "!wp";
      break;
    case LF_AT:     /*'@'*/
      fld = "!cs"; /* contained sign */
      break;
#endif
    }
  return fld;
}
