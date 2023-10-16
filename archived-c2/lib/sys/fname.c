#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype128.h>
#include "oraccsys.h"

#define EXTSEP_STRING "."
#define DIRSEP_CHAR   '/'
#define DIRSEP_STRING "/"
#define MAX_BASENAME_ 1024
#define MAX_PATH_     2048

const char *
oracc_builds (void)
{
  return "/Users/stinney/orc";
}

const char *
oracc_home (void)
{
  static char *cached_home = NULL;
  if (NULL == cached_home)
    {
      cached_home = getenv ("ORACC");
      if (NULL == cached_home)
	cached_home = "/Users/stinney/orc";
    }
  return cached_home;
}

/* this should really be called oracc_home but that function
   returns the ORACC env var */
const char *
oracc_project_home (void)
{
  static char *cached_home = NULL;
  if (NULL == cached_home)
    {
      cached_home = getenv ("ORACC_HOME");
      if (NULL == cached_home)
	cached_home = "/Users/stinney/oracc-projects";
    }
  return cached_home;
}

const char *
oracc_var (void)
{
  static char *cached_var = NULL;
  if (NULL == cached_var)
    {
      cached_var = getenv ("ORACC");
      if (NULL == cached_var)
	cached_var = "/Users/stinney/orc";
    }
  return cached_var;
}

char *
new_ext (char *fn, const char *fext, int force)
{
  char *tmp;
  size_t len;

  if (NULL == fn || NULL == fext)
    return fn;

  if (!strcmp (fn, "-"))
    fn = strdup("stdin");

  /* does fext wrongly begin with a period? */
  if ('.' == *fext)
    ++fext;
  tmp = find_ext (fn);
  if (*tmp)
    {
      if (FALSE == force)
        return fn;
      else
        *(fn + (tmp - fn)) = '\0';
    }
  len = strlen(fn);
  if (*fext)
    {
      fn = realloc(fn, len + strlen(EXTSEP_STRING) + strlen(fext) + 1);
      strcat (fn, EXTSEP_STRING);
      strcat (fn, fext);
    }
  else
    {
      fn = realloc(fn, len + 1);
      fn[len] = '\0';
    }
  return fn;
}

char *
new_ext_no_alloc(char *oldfile,const char *ext)
{
  char *p;
  p = oldfile + strlen(oldfile);
  while (p > oldfile && p[-1] != '.')
    --p;
  if ((p-oldfile) > 1 && isalnum(p[-2]))
    strcpy(p-1,ext);
  else
    strcat(p,ext);
  return oldfile;
}

char *
find_ext(char *fn)
{
  char *tmp = strrchr (fn, '.');
  if (NULL == tmp || tmp == fn || *(tmp + 1) == DIRSEP_CHAR || *(tmp - 1) == '.')
    return fn + strlen (fn);
  else
    return tmp;
}

char *
new_path (char *fn, const char *path, int force)
{
  char *tmp, *npath, *npathsep;

  if (NULL == fn || '\0' == *fn)
    return NULL;

  if (NULL == path)
    return filename(fn);

  if (!strcmp (fn, "-"))
    fn = strdup("stdin");

  if (FALSE == force)
    {
      const char *p = find_path (fn);
      if (*p)
        return fn;
    }
  npath = strdup(path);
  fn = filename (fn);
  tmp = malloc (strlen(npath) + 1 + strlen(fn) + 1);
  npathsep = 
    (npath == NULL || *npath == '\0') 
    ? "" : (npath[strlen (npath) - 1] != DIRSEP_CHAR ? DIRSEP_STRING : "");
  sprintf (tmp, "%s%s%s", npath, npathsep, fn);
  free(npath);
  free(fn);
  return tmp;
}

char *
filename (char *path)
{
  const char *tmp;
  if (NULL == path)
    return NULL;
  tmp = find_path (path);
  if (*tmp)
    {
      if (tmp != path)
	{
	  char *save = path;
	  path = strdup (tmp+1);
	  free (save);
	}
    }
  return path;
}

char *
find_path (char *fn)
{
  char *p = strdup (fn), *end;
  if (DIRSEP_CHAR) /* paranoia: just in case DA is ever implemented on 
		      a flat file system */
    {
      end = strrchr (p, DIRSEP_CHAR);
    }
  else
    {
      end = NULL;
    }
  if (NULL == end)
    end = fn + strlen (fn);
  else
    end = fn + (end - p);
  free (p);
  return end;
}

const char *
preslash(const char *s, const char*slash)
{
  static char buf[128];
  if (slash - s > 127)
    return NULL;
  strncpy(buf,s,slash-s);
  buf[slash-s] = '\0';
  return buf;
}

char *
expand (const char *project, const char *pqid, const char *ext)
{
  static char buf[MAX_PATH_];
  static const char*oracc;
  char projbuf[1024], prefix[5], pqid_buf[8];
  const char*colon;

  *buf = '\0';
  if (!pqid || !ext)
    return NULL;
  if (!project)
    *projbuf = '\0';
  if ((colon = strchr(pqid, ':')))
    {
      strcpy(projbuf,pqid);
      projbuf[colon-pqid] = '\0';
      pqid = ++colon;
    }
  else if (project)
    strcpy(projbuf,project);
  if (!oracc)
    oracc = oracc_home();
  strncpy(prefix,pqid,4);
  prefix[4] = '\0';
  strncpy(pqid_buf,pqid,7);
  pqid_buf[7] = '\0';
  sprintf(buf,"%s/bld/%s/%s/%s/%s.%s",oracc,projbuf,prefix,pqid_buf,pqid_buf,ext);
  return buf;
}

char *
expand_xtr (const char *project, const char *pqid, const char *code, const char *lang)
{
  static char buf[MAX_PATH_];
  static const char*oracc;
  char projbuf[1024], prefix[5], *colon;

  if (!pqid || !code)
    return NULL;

  if ((colon = strchr(pqid, ':')))
    {
      strcpy(projbuf,pqid);
      projbuf[colon-pqid] = '\0';
      pqid = ++colon;
    }
  else
    {
      if (!project)
	*projbuf = '\0';
      else
	sprintf(projbuf,"%s",project);
    }

  oracc = oracc_home();
  strncpy(prefix,pqid,4);
  prefix[4] = '\0';
  if (lang)
    sprintf(buf,"%s/bld/%s/%s/%s/%s_%s-%s.xtr",oracc,projbuf,prefix,pqid,pqid,code,lang);
  else
    sprintf(buf,"%s/bld/%s/%s/%s/%s_%s.xtr",oracc,projbuf,prefix,pqid,pqid,code);
  return buf;
}
