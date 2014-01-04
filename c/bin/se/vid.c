#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "pool.h"
#include "npool.h"
#include "loadfile.h"
#include "hash.h"
#include "selib.h"
#include "vid.h"

extern int l2;
extern const char *curr_index, *curr_project;
static char *basebuf = NULL;
static int basebuf_alloced = 0;

extern const char *curr_project, *curr_index;

static void vid_hash_data(struct vid_data *vp);

#undef VID_DEBUG

struct vid_data *
vid_init(void)
{
  struct vid_data *vp;
  pool_init();
  vp = calloc(1,sizeof(struct vid_data));
  vp->vidh = hash_create(1000);
  vp->pool = npool_init();
  return vp;
}

void
vid_term(struct vid_data *vp)
{
  hash_free(vp->vidh,NULL);
  npool_term(vp->pool);
  free(vp->ids);
  free(vp);
}

void
vid_finish(struct vid_data *vp)
{
  const char *vids = se_file(curr_project, curr_index, "vid.dat");
  vid_dump_data(vp,vids);
  vid_term(vp);
}

void
vid_new_id(struct vid_data *vp, const char *xid)
{
  const char *pd = strchr(xid,'.');
  char *vidp;
  int len;

  if (pd)
    len = pd-xid;
  else
    len = strlen(xid);
  
  if (len > (basebuf_alloced-1))
    {
      basebuf_alloced = (len*2) + 1;
      basebuf = realloc(basebuf,basebuf_alloced);
    }
  memcpy(basebuf,xid,len);
  basebuf[len] = '\0';
#ifdef VID_DEBUG
  fprintf(stderr,"vid_new_id: looking for %s to resolve %s\n",basebuf,xid);
#endif
  if (!(vidp = hash_find(vp->vidh,(unsigned char *)basebuf)))
    {
      char vid_buf[8];
      char *xid_pool = (char *)pool_copy((unsigned char *)basebuf);
      sprintf(vid_buf,"v%06d",vp->ids_used);
      hash_add(vp->vidh,pool_copy((unsigned char *)basebuf),pool_copy((unsigned char *)vid_buf));
      vidp = vid_buf;
      if (vp->ids_used == vp->ids_alloced)
	{
	  vp->ids_alloced += 256;
	  vp->ids = realloc(vp->ids,vp->ids_alloced * sizeof(const char *));
	}
      vp->ids[vp->ids_used++] = xid_pool;
      if (strlen(basebuf) >= vp->max_len)
	vp->max_len = strlen(basebuf) + 1;
    }
#ifdef VID_DEBUG
  fprintf(stderr,"vid_new_id: mapped %s to %s\n",basebuf,vidp);
#endif
}

/* Create a new ID based on the Word ID coming in.  WID has the form:
   <TEXT>.<LINE>.<WORD>; the virtualization is applied to the TEXT
   component, and .<LINE>.<WORD> components are added to that.
 */

const char *
vid_map_id(struct vid_data *vp, const char *xid)
{
  const char *pd = strchr(xid,'.');
  char *retbuf, *vidp;

  if (l2)
    {
      static char buf[128];
      const char *underline = strchr(xid,'_');
      if (underline)
	{
	  int len = underline-xid;
	  strncpy(buf,xid,len);
	  buf[len] = '\0';
	  if ((vidp = hash_find(vp->vidh,(unsigned char *)buf)))
	    {
	      if (pd)
		sprintf(buf,"%s%s",vidp,pd);
	      else
		strcpy(buf,vidp);
	      retbuf = buf;
	    }
	  else
	    {
	      fprintf(stderr,"vid_map_id: no map for %s when trying %s\n",xid,buf);
	      retbuf = "v000000";
	    }
	}
      else if (pd)
	{
	  int len = pd-xid;
	  strncpy(buf,xid,len);
	  buf[len] = '\0';
	  if ((vidp = hash_find(vp->vidh,(unsigned char *)buf)))
	    {
	      sprintf(buf,"%s%s",vidp,pd);
	      retbuf = buf;
	    }
	  else
	    {
	      fprintf(stderr,"vid_map_id: no map for %s when trying %s\n",xid,buf);
	      retbuf = "v000000";
	    }
	}
      else
	{
	  vidp = hash_find(vp->vidh,(unsigned char *)xid);
	  if (vidp)
	    return vidp;
	  else
	    {
	      fprintf(stderr,"vid_map_id: no map for %s when trying %s\n",xid,xid);
	      retbuf = "v000000";
	    }
	}
    }
  else
    {
      vidp = hash_find(vp->vidh,
		       (unsigned char *)vp->ids[vp->ids_used-1]);
      if (pd)
	{
	  retbuf = malloc(8+strlen(pd)+1);
	  sprintf(retbuf,"%s%s",vidp,pd);
	}
      else
	{
	  retbuf = malloc(8);
	  strcpy(retbuf,vidp);
	}
    }
  return retbuf;
}

