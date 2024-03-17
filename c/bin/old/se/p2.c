#include <stdlib.h>
#include "npool.h"
#include "p2.h"
#include "xpd2.h"

char *
p2_maybe_append_designation(const char *s, struct npool *pool, const char *final)
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
      char *tmp2 = malloc(strlen(entry)+strlen(final)+2);
      sprintf(tmp2, "%s,%s", entry, final);
      s = (char*)npool_copy((unsigned char *)tmp2,pool);
      free(tmp2);
    }
  else
    s = (char*)npool_copy((unsigned char *)s,pool);
  return (char*)s;
}

struct p2_options *
p2_load(const char *project, const char *state, struct npool *pool)
{
  struct p2_options *ret = calloc(1,sizeof(struct p2_options));
  char *opt = malloc(128);
  struct xpd *xpd = NULL;
  extern int verbose;
  extern const char *config;

  if (config)
    xpd_set_configname(config);
  xpd = xpd_init(project, pool);
  
  if (verbose)
    {
      const char *vstate = (state ? state : "(not set)");
      fprintf(stderr, "p2_load: state on entry = %s\n",vstate);
    }
  
  if (!state || strcmp(state, "special"))
    state = "default";
  else if (strcmp(state,"special"))
    {
      const char *special_ok = xpd_option(xpd, "outline-special-select");      
      if (!special_ok || strcmp(special_ok, "true"))
	state = "default";
    }

  sprintf(opt, "outline-%s-sort-final", state);
  ret->sort_final = xpd_option(xpd, opt);

  if (!ret->sort_final)
    ret->sort_final = "designation";

  sprintf(opt, "outline-%s-sort-fields", state);
  ret->sort_fields = p2_maybe_append_designation(xpd_option(xpd, opt), pool, ret->sort_final);

  fprintf(stderr, "se/p2_load: sort_fields = %s\n", ret->sort_fields);
  
  sprintf(opt, "outline-%s-sort-labels", state);
  ret->sort_labels = xpd_option(xpd, opt);

  sprintf(opt, "outline-%s-sort-heading-template", state);
  ret->heading_punct = xpd_option(xpd, opt);

  sprintf(opt, "outline-%s-catalog-fields", state);
  ret->catalog_fields = xpd_option(xpd, opt);

  sprintf(opt, "outline-%s-catalog-widths", state);
  ret->catalog_widths = xpd_option(xpd, opt);

  sprintf(opt, "outline-%s-catalog-labels", state);
  ret->catalog_labels = xpd_option(xpd, opt);

  xpd_term(xpd);
  return ret;
}
