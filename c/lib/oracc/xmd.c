#include <unistd.h>
#include <psd_base.h>
#include <runexpat.h>
#include <fname.h>
#include <string.h>
#include <hash.h>
#include <npool.h>
#include "xmd.h"

static Hash_table *xmd_vals = NULL;
static int in_cat_data;
static struct npool * xmd_pool = NULL;

void
xmd_init(void)
{
  if (xmd_vals)
    {
      hash_free(xmd_vals, NULL);
      if (xmd_pool)
	npool_term(xmd_pool);
    }
  
  xmd_vals = hash_create(1);
  xmd_pool = npool_init();
}

void
xmd_term(void)
{
  if (xmd_vals)
    {
      hash_free(xmd_vals,NULL);
      xmd_vals = NULL;
    }
  if (xmd_pool)
    {
      npool_term(xmd_pool);
      xmd_pool = NULL;
    }
}

static void
xmd_sH(void *userData, const char *name, const char **atts)
{
  if (!in_cat_data)
    {
      if (!strcmp(name,"cat"))
	in_cat_data = 1;
    }
  else if (!strcmp(name,"images"))
    in_cat_data = 0;
  charData_discard();
}

static void
xmd_eH(void *userData, const char *name)
{
  if (in_cat_data) /* breaks if a key can occur more than once */
    {
      if (!strcmp(name,"cat"))
	in_cat_data = 0;
      else if (strcmp(name,"subfield")) /*FIXME: should do something
					  with subfields */
	hash_add(xmd_vals, 
		 (unsigned char*)npool_copy(name, xmd_pool), 
		 (unsigned char *)npool_copy(charData_retrieve(), xmd_pool));
    }
  else if (!strcmp(name,"images"))
    in_cat_data = 1;
}

Hash_table *
xmd_load(const char *project, const char *pq)
{
  const char *fn = expand(project,pq,"xmd");
  const char *fns[2];
  
  fns[0] = fn;
  fns[1] = NULL;
  in_cat_data = 0;
  runexpat(i_list, fns, xmd_sH, xmd_eH);
  return xmd_vals;
}

Hash_table *
l2_xmd_load(const char *project, const char *pq)
{
  const char *fn = NULL;
  const char *fns[2];
  char *tmp = NULL, *nproj, *npq;
  
  if (!project && (strchr(pq,':')))
    {
      tmp = malloc(strlen(pq)+1);
      strcpy(tmp,pq);
      nproj = npq = tmp;
      npq = strchr(npq, ':');
      *npq++ = '\0';
      fn = l2_expand(nproj,npq,"xmd");
      free(tmp);
    }
  else
    fn = l2_expand(project,pq,"xmd");

  fns[0] = fn;
  fns[1] = NULL;
  in_cat_data = 0;
  runexpat(i_list, fns, xmd_sH, xmd_eH);
  return xmd_vals;
}
