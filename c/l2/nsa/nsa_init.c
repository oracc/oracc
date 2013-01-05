#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "nsa.h"
#include "runexpat.h"

static const char *curr_comhead_sys;
static char *last_u;
struct nsa_system *sex_system;
static struct nsa_context *context;
static struct nsa_system *curr_system;
#define ucc(p) ((const unsigned char *)p)

static void sH_index_step(const char *step, struct nsa_step *curr_step,int overt_star);
static unsigned const char **split_step(const char *step,int overt_star);

static void
sH(void *userData, const char *name, const char **atts)
{
  charData_discard();
  if (!strcmp(name,"system"))
    {
      const char *det;
      const char *mev;
      char *endll;
      mev = findAttr(atts,"mev");
      curr_system = new_system();
      curr_system->n = (const char *)npool_copy((unsigned char *)findAttr(atts,"n"),context->cpool);
      curr_system->mev = new_count();
      curr_system->mev->num = strtoll(mev,&endll,10);
      curr_system->mev->den = (int)strtol(endll+1,&endll,10);
      curr_system->meu = (char *)npool_copy((unsigned char *)endll,
					    context->cpool);
      hash_add(context->systems,
	       (const unsigned char *)curr_system->n,curr_system);
      if ((det = findAttr(atts,"det")))
	hash_add(context->sysdets,
		 npool_copy((unsigned char*)det,context->cpool),
		 curr_system);
    }
  else if (!strcmp(name,"step"))
    {
      struct nsa_step *curr_step = new_step();
      const char *overt = findAttr(atts,"overt");
      const char *aev = findAttr(atts,"aev");
      const char *sex = findAttr(atts,"sex");
      char *slash;
      int overt_star;

      curr_step->ecf = (const char *)npool_copy((unsigned char *)findAttr(atts,"ecf"),context->cpool);
      curr_step->etr = (const char *)npool_copy((unsigned char *)findAttr(atts,"etr"),context->cpool);
      curr_step->atf = (const char *)npool_copy((unsigned char *)findAttr(atts,"atf"),context->cpool);
      curr_step->utf = (const char *)npool_copy((unsigned char *)findAttr(atts,"utf"),context->cpool);
      if (sex && *sex == 'a') /* horrible cheat! */
	curr_step->sex = nsa_sex_asz_axis;
      curr_step->sys = curr_system;
      list_add(curr_system->steps,curr_step);
      if (aev && *aev)
	{
	  curr_step->aev = new_count();
	  slash = strchr(aev,'/');
	  if (!slash)
	    {
	      fprintf(stderr,"nsa: system %s/step %s: bad AEV (should be n/1; actual='%s')\n",curr_system->n,curr_step->atf,aev);
	      curr_step->aev->num = 0;
	      curr_step->aev->den = 1;
	    }
	  else
	    {
	      curr_step->aev->num = strtoll(aev,NULL,10);
	      curr_step->aev->den = (int)strtol(slash+1,NULL,10);
	    }
	}
      curr_step->node = curr_system->steps->last;
      curr_step->sys = curr_system;
      if (!overt || !*overt || strcmp(overt,"no"))
	overt_star = 0;
      else
	overt_star = 1;
      /*	  sH_index_step(curr_step->ecf, curr_step); */
      /* sH_index_step(curr_step->etr, curr_step); */
      sH_index_step(curr_step->utf, curr_step, overt_star);
      if (curr_step->atf && strcmp(curr_step->utf,curr_step->atf))
	sH_index_step(curr_step->atf, curr_step, overt_star);
    }
  else if (!strcmp(name,"ok-com-heads"))
    {
      curr_comhead_sys = (const char *)npool_copy((unsigned char *)findAttr(atts,"sys"),context->cpool);
    }
  else if (!strcmp(name,"igi"))
    last_u = (char *)npool_copy((unsigned char *)findAttr(atts,"u"),context->cpool);
}
static void
eH(void *userData, const char *name)
{
  static int defined = 1;
  if (!strcmp(name,"key"))
    {
      const unsigned char *k = (const unsigned char *)charData_retrieve();
      if (!hash_find(context->syskeys,k))
	hash_add(context->syskeys,npool_copy(k,context->cpool),curr_system);
    }
  else if (!strcmp(name,"det"))
    {
      const unsigned char *d = (const unsigned char *)charData_retrieve();
      if (!hash_find(context->sysdets,d))
	hash_add(context->sysdets,npool_copy(d,context->cpool),curr_system);
    }
  else if (!strcmp(name,"och"))
    {
      const unsigned char *d = (const unsigned char *)charData_retrieve();
      List *l;
      if (!(l = hash_find(context->comheads,d)))
	{
	  l = list_create(LIST_SINGLE);
	  list_add(l,(void*)curr_comhead_sys);
	  hash_add(context->comheads,npool_copy(d,context->cpool),(void*)l);
	}
      else
	{
	  list_add(l,(void*)curr_comhead_sys);
	}
    }
  else if (!strcmp(name,"gal2"))
    {
      const unsigned char *d = (const unsigned char *)charData_retrieve();
      if (!hash_find(context->gal2_tokens,d))
	hash_add(context->gal2_tokens,npool_copy(d,context->cpool),&defined);
    }
  else if (!strcmp(name,"igi"))
    {
      const unsigned char *d = (const unsigned char *)charData_retrieve();
      if (!hash_find(context->igigal_keys,d))
	hash_add(context->igigal_keys,npool_copy(d,context->cpool),last_u);
    }
  else if (!strcmp(name,"la2"))
    {
      const unsigned char *d = (const unsigned char *)charData_retrieve();
      if (!hash_find(context->la2_tokens,d))
	hash_add(context->la2_tokens,npool_copy(d,context->cpool),&defined);
    }
  else if (!strcmp(name,"suffix"))
    {
      const unsigned char *d = (const unsigned char *)charData_retrieve();
      if (!hash_find(context->morph_suffixes,d))
	hash_add(context->morph_suffixes,npool_copy(d,context->cpool),&defined);
    }
  else if (!strcmp(name,"sexfrac"))
    {
      const unsigned char *d = (const unsigned char *)charData_retrieve();
      if (!hash_find(context->sexfracs,d))
	hash_add(context->sexfracs,npool_copy(d,context->cpool),&defined);
    }
}

