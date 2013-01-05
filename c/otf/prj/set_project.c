#include <string.h>
#include "ox.h"
#include "proj_context.h"
#include "fname.h"
#include "lang.h"
#include "npool.h"

void
set_project(struct proj_context *p, const char *proj)
{
  char projstyles[_MAX_PATH];

  project = proj;
  project_dir = project;
  sprintf(project_base,"%s/%s",oracc_home(),proj);
  
  sprintf(projstyles,"/usr/local/oracc/www/%s/office-master-styles.xml",
	  project_dir);
  if (!xaccess(projstyles,R_OK,0))
    master_styles = (const char *)npool_copy((unsigned char *)projstyles, p->owner->pool);
  else
    master_styles = "/usr/local/oracc/lib/config/office-master-styles.xml";
  sprintf(projstyles,"/usr/local/oracc/www/%s/office-automatic-styles.xml",
	  project_dir);
  if (!xaccess(projstyles,R_OK,0))
    automatic_styles = (const char *)npool_copy((unsigned char *)projstyles, p->owner->pool);
  else
    automatic_styles = "/usr/local/oracc/lib/config/office-automatic-styles.xml";
  sprintf(projstyles,"/usr/local/oracc/www/%s/office-styles.xml",
	  project_dir);
  if (!xaccess(projstyles,R_OK,0))
    styles_styles = (const char *)npool_copy((unsigned char *)projstyles, p->owner->pool);
  else
    styles_styles = "/usr/local/oracc/lib/config/office-styles.xml";
}
