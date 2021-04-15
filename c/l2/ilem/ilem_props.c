/* Load a properties file for validating $props in ilem_para context.
 *
 * Format:
 *
 *  - text file, 
 *  - first token in line is property name, 
 *  - remaining tokens are property values,
 *  - indented lines continue previous value set
 *  - value '*' means uncontrolled value set
 *  - value '-' means property takes no value (boolean, presence is true, absence false)
 *  - value '@' means value may be a reference (e.g., @1 ... $@1)
 *  - value '|' is reserved for system use
 *
 * Property Names:
 *
 *  - no spaces in property names
 *
 * Values:
 *
 *  - spaces in values must be replaced with underscore
 *  - values may not use underscore which can't be globalled for space; use hyphen instead
 *
 * Using props before/after lemma:
 *
 *  - all values may be given as $property=value
 *  - unique values may be given directly with $
 *  - non-unique values must be given as $property=value
 *  - non-unique values generate an error when reading the lemprops file;
 *    this error can be suppressed by prefixing the value with '=';
 *    the '=' is a flag which is not part of the value itself
 *
 */

#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include "key.h"
#include "hash.h"
#include "npool.h"
#include "loadfile.h"
#include "warning.h"
#include "ilem_props.h"

int ilem_props_equals_flag = 0;
int ilem_props_verbose = 0;
int ilem_props_status = 0;

static char *plist = "00lib/lemprops.txt";
static int pline = 1;

static int v_ok = 1;

static Hash_table *h;
static struct npool *p;
static unsigned char *prop = NULL;

static unsigned char *
ilem_props_prop(unsigned char *s)
{
  unsigned char *t = s;
  unsigned char save = '\0';

  while (*t && ' ' != *t && '\t' != *t && '\n' != *t)
    ++t;

  if (*t)
    {
      save = *t;
      *t = '\0';
    }

  prop = npool_copy(s, p);

  /*printf("prop: :%s:\n", prop);*/

  if (save)
    *t = save;

  return t;

}

static unsigned char *
ilem_props_npool(unsigned char *v)
{
  unsigned char *pv = NULL;
  unsigned char *tmp = malloc(strlen((char*)prop)+strlen((char*)v)+2);
  sprintf((char*)tmp,"%s=%s",prop,v);
  pv = npool_copy(tmp,p);
  free(tmp);
  return pv;
}

/* prop and v are both pool copies on entry to this routine */
static unsigned char *
ilem_props_save(unsigned char *v)
{
  unsigned char *pv = NULL;
  if (prop && v)
    {
      pv = ilem_props_npool(v);
      if (hash_find(h,pv))
	{
	  fprintf(stderr, "%s:%d: ignoring duplicate property %s\n",
		  plist, pline, prop);
	  ++ilem_props_status;
	}
      else
	{
	  unsigned char *found = NULL;
	  if ((found = hash_find(h,pv)))
	    {
	      fprintf(stderr, "%s:%d: ignoring duplicate value %s in property %s\n",
		      plist, pline, v, prop);
	      ++ilem_props_status;
	    }
	  else
	    {
	      hash_add(h,pv,&v_ok);
	      if (ilem_props_verbose)
		fprintf(stdout, "adding pv %s\n", pv);
	      if ((found = hash_find(h,v)))
		{
		  /* This is a repeat occurrence of v 
		   *   found is the previous occurrences
		   *   pv    is the second occurrence
		   */
		  unsigned char *ambig_val = NULL, *tmp;
		  tmp = malloc(strlen((char*)found) + strlen((char*)pv) + 2);
		  sprintf((char*)tmp, "%s %s", found, pv);
		  ambig_val = npool_copy(tmp,p);
		  free(tmp);
		  hash_add(h,v,ambig_val);
		  if (!ilem_props_equals_flag)
		    fprintf(stderr, "%s:%d: ambiguous value %s requires PROP=VAL: %s\n",
			    plist, pline, v, ambig_val);
		  ++ilem_props_status;
		  if (ilem_props_verbose)
		    fprintf(stdout, "adding v %s || %s\n", v, ambig_val);
		}
	      else
		{
		  hash_add(h,v,pv);
		  if (ilem_props_verbose)
		    fprintf(stdout, "adding v %s => %s\n", v, pv);
		}
	    }
	}
    }
  return pv;
}

