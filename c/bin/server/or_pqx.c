#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype128.h>
#include "resolver.h"
#include "content.h"

extern int patterns_debug;

const char *catentry_html = "/Users/stinney/orc/lib/scripts/g2-xmd-HTML.xsl";
const char *pqx_html = "/Users/stinney/orc/lib/scripts/p3-html-wrapper.xsl";
const char *proofing_html = "/Users/stinney/orc/lib/scripts/g2-xtf-HTML.xsl";
const char *sxh_html = "/Users/stinney/orc/lib/scripts/sxh-view.xsl";
const char *no_html = "/Users/stinney/orc/www/no_html.html";
extern struct component *have_component[];
static void or_image(const char *project, const char *pqx, const char *type);

static char *
make_key_url(struct component *components)
{
  int i, len = 0;
  char *k = NULL;
  for (i = 1; components[i].text; ++i)
    len += strlen(components[i].text) + 1;
  k = malloc(len+2);
  *k = '\0';
  for (i = 1; components[i].text; ++i)
    {
      if (components[i].type != C_VIEW || components[i].value != V_PROJECT_TYPE)
	(void)sprintf(k + strlen(k), "%s/", components[i].text);
    }
  k[strlen(k)-1] = '\0';
  return k;
}

void
pqx_handler(struct component *components)
{
  if (patterns_debug)
    show_components(components);
  else
    {
      if (project)
	{
	  if (components[0].count == 1)
	    {
	      char *key_url = make_key_url(components);
	      struct h_pqxtab *h = NULL;
	      if (key_url && strlen(key_url))
		{
		  if ((h = pqx_h_tab(key_url, strlen(key_url))))
		    {
		      h->func(project, components);
		    }
		  else
		    {
		      do404();
		    }
		}
	      else
		h_pqx_html_pager(project, components);
	    }
	  else
	    h_pqx_html_pager(project, components);
	}
      else
	{
	  find(NULL, make_find_phrase("txt", elements[0], 1));
	}
    }
}

void
h_pqx_cuneified_html(const char *project, struct component *components)
{
  char *qs = NULL, *xproject = NULL, pqx[8], *colon;
  
  if (components[0].replace)
    {
      const char *pqid = components[0].replace;
      if ((colon = strchr(components[0].replace, ':')))
	{
	  strcpy(pqx, colon+1);
	  xproject = malloc(1+(colon-pqid));
	  strncpy(xproject,pqid,colon-pqid);
	  xproject[colon-pqid] = '\0';
	}
      else
	{
	  xproject = (char*)project;
	  strcpy(pqx, pqid);
	}
    }
  else
    {
      xproject = (char*)project;
      strcpy(pqx, components[0].text);
    }

  qs = malloc(strlen(xproject) + strlen(pqx) + 16);
  sprintf(qs, "project=%s&text=%s", xproject, pqx);
  setenv("QUERY_STRING", qs, 1);
  cuneify();
  do404();
}

static void
h_pqx_html_handler(const char *project, struct component *components,
		   int tlit, const char *xlat)
{
  char *div = or_find_pqx_file(project, 
			       components[0].replace 
			       ? components[0].replace 
			       : components[0].text, 
			       (!strcmp(xlat, "score") ? ".sxh" : "html"));
  const char *line = NULL, *frag = NULL;
  struct content_opts *cop = NULL;
  
  if (query_string && *query_string)
    {
      char *qs = malloc(strlen(query_string)+1);
      strcpy(qs, query_string);
      line = qs;
      qs = strchr(qs, ',');
      if (qs)
	{
	  *qs++ = '\0';
	  frag = qs;
	}
      if ((!strlen(line) || !_is_ncname(line))
	  || (frag && strlen(frag) && !_is_ncname(frag)))
	{
	  do404();
	}
    }
  print_hdr();
#if 1
  cop = content_new_options();
  cop->echo = 1;
  cop->hilite_id = line;
  cop->frag_id = frag;
  cop->sigs = 1;
  content(cop, div);
#else
  if (line && frag)
    execl("/Users/stinney/orc/bin/sigfixer", "sigfixer",
	  project,
	  "xsltproc", 
	  "--stringparam", "standalone", "true",
	  "--stringparam", "project", project,
	  "--stringparam", "transonly", tlit ? "false" : "true",
	  "--stringparam", "trans", xlat,
	  "--stringparam", "line-id", line,
	  "--stringparam", "frag-id", frag,
	  pqx_html,
	  div, NULL);
  else if (line)
    execl("/Users/stinney/orc/bin/sigfixer", "sigfixer",
	  project,
	  "xsltproc", 
	  "--stringparam", "standalone", "true",
	  "--stringparam", "project", project,
	  "--stringparam", "transonly", tlit ? "false" : "true",
	  "--stringparam", "trans", xlat,
	  "--stringparam", "line-id", line,
	  pqx_html,
	  div, NULL);
  else
    execl("/Users/stinney/orc/bin/sigfixer", "sigfixer",
	  project,
	  "xsltproc", 
	  "--stringparam", "standalone", "true",
	  "--stringparam", "project", project,
	  "--stringparam", "transonly", tlit ? "false" : "true",
	  "--stringparam", "trans", xlat,
	  pqx_html,
	  div, NULL);
#endif
  do404();
}

