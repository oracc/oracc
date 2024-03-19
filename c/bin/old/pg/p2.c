#include <stdlib.h>
#include "npool.h"
#include "p2.h"
#include "xpd2.h"

extern int csi_debug;
extern FILE *fdbg;

char *
p2_maybe_append_designation(const char *s, struct npool *pool, 
			    struct p2_options *p2opts)
{
  int designation_ok = 0;
  const char *entry = NULL;

  if (!s)
    return NULL;
  else
    entry = s;

  while (*s)
    {
      if (!strncmp(s,"designation",strlen("designation")) 
	  && s[strlen("designation")])
	{
	  designation_ok = 1;
	  break;
	}
      else
	{
	  s += strlen("designation");
	  while (*s && *s != ',')
	    ++s;
	  if (',' == *s)
	    ++s;
	}
    }
  if (!designation_ok)
    {
      char *tmp2 = malloc(strlen(entry)+strlen(p2opts->sort_final)+2);
      sprintf(tmp2, "%s,%s", entry, p2opts->sort_final);
      s = (char*)npool_copy((unsigned char *)tmp2,pool);
      free(tmp2);
    }
  else
    s = (char*)npool_copy((unsigned char *)s,pool);
  return (char*)s;
}

void
dump_pgopts(struct p2_options *o)
{
  fprintf(fdbg,
	  "pgopts->sort_fields=%s\n\tsort_labels=%s\n\tsort_final=%s\n\theading_punct=%s\n\tcatalog_fields=%s\n\tcatlog_widths=%s\n",
	  o->sort_fields,
	  o->sort_labels,
	  o->sort_final,
	  o->heading_punct,
	  o->catalog_fields,
	  o->catalog_widths);
}

struct p2_options *
p2_load(const char *project, const char *state, struct npool *pool)
{
  struct p2_options *ret = calloc(1,sizeof(struct p2_options));
  char *opt = malloc(128);
  struct xpd *xpd = xpd_init(project, pool);

  if (!state || strcmp(state, "special"))
    state = "default";
  else
    {
      const char *special_ok = xpd_option(xpd, "outline-special-select");
      if (!special_ok || strcmp(special_ok, "true"))
	state = "default";
    }

  sprintf(opt, "outline-%s-sort-fields", state);
  ret->sort_fields = xpd_option(xpd, opt);

  sprintf(opt, "outline-%s-sort-final-field", state);
  ret->sort_final = xpd_option(xpd, opt);
  if (!ret->sort_final || !*ret->sort_final)
    ret->sort_final = "designation";

  sprintf(opt, "outline-%s-sort-labels", state);
  ret->sort_labels = xpd_option(xpd, opt);

  sprintf(opt, "outline-%s-sort-heading-template", state);
  ret->heading_punct = xpd_option(xpd, opt);

  sprintf(opt, "outline-%s-catalog-fields", state);
  ret->catalog_fields = xpd_option(xpd, opt);

  sprintf(opt, "outline-%s-catalog-widths", state);
  ret->catalog_widths = xpd_option(xpd, opt);

  xpd_term(xpd);
  
  ret->sort_fields = p2_maybe_append_designation(ret->sort_fields, pool, ret);

  if (!quiet)
    fprintf(stderr, "pg/p2_load: sort_fields = %s\n", ret->sort_fields);

  if (csi_debug)
    dump_pgopts(ret);

  return ret;
}
