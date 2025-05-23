#include <stdio.h>
#include <unistd.h>
#include <gdbm.h>
#include <psd_base.h>
#include <hash.h>
#include "sortinfo.h"
#include "pg.h"

#undef MAX_PATH
#define MAX_PATH 1024

#ifndef strdup
extern char*strdup(const char *);
#endif

#undef cc
#define cc(x) ((const char *)x)
#define ucc(x) ((const unsigned char *)x)

extern int fragment;

static GDBM_FILE linkmap;
int use_linkmap = 0;

extern const char *project;

void
ptoq_init(const char *fname)
{
  linkmap = gdbm_open((char*)fname,512,GDBM_READER, 0640,NULL);
  use_linkmap = linkmap != NULL;
}

void
ptoq_term()
{
  gdbm_close(linkmap);
}

const char *
q_of(const char *p)
{
  char *s = NULL;
  datum d,k;
  k.dptr = (char*)p;
  k.dsize = strlen(p);
  d = gdbm_fetch(linkmap,k);
  if (d.dsize)
    {
      s = malloc(d.dsize+1);
      strncpy(s,d.dptr,d.dsize);
      s[d.dsize] = '\0';
      return s;
    }
  else
    return NULL;
}

struct item *
prune_p_with_q(struct item*items, int *nitems, int with_word_id)
{
  struct item *retitems;
  int retnitems = 0;
  char fname[MAX_PATH];

  if (!project)
    return items;

  sprintf(fname, "/home/oracc/www/%s/linkmap.gdbm", project);

  if (access(fname,R_OK))
    return items;

  ptoq_init(fname);
  if (use_linkmap)
    {
      int i;
      Hash_table*qtab = hash_create(*nitems);
      /* first map the Q-results by line */
      for (i = 0; i < *nitems; ++i)
	{
	  if (*items[i].s == 'Q')
	    {
	      char *qlid = strdup(cc(items[i].s));
	      char *ldot = strchr(qlid,'.');
	      char *wdot;
	      if (ldot && (wdot = strchr(ldot+1,'.')))
		*wdot = '\0';
	      if (!hash_find(qtab,ucc(qlid)))
		hash_add(qtab, ucc(qlid), (unsigned char *)items[i].s);
	    }
	}
      /* now for each P-results:
	 if there is a corresponding Q-line:
	    if the Q-line is in the resultset, drop the P
	    else add the P's Q-line to the resultset instead of the P
	         (set the word-id part to 1 just so it fits the rest of
		 the results if with_word_id is true)
      */
      retitems = malloc(sizeof(struct item) * *nitems);
      for (i = 0; i < *nitems; ++i)
	{
	  if (*items[i].s == 'P')
	    {
	      static char pbuf[32], *p;
	      const char *ldot = strchr(cc(items[i].s),'.');
	      const char *wdot = strchr(ldot+1,'.');
	      const char *qlid = NULL;

	      if (!wdot)
		p = (char*)items[i].s;
	      else
		{
		  int n = wdot - cc(items[i].s);
		  strncpy(pbuf, cc(items[i].s), n);
		  pbuf[n] = '\0';
		  p = pbuf;
		}
	      
	      qlid = q_of(p);
	      if (qlid)
		{
		  if (!hash_find(qtab,ucc(qlid)))
		    {
		      char *dot;
		      strcpy(pbuf,qlid);
		      strcat(pbuf,".1"); /* don't know how to do better than this */
		      items[i].s = (unsigned char *)strdup(pbuf);
		      items[i].pq = (unsigned char *)strdup(pbuf);
		      if ((dot = strchr((const char *)items[i].pq,'.')))
			{
			  *dot++ = '\0';
			  items[i].lkey = atoi(dot);
			}
		      else
			items[i].lkey = 0;			  
		      retitems[retnitems++] = items[i];
		    }
		  free((char*)qlid);
		}
	      else
		retitems[retnitems++] = items[i];
	    }
	  else
	    retitems[retnitems++] = items[i];
	}
      ptoq_term();
      retitems = realloc(retitems,sizeof(struct item) * retnitems);
      *nitems = retnitems;
      return retitems;
    }
  else
    return items;
}
