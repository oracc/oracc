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
 *  - value '#' means value may be a reference (e.g., @1 ... $@1)
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

static Hash_table *h;
static struct npool *p;

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

  printf("prop: :%s:\n", s);

  if (save)
    *t = save;

  return t;

}

static unsigned char *
ilem_props_values(unsigned char *s)
{
  unsigned char *t = s;
  unsigned char save = '\0';

  while (*t && '\n' != *t)
    ++t;

  if (*t)
    {
      save = *t;
      *t = '\0';
    }

  printf("values: %s\n", s);
  if (save)
    *t = save;

  return t;
}

static void
ilem_props_load(void)
{
  char *plist = "00lib/lemprops.txt";
  int pline = 1;
  unsigned char *tok = NULL, *val = NULL;
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
	      tok = NULL;
	      ++s;
	      break;
	    default:
	      if (tok == NULL && (*s > 127 || !isspace(*s)))
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
  p = npool_init();
  ilem_props_load();
}

void
ilem_props_term(void)
{
  hash_free(h,NULL);
  npool_term(p);
}

/* receive a string which is KEY or KEY=VALUE; no spaces in VALUE 
 * return a structure containing pointers to key and value;
 * validate KEY/VALUE against our hash.
 */
struct keypair *
ilem_props_look(const char *kv)
{
  static struct keypair kp;
  return &kp;
}

