#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "xsystem.h"

/* Simple file finder for CDF: only understand 
   .atf .otf .jp(e)g and .png, only look in two
   places within the project for either type.
 */

extern char project_base[];
extern int verbose;

const char *image_paths[] = 
  {
    "." , "00res/images" , "00lib/images" , NULL
  };

const char *text_paths[] = 
  {
    "." , "00atf" , "01tmp/00atf", NULL
  };

#define _MAX_PATH 1024
static char buf[_MAX_PATH];

char *
cdt_findfile(const char *file)
{
  const char *dot = NULL, **paths;
  int i;

  if (!file || !((dot = strrchr(file,'.'))))
    return NULL;
  if (dot[1] == 'a' || dot[1] == 'o')
    paths = text_paths;
  else
    paths = image_paths;
  
  for (i = 0; paths[i]; ++i)
    {
      if (!strcmp(paths[i], "."))
	sprintf(buf,"%s",file);
      else
	sprintf(buf,"%s/%s/%s",project_base,paths[i],file);
      if (!xaccess(buf,R_OK,0))
	{
	  if (verbose)
	    fprintf(stderr,"cdt_findfile: found `%s'\n", buf);
	  return buf;
	}
      else if (verbose)
	fprintf(stderr,"cdt_findfile: no file `%s'\n", buf);
    }

  return NULL;
}
