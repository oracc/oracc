#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <psd_base.h>
#include <misc.h>
#include <skl.h>

void
skl_copy(const char *project, const char *skel, const char *type, const char*dir)
{
  const char *file = NULL, *base;
  char dest[_MAX_PATH];
  int skyext = !strcmp(type,"idref");

  if (NULL == (file = skl_file(project,skel,skyext)))
    return;
  base = file + strlen(file);
  while (base > file && base[-1] != '/')
    --base;
  sprintf(dest,"%s/%s",dir,base);
  filecopy(file,dest);
}
