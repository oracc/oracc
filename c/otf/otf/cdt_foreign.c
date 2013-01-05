#include "cdt.h"
#include "cdt_foreign.h"

extern struct cdt_foreign cdt_atf;
extern struct cdt_foreign cdt_database;

struct cdt_foreign *
cdt_foreign_get_handlers(int code)
{
  switch ((enum cdt_codes)code)
    {
    case foreign_catalog:
    case foreign_bibliography:
      return &cdt_database;
      break;
    case foreign_atf:
      return &cdt_atf;
      break;
    default:
      return NULL;
      break;
    }
}