struct nsa_context *
nsa_init(const char *file)
{
  const char *fname[2];
  fname[0] = file;
  fname[1] = NULL;
  if (!access(fname[0],R_OK))
    {
      context = new_context();
      runexpat(i_list, fname, sH, eH);
      sex_system = new_system();
      sex_system->n = "sexnum";
      free(sex_system->steps);
      sex_system->steps = NULL;
      return context;
    }
  return NULL;
}

void
nsa_term(void)
{
  nsa_del_system(sex_system);
}

void
sH_index_step(const char *step, struct nsa_step *curr_step, int overt_star)
{
  const unsigned char **step_bits = split_step(step, overt_star);
  Hash_table *curr_hash = context->step_index;
  int i;
  for (i = 0; step_bits[i]; ++i)
    {
      struct nsa_hash_data *d;
      if ((d = hash_find(curr_hash,step_bits[i])))
	{
	  if (step_bits[i+1]) /* non-terminal */
	    {
	      struct nsa_hash_data *newd = new_hash_data();
	      newd->cands = NULL;
	      newd->continuations = NULL;
	      if (!d->continuations)
		d->continuations = hash_create(1);
	      hash_add(d->continuations,step_bits[i],newd);
	      curr_hash = d->continuations;
	    }
	  else  /* terminal */
	    {
	      if (!d->cands)
		d->cands = list_create(LIST_DOUBLE);
	      list_add(d->cands, curr_step);
	    }
	}
      else
	{
	  d = new_hash_data();
	  hash_add(curr_hash,step_bits[i],d);
	  if (step_bits[i+1])
	    {
	      d->cands = NULL;
	      d->continuations = hash_create(1);
	      curr_hash = d->continuations;
	    }
	  else
	    {
	      d->continuations = NULL;
	      d->cands = list_create(LIST_DOUBLE);
	      list_add(d->cands, curr_step);
	    }
	}
    }
  free(step_bits);
}

static unsigned const char **
split_step(const char *step, int overt_star)
{
  unsigned const char **ret;
  unsigned char *tmp = npool_copy((const unsigned char *)step,context->cpool);
  if (strchr((const char *)tmp, ' '))
    {
      int n = 1;
      unsigned char *tmp2 = tmp;
      while (*tmp2)
	{
	  if (' ' == *tmp2)
	    {
	      ++n;
	      while (' ' == *tmp2)
		++tmp2;
	    }
	  else
	    ++tmp2;
	}
      ret = malloc((n+1) * sizeof(unsigned const char *));
      for (n = 0, tmp2 = tmp; *tmp2; ++tmp2, ++n)
	{
	  char buf[1024];
	  unsigned char *s = tmp2, save;
	  while (*tmp2 && ' ' != *tmp2)
	    ++tmp2;
	  save = *tmp2;
	  *tmp2 = '\0';
	  sprintf(buf,"%s%s",overt_star ? "*" : "", s);
	  ret[n] = npool_copy((unsigned char *)buf,context->cpool);
	  if (save)
	    {
	      while (' ' == tmp2[1])
		++tmp2;
	    }
	  else
	    {
	      ++n;
	      break;
	    }
	}
      ret[n] = NULL;
    }
  else
    {
      char buf[1024];
      ret = malloc(2*sizeof(unsigned const char *));
      sprintf(buf,"%s%s", overt_star ? "*" : "", tmp);
      ret[0] = npool_copy((unsigned char *)buf,context->cpool);
      ret[1] = NULL;
    }
  return ret;
}