const char *
vid_get_id(struct vid_data *vp, int vid, enum vid_proj which_proj)
{
  char *id = &vp->padded_ids[vid * vp->max_len];
  char *atptr = NULL;
  if ((atptr = strchr(id, '@')))
    {
      if (which_proj == vid_proj_xtf)
	{
	  char *colon = strchr(atptr, ':');
	  return memmove(atptr, colon, strlen(colon));
	}
      else
	{
	  return atptr + 1;
	}
    }
  else
    return id;
}

void
vid_dump_data(struct vid_data *vp, const char *fname)
{
  char *buf;
  FILE *dp = fopen(fname,"wb");
  int i;
  if (!dp)
    {
      fprintf(stderr, "vid: can't open '%s' for write\n",fname);
      return;
    }
  fprintf(dp, "%d%c%d%c", vp->ids_used,0,vp->max_len,0);
  buf = malloc(vp->max_len);
  for (i = 0; i < vp->ids_used; ++i)
    {
      memset(buf,vp->max_len,'\0');
      strcpy(buf,vp->ids[i]);
      fwrite(buf,1,vp->max_len,dp);
    }
  fclose(dp);
}

struct vid_data *
vid_load_data(const char *fname)
{
  struct vid_data *vp = calloc(1,sizeof(struct vid_data));
  size_t fsize;
  char *vpfile = (char*)loadfile((unsigned char *)fname,&fsize);
  if (vpfile)
    {
      char *maxp = vpfile + strlen(vpfile) + 1;
      vp->ids_used = atoi(vpfile);
      vp->max_len = atoi(maxp);
      vp->padded_ids = maxp + strlen(maxp) + 1;
      vp->ids = NULL;
      vp->file = vpfile;
      vp->pool = npool_init();
    }
  if (l2)
    vid_hash_data(vp);
  return vp;
}

static void
vid_hash_data(struct vid_data *vp)
{
  int i = 0;
  char *keybuf = malloc(vp->max_len+1);
  const char *atptr = NULL;
  unsigned char *keyptr;
  vp->vpool = malloc(vp->ids_used * 8);
  vp->ids = malloc(vp->ids_used * sizeof(char*));
  vp->vidh = hash_create(vp->ids_used);
  vp->pool = npool_init();
  for (i = 0; i < vp->ids_used; ++i)
    {
      vp->ids[i] = &vp->vpool[i*8];
      sprintf(vp->ids[i], "v%06d", i);
      keyptr = (unsigned char *)&vp->padded_ids[i*vp->max_len];
      if ((atptr = strchr(keyptr, '@')))
	{
	  /* This is a double-qualified entry: the first project, before the '@',
	     is the one for XTF files; the second, after the '@', is for XMD files.
	     We make entries for both in the hash so that any indexer finds the 
	     common VID.  This means that when the VID is dereferenced the search
	     engine needs to select the correct project when handed a double-qualified
	     QID */
	  const char *colon = NULL;
	  strcpy(keybuf, keyptr);
	  colon = strchr(keybuf, ':');
	  strcpy(keybuf + (atptr-(const char*)keyptr), colon);
	  hash_add(vp->vidh, 
		   npool_copy(keybuf,vp->pool),
		   vp->ids[i]);
	  hash_add(vp->vidh, 
		   atptr+1,
		   vp->ids[i]);
	}
      else
	{
	  hash_add(vp->vidh, 
		   keyptr,
		   vp->ids[i]);
	}
    }
  free(keybuf);
}

void
vid_free(struct vid_data*vp)
{
  hash_free(vp->vidh,NULL);
  free(vp->vpool);
  free(vp->ids);
  free(vp->file);
}
