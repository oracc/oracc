#include "warning.h"
#include "memblock.h"
#include "npool.h"
#include "lang.h"
#include "sigs.h"
#include "f2.h"
#include "ilem_form.h"
#include "xcl.h"
#include "words.h"

struct mb *ifp_mem = NULL;
struct mb *ifp_struct_mem = NULL;
static void set_cof_ptrs(struct xcl_l *l, struct xcl_l *tail, int nth);

void
sigs_cof_init(void)
{
  ifp_struct_mem = mb_init(sizeof(struct ilem_form), 1);
  ifp_mem = mb_init(sizeof(struct ilem_form *), 1);
}

void
sigs_cof_term(void)
{
  if (ifp_struct_mem)
    {
      mb_free(ifp_struct_mem);
      ifp_struct_mem = NULL;
    }
  if (ifp_mem)
    {
      mb_free(ifp_mem);
      ifp_mem = NULL;
    }
}

/* Each of l's finds is a COF-head with its own chain of tails.
 * Iterate over the COF-tails listed in l and append each of the
 * relevant tails from l's finds' COF-heads.
 */
void
sigs_cof_finds(struct xcl_l *l)
{
  struct xcl_l *tail_l = NULL;
  int nth = 0;
  for (tail_l = list_first(l->cof_tails); tail_l; tail_l = list_next(l->cof_tails))
    set_cof_ptrs(l, tail_l, nth++);
}

static void
set_cof_ptrs(struct xcl_l *l, struct xcl_l *tail, int nth)
{
  int i;
  struct ilem_form *ifps;
  ifps = mb_new_array(ifp_struct_mem, (1+l->f->fcount));
  tail->f->fcount = l->f->fcount;
#if 1
  tail->f->finds = malloc((1+l->f->fcount) * sizeof(struct ilem_form *));
#else
  tail->f->finds = mb_new_array(ifp_mem, l->f->fcount);
#endif
  for (i = 0; i < l->f->fcount; ++i)
    {
      tail->f->sp = l->f->sp;
      tail->f->look = l->f->look;
      tail->f->finds[i] = &ifps[i];
      if (l->f->finds && l->f->finds[i])
	{
	  *tail->f->finds[i] = *(l->f->finds[i]);
	  if (l->f->finds[i]->f2.parts && l->f->finds[i]->f2.parts[nth])
	    {
	      tail->f->finds[i]->f2 = *(l->f->finds[i]->f2.parts[nth]);
	      /* reset the head's parts[nth] to point to the
		 tail's l->f->f2 structure so flag-setting 
		 propagates correctly */
	      l->f->finds[i]->f2.parts[nth] = &tail->f->finds[i]->f2;
	    }
	}
    }
}
