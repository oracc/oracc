#include <ctype128.h>
#include <psd_base.h>
#include <loadfile.h>
#include <list.h>
#include "se.h"
#include "selib.h"

#ifndef strdup
extern char *strdup(const char*);
#endif

extern const char *prog;

struct keytab
{
  const char *name;
  unsigned char **lines;
};

static struct keytab *
get_keytab(const char *proj,const char *index)
{
  struct keytab *kp;
  static List *keytabs = NULL;
  const char *iname = se_file(proj,index,"fieldnames.tab");
  if (NULL == keytabs)
    keytabs = list_create(LIST_SINGLE);
  else
    {
      for (kp = list_first(keytabs); kp; kp = list_next(keytabs))
	if (!strcmp(iname,kp->name))
	  break;
      if (kp)
	return kp;
    }
  kp = malloc(sizeof(struct keytab));
  kp->name = strdup(iname);
  kp->lines = loadfile_lines((unsigned char *)kp->name,NULL);
  if (kp->lines)
    {
      progress("se eval: opened keytab %s\n",kp->name);
      list_add(keytabs,kp);
    }
  else
    {
      error(NULL,"se eval: failed to open keytab %s\n",kp->name);
      kp = NULL;
    }
  return kp;
}

/* return -1 for field code not found; -2 for ambiguous field code */
int
fieldcode(const char *proj,const char *index,const char *fldname)
{
  struct keytab *kp;
  int code = -1;
  int nmatches = 0;

  if (!proj || !index || !fldname || !strcmp(fldname,"*"))
    return -1;
  kp = get_keytab(proj,index);
  if (kp)
    {
      unsigned char **lp = kp->lines;
      int len = strlen(fldname);
      while (*lp)
	{
	  if (!strncmp(fldname,(const char *)*lp,len))
	    {
	      unsigned char *c = *lp;
	      c = c + len;
	      if (*c && isspace(*c))
		{
		  while (isspace(*c))
		    ++c;
		  code = atoi((const char *)c);
		  nmatches = 1;
		  break;
		}
	      else
		{
		  while (*c && !isspace(*c))
		    ++c;
		  code = atoi((const char *)c+1);
		  ++nmatches;
		}
	    }
	  ++lp;
	}
    }
  if (nmatches > 1)
    fprintf(stderr,"%s: field %s ambiguous in index %s/%s\n",prog,fldname,proj,index);
  else if (nmatches == 0)
    fprintf(stderr,"%s: field %s not found in index %s/%s\n",prog,fldname,proj,index);
  return code;
}
