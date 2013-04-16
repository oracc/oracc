#include <ctype.h>
#include <wchar.h>
#include <wctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "atf.h"
#include "../ox/globals.h"

int
u_charbytes(const unsigned char *g)
{
  if (*g <= 127)
    return 1;
  else
    {
      size_t len = mbtowc(NULL,(const char *)g,6);
      if (len > 0)
	return len;
      else
	{
	  fprintf(stderr,"u_charbytes: mbtowc failed\n");
	  return 1; /* if a pointer is using this make sure it moves */
	}
    }
}

int
u_islower(const unsigned char *g)
{
  if (*g > 127)
    {
      size_t len = 0;
      wchar_t c1 = 0;
      c1 = utf1char(g,&len);
      if (c1 && len > 0)
	return iswlower(c1);
      else
	return 0;
    }
  else
    return islower(*g);
}
int
u_isupper(const unsigned char *g)
{
  if (*g > 127)
    {
      size_t len = 0;
      wchar_t c1 = 0;
      c1 = utf1char(g,&len);
      if (c1 && len > 0)
	return iswupper(c1);
      else
	return 0;
    }
  else
    return isupper(*g);
}

unsigned char *
utf2atf(const unsigned char *src)
{
  size_t mylen = 0;
  wchar_t *wc = utf2wcs(src,&mylen);
  if (wc)
    return wcs2atf(wc,mylen);
  else
    return NULL;
}

wchar_t
utf1char(const unsigned char *src,size_t *len)
{
  static wchar_t wc = 0;
  int ret = 0;
  size_t todo = 0;
  const unsigned char *s = src;
  while (*s > 127)
    ++s;
  todo = s - src;
  if (todo)
    {
      ret = mbtowc(&wc,(const char *)src,todo);
      if (ret < 0)
	{
	  int i;
	  fprintf(stderr,"%s:%d: bad utf8: mbtowc failed on:", 
		  file, atf_cbd_err ? cbd_err_line : lnum);
	  for (i = 0; i < todo; ++i)
	    fprintf(stderr," %x",src[i]);
	  fprintf(stderr,"\n");
	  if (len)
	    *len = (size_t)-1;
	  return 0;
	}
      else
	{
	  if (len)
	    *len = ret;
	  return wc;
	}
    }
  else
    {
      if (len)
	*len = 0;
      return 0;
    }
}
wchar_t*
utf2wcs(const unsigned char *src, size_t *len)
{
  static wchar_t *wdest = NULL;
  static int wdest_alloced = 0;
  size_t nbytes;
  /*Avoid gcc complaining about dereferencing type-punned pointer in mbstowcs call*/
  static const char *psrc;

  if (!src)
    {
      if (wdest)
	{
	  free(wdest);
	  wdest = NULL;
	  wdest_alloced = 0;
	}
      return NULL;
    }
  else
     psrc = (const char *)src;

  if ((1+strlen((const char *)src)) >= wdest_alloced)
    {
      while ((1+strlen((const char *)src)) >= wdest_alloced)
	wdest_alloced += 1024;
      wdest = realloc(wdest, wdest_alloced * sizeof(wchar_t));
    }

  nbytes = mbsrtowcs(wdest,&psrc,strlen((const char *)src),NULL);

  if (nbytes == (size_t)-1)
    {
      fprintf(stderr,"%d: conversion of '%s' from UTF-8 to wide chars failed\n", 
	      (atf_cbd_err ? cbd_err_line : lnum), src);
      return NULL;
    }

  wdest[nbytes] = 0;
  if (len)
    *len = nbytes;
  return wdest;
}

const unsigned char *
utf_lcase(const unsigned char *s)
{
  size_t len;
  wchar_t *ws = utf2wcs(s,&len);
  if (ws)    
    return wcs2utf(wcs_lcase(ws),len);
  else
    return NULL;
}
const unsigned char *
utf_ucase(const unsigned char *s)
{
  size_t len;
  wchar_t *ws = utf2wcs(s,&len);
  if (ws)    
    return wcs2utf(wcs_ucase(ws),len);
  else
    return NULL;
}