void
h_pqx_html(const char *project, struct component *components)
{
  if (components[0].count == 1)
    {
      h_pqx_html_handler(project, components, 1, "en");
    }
  else
    do400("PQX lists cannot be returned as HTML");
}

void
h_pqx_html_pager(const char *project, struct component *components)
{
  /* line_id/frag_id are not used yet */
  const char *line_id = NULL, *frag_id = NULL, *mode = NULL;

  if (query_string)
    line_id = query_string;
  else
    {
      if (!line_id)
	line_id = "none";
    }
  if (!frag_id)
    frag_id = "none";
  if (have_component[C_UI])
    mode = have_component[C_UI]->text;
  else
    mode = "full";
#if 1
  execl("/usr/bin/perl", "perl", "/Users/stinney/orc/bin/p3-pager.plx",
	cgi_arg("project", project),
	cgi_arg("adhoc", 
		components[0].replace ? components[0].replace : components[0].text),
	cgi_arg("line-id", line_id),
	cgi_arg("frag-id", frag_id),
	cgi_arg("mode", mode),
	NULL);
#else
  execl("/usr/bin/perl", "perl", "/Users/stinney/orc/bin/p3-pager.plx",
	"-p", cgi_arg("project", project),
	"-p", cgi_arg("adhoc", 
		      components[0].replace ? components[0].replace : components[0].text),
	"-p", cgi_arg("line-id", line_id),
	"-p", cgi_arg("frag-id", frag_id),
	"-p", cgi_arg("mode", mode),
	NULL);
#endif
}

void
h_pqx_image(const char *project, struct component *components)
{
  or_image(project, components[0].text, "image");
}

void
h_pqx_images(const char *project, struct component *components)
{
  or_image(project, components[0].text, "all");
}

void
h_pqx_cat_html(const char *project, struct component *components)
{
  char *xmd = or_find_pqx_file(project, 	
			       components[0].replace 
			       ? components[0].replace 
			       : components[0].text, "xmd");
  print_hdr();
  execl("/usr/bin/xsltproc", "xsltproc", 
	catentry_html, 
	xmd, NULL);
  do404();
}

void
h_pqx_cat_xml(const char *project, struct component *components)
{
  char *xmd = or_find_pqx_file(project, 
			       components[0].replace 
			       ? components[0].replace 
			       : components[0].text, "xmd");
  cat_file(xmd);
}

void
h_pqx_photo(const char *project, struct component *components)
{
  or_image(project, components[0].text, "photo");
}

void
h_pqx_proofing_html(const char *project, struct component *components)
{
  char *xtf = or_find_pqx_file(project, 
			       components[0].replace 
			       ? components[0].replace 
			       : components[0].text, "xtf");
  print_hdr();
  execl("/usr/bin/xsltproc", "xsltproc", 
	"--param", "standalone", "true()",
	"--stringparam", "project", project,
	"--stringparam", "proofing-mode", "true()",
	proofing_html, 
	xtf, NULL);
  do404();
}

