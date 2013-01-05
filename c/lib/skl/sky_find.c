#include <psd_base.h>
#include <hash.h>
#include <skl.h>

#ifndef strdup
extern char*strdup(const char *);
#endif

const char **ids = NULL;
static struct reftab *_sky_find(Hash_table *hp, const char *key);

const char **
sky_last_ids(void)
{
  return ids;
}

const char *
sky_find(Hash_table *hp, const char **keys)
{
  const char **keysp = keys;

  if (ids)
    {
      free(ids);
      ids = NULL;
    }

  while (*keysp)
    {
      struct reftab *rp = _sky_find(hp,*keysp);
      if (!rp)
	return NULL;

      if (rp->type == rt_singleton)
	return rp->c.id;
      if (ids)
	{
	  const char **destid,**nextid;
	  for (destid = nextid = ids; *nextid; ++nextid)
	    {
	      const char *ok = hash_find(rp->c.hash,(unsigned char *)*nextid);
	      if (ok)
		*destid++ = *nextid;
	    }
	  if (destid > ids)
	    *destid = NULL;
	  else
	    return NULL;
	}
      else
	ids = hash_keys(rp->c.hash);
      ++keysp;
    }
  if (ids)
    {
      if (ids[0] && !ids[1])
	return ids[0];
      else
	return (const char *)1;
    }
  else
    return NULL;
}

static struct reftab *
_sky_find(Hash_table *hp, const char *key)
{
  struct reftab *rp;
  if (!hp || !key)
    return NULL;
  rp = hash_find(hp,(unsigned char *)key);
  if (rp)
    {
      if (rp->type == rt_raw)
	{
	  if (strchr(rp->c.id,' '))
	    {
	      char *idp = strdup(rp->c.id);
	      rp->c.hash = hash_create(strlen(idp) / 3);
	      while (idp)
		{
		  char *endp = strchr(idp,' ');
		  if (endp)
		    *endp++ = '\0';
		  hash_add(rp->c.hash,(unsigned char *)idp,(char *)1);
		  idp = endp;
		}
	      rp->type = rt_multi;
	    }
	  else
	    rp->type = rt_singleton;
	}
      return rp;
    }
  else
    return NULL;
}
