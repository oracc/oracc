#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "resolver.h"

/* This module remaps XMD files; ATF files are remapped in p3-pager since the time when metadata and atf
   started being proxied separately */

struct pqx_info
{
  char *orig;
  char *replace;
  char *xmd;
  int xmd_ok;
};

static int map_one_pqx(const char *p, struct pqx_info **r);
static void map_remaps(struct pqx_info **r, int rlen);
static char *map_needs_remap(const char *pqx);

static int
map_one_pqx(const char *p, struct pqx_info **r)
{
  int i;
  const char *pqx = p + strlen(p);
  for (i = 0; r[i]; ++i)
    {
      const char *t = r[i]->orig + strlen(r[i]->orig);
      if (pqx[-1] == t[-1]
	  && pqx[-2] == t[-2]
	  && pqx[-3] == t[-3]
	  && pqx[-4] == t[-4]
	  && pqx[-5] == t[-5]
	  && pqx[-6] == t[-6]
	  && pqx[-7] == t[-7]
	  )
	{
	  r[i]->replace = malloc(strlen(p)+1);
	  strcpy(r[i]->replace, p);
	  return i;
	}
    }
  return -1;
}

static void
map_remaps(struct pqx_info **r, int rlen)
{
#define CAT_LINE_MAX 128
  char *pqids_lst = malloc(strlen("/home/oracc/pub//cat/pqids.lst")
				 + strlen(project) + 1);
  char buf[CAT_LINE_MAX];
  (void)sprintf(pqids_lst, "/home/oracc/pub/%s/cat/pqids.lst", project);
  if (!access(pqids_lst, R_OK))
    {
      FILE *pqids = NULL;
      if ((pqids =  fopen(pqids_lst, "r")))
	{
	  while ((NULL != fgets(buf, CAT_LINE_MAX, pqids)))
	    {
	      int i = -1;
	      char *pqx = buf + strlen(buf);
	      if (pqx[-1] == '\n')
		pqx[-1] = '\0';
	      i = map_one_pqx(buf, r);
	      if (i >= 0)
		{
		  if (rlen > 1)
		    {
		      while (i < rlen)
			{
			  r[i] = r[i+1];
			  ++i;
			}
		      r[--rlen] = NULL;
		    }
		  else
		    {
		      r[0] = NULL;
		      break;
		    }
		}
	    }
	  fclose(pqids);
	  free(pqids_lst);
	}
    }
}

static char *
map_needs_remap(const char *pqx)
{
  char *tmp = malloc(strlen("/home/oracc/bld//XXXX/XXXXXXX/XXXXXXX.xmd")
		     + strlen(project) + 1);
  sprintf(tmp, "/home/oracc/bld/%s/%s", project, pqx);
  tmp[strlen(tmp)-3] = '\0';
  sprintf(tmp+strlen(tmp), "/%s/%s.xmd", pqx, pqx);
  if (access(tmp, R_OK))
    {
      free(tmp);
      return NULL;
    }
  return tmp;
}

const char *
map_PQX(const char *text, int count)
{
  struct pqx_info *pqx = calloc(count, sizeof(struct pqx_info));
  struct pqx_info **remaps = malloc(count * sizeof(struct pqx_info *));
  char *curr = NULL, *end = NULL, *replace = NULL;
  int i = 0, r = 0, newbufsize = 0;
  curr = malloc(strlen(text)+1);
  strcpy(curr, text);
  for (; i < count; ++i)
    {
      if (!(end = strchr(curr, ',')))
	end = curr + strlen(curr);
      pqx[i].orig = curr;
      if (*end)
	{
	  *end = '\0';
	  curr = end + 1;
	}
      else
	curr = end;
      if (project && !(pqx[i].xmd = map_needs_remap(pqx[i].orig)))
	remaps[r++] = &pqx[i];
    }
  if (r > 0)
    {
      remaps[r] = NULL;
      map_remaps(remaps, r);
      if (remaps[0])
	{
	  /* we didn't find all the replacements */
	  fprintf(stderr, "%s not found\n", text);
	  do404();
	  return NULL;
	}
      else
	{
	  for (i = 0; i < count; ++i)
	    newbufsize += strlen(pqx[i].replace ? pqx[i].replace : pqx[i].orig);
	  newbufsize += (count + 1);
	  replace = malloc(newbufsize);
	  *replace = 0;
	  for (i = 0; i < count; ++i)
	    {
	      strcat(replace, pqx[i].replace ? pqx[i].replace : pqx[i].orig);
	      if ((i+1) < count)
		strcat(replace, ",");
	    }
	  fprintf(stderr, "%s mapped to %s\n", text, replace);
	  return replace;
	}
    }
  else
    return text;
}
