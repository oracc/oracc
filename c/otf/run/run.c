#include "proj_context.h"
#include "lang.h"
#include "run.h"

struct run_context *
run_init(void)
{
  struct run_context *ret = calloc(1, sizeof(struct run_context));
  ret->known_projects = hash_create(1);
  ret->pool = npool_init();
  return ret;
}

void
run_term(struct run_context *rp)
{
  npool_term(rp->pool);
  hash_free(rp->known_projects, (hash_free_func*)proj_term);
  free(rp);
}
