#include <stdlib.h>
#include <stdio.h>
#include <ctype128.h>
#include "loadfile.h"
#include "npool.h"
#include "runexpat.h"
#include "fname.h"
#include "hash.h"
#include "xpd2.h"

extern int verbose;
static void xpds_load(struct xpd *, const char *);

const char *
xpd_option(struct xpd *xp, const char *name)
{
  if (xp)
    return hash_find(xp->opts, (const unsigned char *)name);
  else
    return NULL;
}

/* return a space-separated list of projects to search for
   sigs for the arg lang */
const char *
xpd_lang(struct xpd *xp, const char *lang)
{
  if (xp)
    return hash_find(xp->lang, (const unsigned char *)lang);
  else
    return NULL;
}

int
xpd_option_int(struct xpd *xp, const char *name)
{
  const char *var = hash_find(xp->opts, (const unsigned char *)name);
  if (var)
    return atoi(var);
  else
    return 0;
}

struct xpd *
xpd_init(const char *project, struct npool *pool)
{
  static char buf[64];
  if (*project == '.') 
    /* can't use 00lib because of config inheriting */
    sprintf(buf,"%s/02xml/config.xml",project);
  else
    sprintf(buf,"%s/xml/%s/config.xml",oracc_home(),project);
  if (!xaccess(buf,R_OK,0))
    {
      struct xpd *xp = calloc(1,sizeof(struct xpd));
      xp->project = (const char *)npool_copy((unsigned char *)project,pool);
      xp->file = (const char *)npool_copy((unsigned char *)buf,pool);
      xp->pool = pool;
      xp->opts = hash_create(20);
      xp->lang = hash_create(20);
      xpds_load(xp,buf);
      if (verbose)
	fprintf(stderr,"xpd_init: loaded %s\n",buf);
      return xp;
    }
  else
    {
      if (verbose)
	fprintf(stderr,"xpd_init: failed to load %s\n",buf);
      return NULL;
    }
}

/* N.B.: Do not free xpd->pool here as it is passed into 
   xpd_init from elsewhere and will be freed by caller */
void
xpd_term(struct xpd *xp)
{
  if (xp)
    {
      hash_free(xp->opts,NULL);
      hash_free(xp->lang,NULL);
      free(xp);
    }
}

static const char *xpd_ns = "http://oracc.org/ns/xpd/1.0";
#define xpd_offset (strlen(xpd_ns))

static void
xpd_sH(struct xpd *xp, const char *name, const char **atts)
{
  if (!strncmp(name,xpd_ns,xpd_offset))
    {
      const char *lname = name + xpd_offset;
      if (*lname == ':')
	{
	  ++lname;
	  if (!strcmp(lname,"option"))
	    {
	      unsigned const char *optname = (unsigned char *)findAttr(atts,"name");
	      if ('%' == *optname)
		hash_add(xp->lang,
			 npool_copy(++optname,xp->pool),
			 npool_copy((unsigned char *)findAttr(atts,"value"),xp->pool));
	      else
		hash_add(xp->opts,
			 npool_copy(optname,xp->pool),
			 npool_copy((unsigned char *)findAttr(atts,"value"),xp->pool));
	    }
	  else if (!strcmp(lname,"project"))
	    {
	      hash_add(xp->opts,
		       npool_copy((unsigned char *)"project",xp->pool),
		       npool_copy((unsigned char *)findAttr(atts, "n"),xp->pool));
	    }
	}
    }
}

static void
xpd_eH(struct xpd *xp, const char *name)
{
  char *colon = strrchr(name, ':');
  if (colon && strcmp(colon, ":option") && strcmp(colon, ":project"))
    {
      char *t = (unsigned char *)charData_retrieve(), *end;
      unsigned char *n = npool_copy(colon+1,xp->pool);
      while (isspace(*t))
	++t;
      end = t + strlen(t);
      while (end > t && isspace(end[-1]))
	--end;
      *end = '\0';
      hash_add(xp->opts,
	       n,
	       npool_copy(t,xp->pool));      
    }
  charData_discard();
}

static void
xpds_load(struct xpd *xp, const char *fname)
{
  const char *fns[2];
  fns[0] = fname;
  fns[1] = NULL;
  runexpatNSuD(i_list, fns, (void(*)(void*,const char*,const char **))xpd_sH, xpd_eH, ":", xp);
}

void
xpd_echo(const char *xpd_project, FILE *fp)
{
  char tmp[_MAX_PATH], *text;
  size_t nbytes;
  (void)sprintf(tmp,"/usr/local/oracc/xml/%s/config.xml", xpd_project);
  text = (char*)loadfile((unsigned char *)tmp, &nbytes);
  if (text)
    {
      char *xml_decl;
      if ((xml_decl = strstr(text, "<?xml")) && isspace(xml_decl[5]))
	{
	  if ((text = strstr(text, "?>")))
	    while (isspace(text[1]))
	      ++text;
	}
      fputs(text,fp);
    }
}
