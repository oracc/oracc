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
static const char *current_PQ, *current_proj;
extern FILE *f_log;
extern int check_links;

static int loading_links = 1;

static void
sH(void *userData, const char *name, const char **atts)
{
  if (name[0] == 'l' && name[1] == '\0' && loading_links)
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
      fprintf(stderr,"loading label %s#%s = %s\n", current_PQ, label, xmlid);
#endif
      if (xmlid && label) /* lgs doesn't have id/label */
	{
	  char *hashid = malloc(strlen(current_proj)+strlen(xmlid)+2);
	  (void)sprintf(hashid, "%s:%s", current_proj, xmlid);
#if 0
	  /* THIS ONLY APPLIES IF WE START HARVESTING LABELS FROM .xsf FILES */
	  /* the correct ID to use is now on the lg tag in scores, but the label is still
	     on the l tag. The l tag's ID is the lg ID with a suffixed ell, so this code
	     removes the suffixed ell leaving the ID to point to the lg but still using the
	     correct label from the l tag */
	  if ('l' == xmlid[strlen(xmlid)-1])
	    xmlid[strlen(xmlid)-1] = '\0';
#endif
	  sprintf(buf,"%s#%s",current_PQ,label);
	  hash_insert((unsigned char *)buf,hashid,label_table);
	  hash_insert(hashid,(unsigned char *)buf,label_table);
	}
    }
  else if (name[0] == 'x' && name[1] == 'c' && name[2] == 'l' && name[3] == '\0')
    loading_links = 0;
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
      char *f = malloc(strlen(PQ)+1), *fproj = NULL, *fid = NULL;
      strcpy(f, PQ);
      fproj = f;
      fid = strchr(fproj,':');
      *fid++ = '\0';
      
      current_PQ = PQ;
      current_proj = fproj;

      fname[0] = l2_expand(fproj, fid, "xtf");
      fname[1] = NULL;
      if (!access(fname[0],R_OK))
	{
	  loading_links = 1;
	  hash_insert((unsigned char*)PQ,definedp,label_table);
	  runexpat(i_list, fname, sH, eH);
	}
      else
	{
	  hash_insert((unsigned char*)PQ,undefinedp,label_table);
	  if (check_links)
	    vnotice("disabling label-checking because %s was not found", PQ);
	}
    }
}

const char *
label_to_id(const char *qualified_id, const char *label)
{
  char *lbuf = malloc(strlen(qualified_id)+strlen(label)+2), *tmp;
  char *id = NULL;
  (void)sprintf(lbuf, "%s#%s", qualified_id, label);
  for (tmp = lbuf; *tmp; ++tmp)
    if ('_' == *tmp)
      *tmp = ' ';
  id = hash_find(label_table, (const unsigned char *)lbuf);
  if (id)
    /*fprintf(stderr, "label_to_id: %s maps to id %s\n", lbuf, id)*/;
  else
    {
      char *tmp = hash_find(label_table, (const unsigned char *)qualified_id);
      if (!tmp || *tmp == '1') /* files not found are have a "0" as their value in label_table */
	{
	  char *hyphen = NULL;
	  if ((hyphen = strchr(lbuf,'-')))
	    {
	      *hyphen = '\0';
	      id = hash_find(label_table, (const unsigned char *)lbuf);
	      if (!id)
		{
		  char *h = malloc(strlen(lbuf));
		  strcpy(h,lbuf);
		  *hyphen = '-';
		  fprintf(stderr, "label_to_id: %s not found (tried %s as well)\n", lbuf, h);
		}	     
	    }
	  else
	    fprintf(stderr, "label_to_id: %s not found\n", lbuf);
	}
    }
  free(lbuf);
  if (id)
    {
      char *tmp;
      tmp = strchr(id, ':');
      if (tmp)
	id = tmp + 1;
    }
  return id;
}
