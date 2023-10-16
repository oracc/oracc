#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <wchar.h>
#include <wctype.h>
#include <locale.h>
#include <gdl.h>

#include <oraccsys.h>
#include <pool.h>
#include <dbi.h>
#include <gutil.h>
#include "sll.h"

struct sllparts
{
  const char *id;
  const char *sn;
  const char *v;
};

/* Web variables used when called from url-resolver. Note that
   wextension and wgrapheme are overloaded to serve as global records
   of the parts of the key request when in CLI mode, so it's important
   to test only wcaller to determine if the request is coming from a
   web source. */
const char *wcaller = NULL;
const char *wextension = NULL;
const char *wgrapheme = NULL;
const char *wproject = NULL;

static void sll_esp_output(List *lp);
static void sll_esp_ext(const char *key, const char *ext, List *lp);
static void sll_esp_header(const char *v, const char *ext);
static void sll_esp_trailer(void);
static void sll_esp_p(const char *oid, const char *sn, const char *v, const char *p);
static void sll_web_error(const char *err);
static void sll_web_output(List *lp);
static void sll_uri_output(List *lp);
static void sll_sign_frame(const char *oid);
static void sll_open_frame_divs(void);
static void sll_inter_frame_divs(void);
static void sll_close_frame_divs(void);
static void sll_html_header(void);
static void sll_html_p(const char *id, const char *sn, const char *v, const char *p);
static void sll_html_trailer(void);
static struct sllparts *sll_split(const char *sc);

void
sll_web_handler(const char *wcall, const char *wproj, const char *wgraph, const char *wext)
{
  struct sllext *ep = NULL;

  /* This code allows sll_web_handler to be called from another
     program with the global variables as function arguments; when
     called from slx they are set directly from the options code */
  if (!wcaller)
    wcaller = wcall;
  if (!wproject)
    wproject = wproj ? wproj : "ogsl";
  if (!wgrapheme)
    wgrapheme = wgraph;
  if (!wextension)
    wextension = wext;

  if (wextension && !strcmp(wextension, "#none"))
    wextension = NULL;

  char *gdloo = gdl_one_off("<web>", 1, wgrapheme, 1);
  
  if (gdloo == NULL)
    sll_web_error("invalid grapheme in search request");
  else if (wextension && !(ep = sllext(wextension, strlen(wextension))))
    sll_web_error("error");
  else
    {
      Dbi_index *dbi = sll_init_d(wproject, NULL);
      if (dbi)
	{
	  sll_web_output(sll_resolve((uccp)gdloo, wextension, ep));
	  sll_term_d(dbi);
	}
      else
	sll_web_error("failed to open signlist database");
    }
  fflush(stdout);
  exit(0);
}

static void
sll_web_error(const char *err)
{
  fprintf(stderr, "%s\n", err);
}

static void
sll_web_output(List *lp)
{
  if (lp && list_len(lp))
    {
      if (!strcmp(wcaller, "esp"))
	sll_esp_output(lp);
      else
	sll_uri_output(lp);
    }
  else
    {
      if (!strcmp(wgrapheme, "#none"))
	{
	  sll_html_header();
	  sll_sign_frame("");
	  sll_html_trailer();
	}
      else
	{
	  sll_html_header();
	  printf("<p class=\"nomatch\">No matches</p>");
	  sll_html_trailer();
	}
    }
}

/********************************************************************
 *** Routines to respond to a FORM query from an esp-based signlist
 ********************************************************************/

static void
sll_esp_output(List *lp)
{
  if (wextension)
    sll_esp_ext(wgrapheme, wextension, lp);
  else
    {
      const char *letter = NULL, *oid = NULL;
      char *html = NULL;
      oid = list_first(lp);
      if (!oid)
	{
	  sll_esp_header(wgrapheme, wextension);
	  printf("<p class=\"nomatch\">No matches</p>");
	  sll_esp_trailer();
	}
      else
	{
	  letter = (ccp)sll_lookup(sll_tmp_key((uccp)oid, "let"));
	  if (!letter)
	    letter = "BAD";
	  html = (char*)pool_alloc(strlen(wproject) + strlen(letter) + strlen(oid) + strlen("//signlist///index.html") + 1, sllpool);
	  sprintf(html, "/%s/signlist/%s/%s/index.html", wproject, letter, oid);
	  printf("Status: 302 Found\nLocation: %s\n\n", html);
	}
    }
}

