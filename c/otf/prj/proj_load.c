#include "hash.h"
#include "npool.h"
#include "xpd2.h"
#include "proj_context.h"
#include "run.h"

extern int saa_mode;

void
proj_init(struct run_context *runp, const char *project)
{
  struct proj_context *p = hash_find(runp->known_projects, (unsigned char *)project);
  if (!p)
    {
      p = calloc(1,sizeof(struct proj_context));
      p->name = (char *)npool_copy((unsigned char *)project, runp->pool);
      hash_add(runp->known_projects,
	       npool_copy((unsigned char *)project,runp->pool),
	       p);
      p->xpd = xpd_init(project,runp->pool);
      p->owner = runp;
      if (xpd_option(p->xpd,"atf-saa-mode"))
	saa_mode = xpd_option_int(p->xpd,"atf-saa-mode");
      /*set_project(p, project);*/
    }
  runp->proj = p;
}

void
proj_term(struct proj_context *proj)
{
  xpd_term(proj->xpd);
  free(proj);
}
