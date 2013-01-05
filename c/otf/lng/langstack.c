#include "lang.h"

static List *stack = NULL;

struct lang_context *
lang_push(struct lang_context *lp)
{
  if (lp)
    {
      if (!stack)
	stack = list_create(LIST_FIFO);
      list_push(stack, lp);
      return lp;
    }
  /* This can only be happening during error recovery
     so we don't need to emit additional warnings */
  else if (text_lang)
    return text_lang;
  else
    return global_lang;
}

struct lang_context *
lang_pop(void)
{
  struct lang_context *lp = NULL;
  if (!stack)
    return NULL;
  lp = list_pop(stack);
  if (!list_len(stack))
    {
      list_free(stack, NULL);
      stack = NULL;
    }
  if (lp)
    return lp;
  /* This can only be happening during error recovery
     so we don't need to emit additional warnings */
  else if (text_lang)
    return text_lang;
  else
    return global_lang;
}

void
lang_reset(void)
{
  list_free(stack, NULL);
  stack = NULL;
}
