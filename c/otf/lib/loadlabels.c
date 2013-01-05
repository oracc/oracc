#include <unistd.h>
#include <string.h>
#include <psd_base.h>
#include <runexpat.h>
#include <fname.h>
#include <hash.h>
#include <pool.h>
#include "./warning.h"

#define hash_lookup(keyp,tablep) hash_find(tablep,keyp)
static void *
hash_insert(unsigned char *keyp, void *datap, Hash_table *tablep)
{
  if (!hash_find(tablep,keyp))
    hash_add(tablep,pool_copy(keyp),datap);
  return datap;
}

extern char *strdup(const char *);

Hash_table *label_table;
extern const char *project;
static const char *current_PQ;
extern FILE *f_log;

static void
sH(void *userData, const char *name, const char **atts)
{
  if (name[0] == 'l' && name[1] == '\0')
    {
      const char *xmlid = NULL;
      const char *label = NULL;
      char buf[128];
      int i;
      for (i = 0; atts[i] != NULL; i+=2)
	{
	  if (!strcmp(atts[i],"xml:id"))
	    xmlid = atts[i+1];
	  else if (!strcmp(atts[i],"label"))
	    label = atts[i+1];
	  if (xmlid && label)
	    break;
	}
#if 0
      fprintf(stderr,"loading label %s:%s = %s\n", current_PQ, label, xmlid);
#endif
      if (xmlid && label) /* lgs doesn't have id/label */
	{
	  sprintf(buf,"%s:%s",current_PQ,label);
	  hash_insert((unsigned char *)buf,strdup(xmlid),label_table);
	}
    }
}

static void
eH(void *userData, const char *name)
{
}

void
load_labels_init()
{
  /*hash_construct_table(&label_table,4999);*/
  label_table = hash_create(4999);
}

void
load_labels_term()
{
  hash_free(label_table,NULL);
  label_table = NULL;
}

void
define_labels_symbol(const char *sym, const char *idp)
{
  char buf[20];
  sprintf(buf,"%s#%s",textid,sym);
  hash_insert((unsigned char *)buf,(char*)idp,label_table);
  /* fprintf(stderr,"defining symbol %s as %s\n", buf, idp); */
}

void
load_labels(const char *PQ)
{
  const char *fname[2];
  static char *definedp = "1";
  static char *undefinedp = "0";
  if (!hash_lookup((unsigned char *)PQ,label_table))
    {
      current_PQ = PQ;
      fname[0] = expand(project, PQ, "xtf");
      fname[1] = NULL;
      if (!access(fname[0],R_OK))
	{
	  hash_insert((unsigned char*)PQ,definedp,label_table);
	  runexpat(i_list, fname, sH, eH);
	}
      else
	{
	  hash_insert((unsigned char*)PQ,undefinedp,label_table);
#if 0
	  {
	    const char *ename = strstr(fname[0], project);
	    if (!ename)
	      {
		ename = fname[0];
		vwarning("%s: not readable/no label checking", ename);
		--status; /* don't count this as a real error */
	      }
	  }
#endif

	}
    }
}