static void
sll_esp_ext(const char *key, const char *ext, List *lp)
{
  char *r;
#if 0
  int i = 0;
#endif
  sll_esp_header(key,ext);
  for (r = list_first(lp); r; r = list_next(lp))
    {
      char *s = r;
      struct sllparts *sp = NULL;
#if 0
      const char *p = "";
      /* punctuation is unnecessary in the online signlist lookup */
      if (i++)
	p = ";";
#endif
      sp = sll_split(s);
      sll_esp_p(sp->id, sp->sn, sp->v, NULL);
    }
  sll_esp_trailer();
}

/* FIXME: v needs to be stripped of risky characters here */
static void
sll_esp_header(const char *v, const char *ext)
{
  char *vcat = NULL;
  struct sllext *ep = NULL;

  if (ext)
    {
      ep = sllext(ext, strlen(ext));
      vcat = (char*)pool_alloc(strlen(v) + strlen(ep->pre) + strlen(ep->pst) + 1, sllpool);
      (void)sprintf(vcat, "%s%s%s", ep->pre, v, ep->pst);
    }
  else if (v)
    vcat = (char*)v;
  else
    vcat = "(last search)";
  printf("%s\n\n", "Content-type: text/html; charset=utf-8");
  printf("%s\n", "<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\"sux\" xml:lang=\"sux\">");
  printf("%s\n", "<head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />");
  printf("<title>OGSL Results for %s</title>\n", vcat);
  printf("%s\n", "<link rel=\"shortcut icon\" type=\"image/ico\" href=\"/favicon.ico\" />");
  printf("%s\n", "<link rel=\"stylesheet\" type=\"text/css\" href=\"/css/p3.css\" />");
  printf("%s\n", "<script src=\"/js/p3.js\" type=\"text/javascript\"><![CDATA[ ]]></script>");
  printf("%s\n", "</head><body class=\"ogslres\">");
  printf("<h1 class=\"ogslres\">%s</h1>\n", vcat);
}

static void
sll_esp_trailer(void)
{
  printf("%s\n", "</body></html>");
}

static void
sll_esp_p(const char *oid, const char *sn, const char *v, const char *p)
{
  const char *vx = (v ? "&#xa0;=&#xa0;" : "");
  const char *pspan = (p ? "<span class=\"ogsl-punct\">;</span>" : "");
  char html[1024];  
  const char *letter = (ccp)sll_lookup(sll_tmp_key((uccp)oid,"let"));
  if (!v)
    v = "";
  (void)sprintf(html, "/%s/signlist/%s/%s/index.html", wproject, letter, oid);
  printf("<p><a target=\"slmain\" href=\"%s\">%s%s<span class=\"sign\">%s</span></a>%s</p>\n",
	 html, v, vx, sn, pspan);
}

/********************************************************************
 *** Routines to respond to a URI-based request; not supported yet
 ********************************************************************/

static void
sll_uri_output(List *lp)
{
  if (wextension)
    {
      const char *v = NULL;
      const char *first_id = NULL;
      int i = 0;
      Hash *seen = hash_create(1);
      
      sll_html_header();
      for (v = list_first(lp); v; v = list_next(lp))
	{
	  struct sllparts *sp = sll_split(v);
	  if (!hash_find(seen, (uccp)sp->v))
	    {
	      const char *p = (i++ ? "; " : "");
	      hash_add(seen, (uccp)sp->v, "");
	      if (!first_id)
		first_id = sp->id;
	      sll_html_p(sp->id,sp->sn,sp->v,p);
	    }
	}
      if (!strstr(wproject, "epsd2"))
	sll_sign_frame(first_id);
      sll_html_trailer();
    }
  else
    {
      sll_html_header();
      sll_sign_frame(list_first(lp));
      sll_html_trailer();
    }
}

