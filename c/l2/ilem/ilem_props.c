/* Load a properties file for validating $props in ilem_para context.
 * Format is a text file, 
 *  - first token in line is property name, 
 *  - remaining tokens are property values,
 *  - indented lines continue previous value set
 *  - value '*' means uncontrolled value set
 *  - value '-' means property takes no value (boolean, presence is true, absence false)
 *  - value '#' means value may be a reference (e.g., @1 ... $@1)
 *  - all values may be given as $property=value
 *  - unique values may be given directly with $
 *  - non-unique values must be given as $property=value
 *  - spaces in values must be replaced with underscore
 *  - values may not use underscore; use hyphen if necessary
 */

#include <stdlib.h>
#include <unistd.h>
#include "key.h"
#include "hash.h"
#include "npool.h"
#include "loadfile.h"
#include "warning.h"

static Hash_table *h;
static struct npool *p;

static void
ilem_props_load(void)
{
  char *plist = "00lib/ilemprops.txt";
  if (!access(plist, R_OK))
    {
      unsigned char *f = loadfile((unsigned char *)plist, NULL);
      if (f)
	printf("%s\n",f);
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