static void
ilem_props_val(unsigned char *v)
{
  if (v && *v)
    {
      if (*v == '@' || *v == '-' || *v == '*')
	{
	  if (!v[1])
	    {
#if 1
	      (void)ilem_props_save(v);
#else
	      unsigned char *pv = ilem_props_save(v);
	      fprintf(stderr, "%s:%d: pv: %s\n", plist, pline, pv);
#endif
	    }
	  else
	    {
	      fprintf(stderr, "%s:%d: property %s has bad value %s (junk after @, *, or -)\n",
		      plist, pline, prop, v);
	      ++ilem_props_status;
	    }
	}
      else if (*v == '|' && !v[1])
	{
	  fprintf(stderr, "%s:%d: in prop %s standalone value '|' is not allowed (reserved for system use)\n",
		  plist, pline, prop);
	  ++ilem_props_status;
	}
      else
	{
	  unsigned char *pv = NULL;
	  if (*v == '=')
	    {
	      ilem_props_equals_flag = 1;
	      ++v;
	    }
	  else
	    ilem_props_equals_flag = 0;
	  v = npool_copy(v,p);
#if 1
	  pv = ilem_props_save(v);
#else
	  pv = ilem_props_save(v);
	  fprintf(stderr, "%s:%d: pv: %s\n", plist, pline, pv);
#endif
	}
    }
}

static unsigned char *
ilem_props_values(unsigned char *s)
{
  unsigned char *t = s, *v = NULL;
  unsigned char save = '\0';

  while (*t && '\n' != *t)
    ++t;

  if (*t)
    {
      save = *t;
      *t = '\0';
    }

  v = s;
  while (*v)
    {
      unsigned char *w = NULL, wsave = '\0';
      while (' ' == *v || '\t' == *v)
	++v;
      w = v;
      while (*w && ' ' != *w && '\t' != *w)
	++w;
      if (*w)
	{
	  wsave = *w;
	  *w = '\0';
	}
      ilem_props_val(v);
      if (wsave)
	*w = wsave;
      v = w;
    }

  if (save)
    *t = save;

  return t;
}

static void
ilem_props_load(void)
{
  if (!access(plist, R_OK))
    {
      unsigned char *f = loadfile((unsigned char *)plist, NULL);
      unsigned char *s = NULL;
      s = f;
      while (*s)
	{
	  /* s should be at start of line */
	  switch (*s)
	    {
	    case '#':
	      while (*s && '\n' != *s)
		++s;
	      break;
	    case ' ':
	    case '\t':
	      while (*s && *s < 128 && (' ' == *s || '\t' == *s))
		++s;
	      if ('\n' != *s)
		s = ilem_props_values(s);
	      break;
	    case '\n':
	      ++pline;
	      ++s;
	      break;
	    default:
	      if ((s == f || s[-1] == '\n') && (*s > 127 || !isspace(*s)))
		s = ilem_props_prop(s);
	      s = ilem_props_values(s);
	      break;
	    }
	}
    }
  else
    {
      vwarning("can't open property file %s", plist);
    }
}

void
ilem_props_init(void)
{
  h = hash_create(1);
  p = npool_init();
  ilem_props_load();
}

void
ilem_props_term(void)
{
  hash_free(h,NULL);
  npool_term(p);
  (void)ilem_props_look(NULL);
}

/* receive a string which is KEY or KEY=VALUE; no spaces in VALUE 
 * return a structure containing pointers to key and value;
 * validate KEY/VALUE against our hash.
 */
struct keypair *
ilem_props_look(unsigned char *kv)
{
  static struct keypair kp;
  static unsigned char *tmp = NULL;
  kp.key = kp.val = NULL;
  if (kv)
    {
      unsigned char *equal = NULL;
      if ((equal = (unsigned char *)strchr((char*)kv,'=')))
	{
	  if (hash_find(h,kv))
	    {
	      kp.key = (char*)kv;
	      kp.val = (char*)(equal+1);
	      *equal = '\0';
	    }
	}
      else
	{
	  unsigned char *found = hash_find(h,kv);
	  if (found)
	    {
	      if (strchr((char*)found, ' '))
		vwarning("ambigous value requires $PROP=VAL: %s", found);
	      else
		{
		  tmp = realloc((char*)tmp,strlen((char*)found)+1);
		  strcpy((char*)tmp,(char*)found);
		  kp.key = (char*)tmp;
		  while (*tmp && *tmp != '=')
		      ++tmp;
		  *tmp = '\0';
		  kp.val = (char*)(tmp+1);
		}
	    }
	  else
	    vwarning("$%s not found in lemprops", kv);
	}
    }
  else
    {
      if (tmp)
	{
	  free(tmp);
	  tmp = NULL;
	}
    }
  return &kp;
}
