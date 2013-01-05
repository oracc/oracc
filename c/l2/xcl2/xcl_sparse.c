#include "xcl.h"

/* Is sparse lemmatization in effect for this text? */
int sparse_lem = 0;

/* Is sparse lemmatization in effect for this field,
   meanign that we are skipping lemmatization? */
int sparse_skipping = 0;

/* space-delimited list of fields that should be 
   lemmatized when in sparse mode */
static char *sparse_lem_fields = NULL;

int
xcl_is_sparse_field(const char *f)
{
  if (f && strlen(f) == 2)
    {
      char field_buf[5];
      field_buf[1] = f[0]; field_buf[2] = f[1];
      field_buf[0] = field_buf[3] = ' ';
      field_buf[4] = '\0';
      return (strstr(sparse_lem_fields,field_buf) ? 1 : 0);
    }
  else
    return 0;
}

void
xcl_set_sparse_fields(const char *f)
{
  if (f && strlen(f))
    {
      sparse_lem = 1;
      if (sparse_lem_fields)
	free(sparse_lem_fields);
      sparse_lem_fields = malloc(strlen(f) + 3);
      sprintf(sparse_lem_fields," %s ", f);
    }
  else if (sparse_lem_fields)
    {
      free(sparse_lem_fields);
      sparse_lem_fields = NULL;
    }
}

void
xcl_set_sparse_skipping(const char *f)
{
  if (f && strlen(f) == 2)
    {
      char field_buf[5];
      field_buf[1] = f[0]; field_buf[2] = f[1];
      field_buf[0] = field_buf[3] = ' ';
      field_buf[4] = '\0';
      sparse_skipping = (strstr(sparse_lem_fields,field_buf) ? 0 : 1);
    }
  else
    sparse_skipping = 0;
}
