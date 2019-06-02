#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "npool.h"
#include "selib.h"
#include "xsystem.h"
#include "v2.h"

static struct npool *v2_pool;
static int v2h = 0;
static int v2l = 0;
static int v2n = 0;
static unsigned char *v2s = NULL;

int
v2_ids(const char *proj, const char *indx)
{
  const char *v2ids = se_file(proj,indx,"v2.ids");
  fprintf (stderr, "checking %s\n", v2ids);
  return !xaccess(v2ids, R_OK, 0);
}

void
v2s_init()
{
  v2_pool = npool_init();
  v2n = 0;
}

void
v2s_term()
{
  npool_term(v2_pool);
  v2n = 0;
}

int
v2s_add(const unsigned char *s)
{
  npool_copy(s,v2_pool);
  return ++v2n;
}


int
v2s_max()
{
  struct pool_block *pbp;
  int len = 0;
  if (!v2_pool)
    return -1;
  for (pbp = v2_pool->base; pbp; pbp = pbp->next)
    {
      const unsigned char *p = pbp->mem;
      while (p < pbp->used)
	{
	  int l = strlen((const char*)p) + 1;
	  if (l > len)
	    len = l;
	  p += l;
	}      
    }
  return len;
}

int
v2_file(const char *dir, const char *perms)
{
  static FILE *f = NULL;
  static char *fn = NULL;
  if (!dir || !*dir)
    {
      if (f)
	{
	  fclose(f);
	  free(fn);
	  f = NULL;
	}
      return -1;
    }
  else
    {
      fn = malloc(strlen(dir) + 2 + strlen("v2.ids"));
      strcpy(fn, dir);
      if (fn[strlen(fn) - 1] != '/')
	strcat(fn, "/");
      strcat(fn, "v2.ids");
      f = fopen(fn, perms);
      return fileno(f);
    }
}

int
v2g_file(const char *dir)
{
  return v2_file(dir, "rb");
}

int
v2s_file(const char *dir)
{
  return v2_file(dir, "wb");
}

int
v2s_save(int fh)
{
  struct pool_block *pbp;
  const char *empty = NULL;
  char lbuf[5];
  int lbuf_len = 0;
  int blocksize = 0;

  if (!v2_pool)
    return -1;

  blocksize = v2s_max();

  if (blocksize > 1 && blocksize < 1024)
    {
      empty = calloc(1, blocksize);
    }
  else
    {
      fprintf(stderr, "bad blocksize in v2s_save: %d\n", blocksize);
      return -1;
    }
  sprintf(lbuf,"%i",blocksize);
  lbuf_len = strlen(lbuf)+1;
  write(fh, lbuf, lbuf_len);
  if (blocksize > lbuf_len)
    write(fh, empty, blocksize-lbuf_len);
  
  for (pbp = v2_pool->base; pbp; pbp = pbp->next)
    {
      const unsigned char *p = pbp->mem;
      while (p < pbp->used)
	{
	  int l = strlen((const char*)p) + 1;
	  write(fh, p, l);
	  if (blocksize > l)
	    write(fh, empty, blocksize-l);
	  p += l;
	}      
    }

  return 0;
}

void
v2g_init(const char *dir)
{
  char lbuf[5]; /* max is 1024 */
  v2h = v2g_file(dir);
  read(v2h, lbuf, 5);
  v2l = atoi(lbuf);
  v2s = malloc(v2l);
}

const char *
v2g_get(unsigned int n)
{
  lseek(v2h, n * v2l, SEEK_SET);
  read(v2h, v2s, v2l);
  return (const char *)v2s;
}

void
v2g_term()
{
  free(v2s);
  v2g_file(NULL);
}

#ifdef MAIN

int
main(int argc, const char **argv)
{
  int len = 0;
  int n = 2;
  int id = 0;

  v2s_init();
  id = v2s_add((const unsigned char *)"one"); printf("added one with ID = %d\n", id);
  id = v2s_add((const unsigned char *)"two"); printf("added two with ID = %d\n", id);
  id = v2s_add((const unsigned char *)"three"); printf("added three with ID = %d\n", id);  
  printf("len = %d\n", len);
  v2s_save(v2s_file("."));
  v2s_file(NULL);
  v2s_term();

  v2g_init(".");
  printf("lookup %d => %s\n", n, v2g_get(n));
  v2g_term();

  return 0;
}

#endif