void
h_pqx_score_html(const char *project, struct component *components)
{
  const char *text = components[0].text;
  char *tmp = or_find_pqx_file(project, 
			       components[0].replace 
			       ? components[0].replace 
			       : components[0].text, "sxh");
  if (!access(tmp, R_OK))
    {
      fprintf(stderr, "/score/: %s; query_string=%s\n", tmp, query_string);
      if (query_string && *query_string)
	{
	  const char *q = query_string;
	  if (q[0] == 's' && q[1] == 'b' && q[2] == '.')
	    {
	      q += 3;
	      if (!strncmp(q, text, 7))
		{
		  q += 7;
		  if (q[0] == '.')
		    {
		      ++q;
		      while (isdigit(*q))
			++q;
		      if (!*q)
			{
			  /* OK, we have a valid score block ID */
			  /* do we want frag or hilited block? */
			  print_hdr();
			  execl("/Users/stinney/orc/bin/xfrag", "xfrag", 
				"-hs", "-p", project,
				tmp, query_string, NULL);
			}
		    }
		}
	    }
	  do404();
	}
      else
	{
	  /*cat_html_file(tmp);*/
	  sigfixer_html(tmp);
	}
    }
  else
    {
      free(tmp);
      tmp = or_find_pqx_file(project, 
			     components[0].replace 
			     ? components[0].replace 
			     : components[0].text, "xsf");
      if (!access(tmp, R_OK))
	{
	  h_pqx_html_handler(project, components, 1, "score");	  
	}
      free(tmp);
      do404();
    }
}

void
h_pqx_sources(const char *project, struct component *components)
{
  const char *text = components[0].text;
  char *tmp = or_find_pqx_file(project, 
			       components[0].replace 
			       ? components[0].replace 
			       : components[0].text, "xtl");
  if (!access(tmp, R_OK))
    {
      list(tmp);
    }
  else
    {
      free(tmp);
      do404();
    }
}

void
h_pqx_thumb(const char *project, struct component *components)
{
  or_image(project, components[0].text, "thumb");
}

void
h_pqx_tlit_html(const char *project, struct component *components)
{
  if (components[0].count == 1)
    {
      h_pqx_html_handler(project, components, 1, "none");
    }
  else
    do400("PQX lists cannot be returned as HTML");
}

void
h_pqx_tlit_tei(const char *project, struct component *components)
{
  char *xtf = or_find_pqx_file(project, 
			       components[0].replace 
			       ? components[0].replace 
			       : components[0].text, "tei");
  cat_file(xtf);
}

void
h_pqx_tlit_xml(const char *project, struct component *components)
{
  char *xtf = or_find_pqx_file(project, 	
			       components[0].replace 
			       ? components[0].replace 
			       : components[0].text, "xtf");
  cat_file(xtf);
}

void
h_pqx_trans_html(const char *project, struct component *components)
{
  if (components[0].count == 1)
    {
      h_pqx_html_handler(project, components, 0, "en");
    }
  else
    do400("PQX lists cannot be returned as HTML");
}

void
h_pqx_trans_xml(const char *project, struct component *components)
{
  char *xtr = or_find_pqx_xtr(project, 
			      components[0].replace 
			      ? components[0].replace 
			      : components[0].text, "project", "en");
  cat_file(xtr);
}

void
h_pqx_view(const char *project, struct component *components)
{
  const char *script_project = NULL, *tmp;
  const char *script_name = components[have_component[V_PROJECT]->index+1].text;
  char *script_xsl = NULL;

  if ((tmp = strchr(script_name, ':')))
    {
      char *tmp2 = malloc(strlen(script_name) + 1);
      strcpy(tmp2, script_name);
      script_project = tmp2;
      tmp2[tmp-script_name] = '\0';
      script_name = script_name + (script_name - tmp) + 1;
    }
  else
    {
      script_project = project;
    }

  script_xsl = or_find_www_file(script_project, NULL, 
				script_name,
				"xsl");
  fprintf(stderr, "view/%s requires %s\n", 
	  components[have_component[V_PROJECT]->index+1].text,
	  script_xsl);
  if (!access(script_xsl, R_OK))
    {
        print_hdr();
	execl("/usr/bin/xsltproc", "xsltproc", 
	      "--param", "standalone", "true()",
	      "--stringparam", "project", project,
	      "--stringparam", "pqx", components[0].text,
	      script_xsl,
	      "/Users/stinney/orc/lib/data/empty.xml", NULL);
    }
  else
    fprintf(stderr, "%s : no such script\n", script_xsl);
  do404();
}

/* This should really return a packet containing all XML data for the text */
void
h_pqx_xml(const char *project, struct component *components)
{
  h_pqx_tlit_xml(project, components);
}

static void
or_image(const char *project, const char *pqx, const char *type)
{
  const char *subtype = (query_string ? query_string : "-");
  execl("/usr/bin/perl", "perl", 
	"/Users/stinney/orc/bin/htmlimage.plx",
	"-xmd", or_find_pqx_file("cdli",pqx, "xmd"), 
	"-type", type,
	"-subtype", subtype,
	NULL);
}
