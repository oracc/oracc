#ifndef _CDT_FOREIGN
#define _CDT_FOREIGN
#include <stdio.h>

struct cdt_node;

typedef void* (*cdt_foreign_init)(const char *);
typedef void (*cdt_foreign_term)(void*);
typedef void (*cdt_foreign_reader)(struct cdt_node *);
typedef void (*cdt_foreign_writer)(FILE*,struct cdt_node *);

struct cdt_foreign
{
  cdt_foreign_init init;
  cdt_foreign_term term;
  cdt_foreign_reader reader;
  cdt_foreign_writer writer;
};

enum foreign_data_ops { op_read , op_write };

extern struct cdt_foreign *cdt_foreign_get_handlers(int code);

#endif/*_CDT_FOREIGN*/