/* WHAT DOES IT MEAN TO CALL THIS WITH A "" ARG ? */
static void
sll_sign_frame(const char *oid)
{
  sll_inter_frame_divs();
  printf("<iframe seamless=\"seamless\" class=\"sign-frame\" id=\"signframe\" src=\"/%s/signs/%s.html\"> </iframe>", wproject, oid);
  sll_close_frame_divs();
}

static void
sll_open_frame_divs(void)
{
  printf("<div name=\"ogsltop\" id=\"ogsltop\">");
}

static void
sll_inter_frame_divs(void)
{
  printf("</div><div name=\"ogsl-body\" id=\"ogslbody\">");
}

static void
sll_close_frame_divs(void)
{
  printf("</div>");
}

static void
sll_html_header(void)
{
  char *vcat = NULL;
  const char *ext = wextension;
  if (wextension)
    {
#if 0 /* WHAT WAS THIS SUPPOSED TO DO IN PERL VERSION */
      const char *sn = NULL;
      if (!strcmp(wextension, "forms"))
	vcat = wgrapheme;
      else if (signlist_hack)
	ext = "signlist";
#endif
      struct sllext *ep = sllext(ext, strlen(ext));
      vcat = (char*)pool_alloc(strlen(ep->pre)+strlen(ep->pst)+strlen(wgrapheme)+1, sllpool);
      sprintf(vcat, "%s%s%s", ep->pre, wgrapheme, ep->pst);
    }
  
  printf("Content-type: text/html; charset=utf-8\n\n");
  printf("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
  printf("<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\"sux\" xml:lang=\"sux\">\n");
  printf("<head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n");
  printf("<title>OGSL Results for %s</title>\n", vcat);
  printf("<link rel=\"shortcut icon\" type=\"image/ico\" href=\"/favicon.ico\" />\n");
  printf("<link rel=\"stylesheet\" type=\"text/css\" href=\"/css/cbd.css\" />\n");
  printf("<script src=\"/js/cbd.js\" type=\"text/javascript\"><![CDATA[ ]]></script>\n");
  printf("<!-- Google tag (gtag.js) -->\n");
  printf("<script async=\"async\" src=\"https://www.googletagmanager.com/gtag/js?id=G-0QKC3P5HJ1\"></script>\n");
  printf("<script>\n");
  printf("  window.dataLayer = window.dataLayer || [];\n");
  printf("  function gtag(){dataLayer.push(arguments);}\n");
  printf("  gtag('js', new Date());\n\n");
  printf("  gtag('config', 'G-0QKC3P5HJ1');\n");
  printf("</script>\n");
  printf("</head><body>\n");

  if (wextension)
    {
      if (!strstr(wproject, "epsd2"))
	sll_open_frame_divs();
      printf("<h1 class=\"ogslres\">%s</h1>\n", vcat);
    }
}

static void
sll_html_trailer(void)
{
  printf("</body></html>\n");
}

static void
sll_html_p(const char *id, const char *sn, const char *v, const char *p)
{
  const char *vx = (v ? "&#xa0;=&#xa0;" : "");
  const char *pspan = "";
  if (p)
    {
      if (strstr(wproject, "epsd2"))
	pspan = "<br/>";
      else
	pspan = "<span class=\"ogsl-punct\">;</span>";
    }
  printf("<a href=\"javascript:showsign('%s','%s')\">%s<span class=\"sign\">%s</span></a>%s\n", wproject, id, vx, sn, pspan);
}

/* sn is always first; second item is either ID or a variable part
   which could be form name (U@c) or homophone (aₓ) */
static struct sllparts *
sll_split(const char *sc)
{
  char *s = (char*)pool_copy((uccp)sc, sllpool);
  static struct sllparts sp;
  memset(&sp, '\0', sizeof(struct sllparts));
  sp.sn = s; /* sn always in first position */
  while (*s && '\t' != *s)
    ++s;
  if ('\t' == *s)
    *s++ = '\0';
  while (*s)
    {
      if (*s == 'o' && isdigit(s[1]))
	sp.id = s;
      else
	sp.v = s;
      while (*s && '\t' != *s)
	++s;
      if (*s)
	*s++ = '\0';
    }
  return &sp;
}
