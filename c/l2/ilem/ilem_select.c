#include <stdlib.h>
#include <stdio.h>
#include "xcl.h"
#include "ilem.h"

static struct ilem_form **forms_tmp = NULL;
int forms_tmp_alloced = 0;

struct ilem_form **
ilem_select(struct ilem_form **fpp, 
	   int fpp_len,
	   void *user_data,
	   void * (*init)(void*data), 
	   int (*test)(struct ilem_form *fp, void *user_data, void *user_setup),
	   void (*term)(void *data, void *setup),
	   int *nformsp)
{
  int i, nforms;
  void *user_setup;

  if (fpp_len >= forms_tmp_alloced)
    {
      forms_tmp_alloced = fpp_len * 10;
      forms_tmp = realloc(forms_tmp, forms_tmp_alloced * sizeof(struct ilem_form *));
    }

  if (init)
    user_setup = init(user_data);
  
  for (i = nforms = 0; i < fpp_len; ++i)
    {
      if (!test(fpp[i],user_data,user_setup))
	forms_tmp[nforms++] = fpp[i];
    }

  /* do this before calling term() in case term() does something with fpp */
  forms_tmp[nforms] = NULL;
  *nformsp = nforms;

  if (term)
    term(user_data,user_setup);

  return forms_tmp;
}

void
ilem_select_clear(void)
{
  if (forms_tmp_alloced)
    {
      free(forms_tmp);
      forms_tmp = NULL;
      forms_tmp_alloced = 0;
    }
}
