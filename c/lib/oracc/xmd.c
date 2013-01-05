#include <unistd.h>
#include <psd_base.h>
#include <runexpat.h>
#include <fname.h>
#include <string.h>
#include <hash.h>
#include <list.h>
#include "xmd.h"

static Hash_table *xmd_vals = NULL;
/*static Hash_table *xmd_keys = NULL; */
static int in_cat_data;
static char *xmd_pool = NULL;
static int xmd_pool_used = 0;
static int xmd_pool_alloced = 0;
static List *xmd_pool_blocks = NULL;
static List_node *xmd_pool_rover;
static void xmd_pool_reset(void);

void
xmd_init(void)
{
  if (xmd_vals)
    {
      hash_free(xmd_vals, NULL);
      if (xmd_pool_blocks)
	xmd_pool_reset();
    }
  
  xmd_vals = hash_create(1);
}

void
xmd_term(void)
{
  if (xmd_vals)
    {
      hash_free(xmd_vals,NULL);
      xmd_vals = NULL;
    }
  list_free(xmd_pool_blocks, list_xfree);
  xmd_pool_blocks = NULL;
  xmd_pool_rover = NULL;
  xmd_pool_alloced = xmd_pool_used = 0;
}

static void
xmd_pool_reset(void)
{
  xmd_pool_rover = xmd_pool_blocks->first;
  xmd_pool = xmd_pool_rover->data;
  xmd_pool_used = 0;
}

static const char *
xmd_pool_copy(const char *s)
{
  const char *ret;
  if (xmd_pool_used + strlen(s) + 1 > xmd_pool_alloced)
    {
      if (xmd_pool_blocks == NULL)
	{
	  xmd_pool_blocks = list_create(LIST_SINGLE);
	  xmd_pool = calloc(8192,1);
	  xmd_pool_alloced = 8192;
	  list_add(xmd_pool_blocks, xmd_pool);
	  xmd_pool_rover = xmd_pool_blocks->first;
	}
      else
	{
	  if (xmd_pool_rover->next == NULL)
	    {
	      xmd_pool = calloc(8192,1);
	      list_add(xmd_pool_blocks, xmd_pool);
	      xmd_pool_rover = xmd_pool_blocks->last;
	    }
	  else
	    {
	      xmd_pool_rover = xmd_pool_rover->next;
	      xmd_pool = xmd_pool_rover->data;
	    }	      
	}
      xmd_pool_used = 0;
    }
  ret = strcpy(xmd_pool+xmd_pool_used, s);
  xmd_pool_used += strlen(s)+1;
  return ret;
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
		 (unsigned char*)xmd_pool_copy(name), 
		 (unsigned char *)xmd_pool_copy(charData_retrieve()));
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
