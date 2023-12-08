#include <stdlib.h>
#include <stdio.h>
#define _MAX_PATH 1024
extern char *strdup(const char *);

const char *
skl_file_e(const char *project, const char *skel, const char *ext)
{
  static char buf[_MAX_PATH];
  if (project == NULL || !*project || skel == NULL || !*skel)
    return NULL;
  sprintf(buf,"@@ORACC@@/www/%s/%s.%s",project,skel,ext);
  return strdup(buf);
}

const char *
skl_file(const char *project, const char *skel, int sky_ext)
{
  return skl_file_e(project,skel,sky_ext ? "sky" : "svl");
}

const char *
skl_file_l(const char *project, const char *skel, const char *lang, const char *ext)
{
  static char buf[_MAX_PATH];
  if (project == NULL || !*project || skel == NULL || !*skel)
    return NULL;
  sprintf(buf,"@@ORACC@@/www/%s/%s-%s.%s",project,skel,lang,ext);
  return strdup(buf);
}

const char *
skl_file_lang(const char *project, const char *skel, const char *lang, int sky_ext)
{
  return skl_file_l(project,skel,lang,sky_ext ? "sky" : "svl");
}
