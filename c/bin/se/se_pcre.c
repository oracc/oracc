#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype128.h>
#include <psd_base.h>
#include <list.h>
#include <pcre.h>
#include "selib.h"

#define OVECCOUNT 3

List *apply_pcre(const char *pat,const char*sub,size_t len);

static unsigned char *glist_buf, *glist_begin;
static size_t glist_buf_len, glist_len;

#ifndef strdup
extern char *strdup(const char*);
#endif

void
se_pcre_init(const char *project, const char *index)
{
  static int initialized = 0;
  static char *iproject = NULL, *iindex = NULL;
  FILE *fp;
  static char glist_fname[_MAX_PATH];

  if (!project || !*project)
    project = "cdli";

  if (initialized && !strcmp(iproject,project) && !strcmp(iindex,index))
    return;
  if (iproject)
    free(iproject);
  if (iindex)
    free(iindex);
  iproject = strdup(project);
  iindex = strdup(index);
  initialized = 1;

  strcpy(glist_fname, se_file(project, index, "key.lst"));
  xaccess (glist_fname, R_OK, TRUE);
  glist_buf_len = fsize_t (glist_fname, NULL);
  glist_buf = malloc(glist_buf_len+3);
  *glist_buf = '\n';
  fp = xfopen (glist_fname, "rb");
  xfread (glist_fname, TRUE, &glist_buf[1], 1, glist_buf_len, fp);
  xfclose (glist_fname, fp);
  ++glist_buf_len;
  glist_buf[glist_buf_len++] = '\n';
  glist_buf[glist_buf_len++] = '\0';
  for (glist_begin = glist_buf+1; '#' == *glist_begin; ++glist_begin)
    {
      while ('\n' != *glist_begin)
	++glist_begin;
    }
  glist_len = glist_buf_len - (glist_begin - glist_buf);
}

List *
se_pcre(const char *pattern)
{
  const char *dotstar;
  if (!pattern || !*pattern)
    return NULL;

  if ((dotstar = strstr(pattern, ".*")))
    {
      /*
       * Supress silly patterns like ".*" or "[ae].*".  We should
       * do something more sophisticated, but this is a start.
       */
      const char *pat = pattern;
      if (*pat == '[')
	{
	  const char *sqb_end;
	  while (']' != *pat)
	    ++pat;
	  sqb_end = pat;
	  ++pat;
	  while (isalnum(*pat))
	    ++pat;
	  if (pat - sqb_end < 2)
	    return NULL;
	}
      else
	{
	  while (isalnum(*pat))
	    ++pat;
	  if (pat - pattern < 2)
	    return NULL;
	}
    }

  return apply_pcre(pattern,(const char *)glist_buf,glist_len);
}

List *
apply_pcre(const char *pattern, const char *subject, size_t sublen)
{
  pcre *re;
  const char *errorp;
  int erroffset;
  int ovector[OVECCOUNT];
  int subject_length = sublen;
  int rc, next_start;
  List *match_list = list_create(LIST_SINGLE);

  re = pcre_compile(pattern, PCRE_MULTILINE|PCRE_UTF8, &errorp, &erroffset, NULL);
  if (re == NULL)
    {
      fprintf(stderr,"PCRE compilation failed at offset %d: %s\n", erroffset, errorp);
      return NULL;
    }
  list_add(match_list, (void*)pattern); /* back compat w/ sf_regex -- to go one day */
  list_add(match_list, (void*)pattern);
  next_start = 0;
  while (next_start < subject_length)
    {
      rc = pcre_exec(re, NULL, 
		     subject, 
		     subject_length,
		     next_start, 0, ovector, OVECCOUNT);

      if (rc == PCRE_ERROR_NOMATCH)
	break;
      else if (rc < 0)
	{
	  fprintf(stderr, "matching error %d\n", rc);
	  return NULL;
	}
      else if (rc == 0)
	{
	  rc = OVECCOUNT/3;
	  fprintf(stderr, "ovector only has room for %d captured substrings\n", rc - 1);
	}
      else
	{
	  char match_buf[1024], *mp;
	  const char *subject_rover = subject + ovector[0];
	  int match_begin;
	  if (subject_rover[-1] == '\n'
	      && (subject[ovector[1]] == '\t'
		  || subject[ovector[1]] == '\n'))
	    {
	      while (*subject_rover && *subject_rover != '\n')
		++subject_rover;
	      next_start = subject_rover - subject;
	      subject_rover = subject + ovector[0];
#if 0
	      while (subject_rover > subject
		     && subject_rover[-1] != '\n'
		     && subject_rover[-1] != '\t')
		--subject_rover;
#endif
	      match_begin = subject_rover - subject;
	      *match_buf = '\0';
	      mp = match_buf;
	      while (match_begin < next_start)
		*mp++ = subject[match_begin++];
	      *mp = '\0';
	      progress("re: adding match %s\n", match_buf);
	      list_add(match_list, xstrdup(match_buf));
	    }
	  else
	    {
	      while  ('\n' != *subject_rover)
		++subject_rover;
	      next_start = subject_rover - subject;
	      
	    }
	}
    }
  return match_list;
}
