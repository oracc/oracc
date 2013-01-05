#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <wctype.h>
#include "atf.h"
extern int verbose;
#define warning2(f,l,t,s) fprintf(stderr,"%s:%d: " t "\n",f,l,s)

/* N.B.: The u_cdot entry in this array is deliberately empty
   because this is how cuneify handles the special CBD notation
   for Sumerian split bases, i.e., 'r<cdot>e'.  This is a hack,
   but a benign one.
 */
const char *const atf_strings[] =
  {
    "sz","SZ","s'","S'","s,","S,","t,","T,","'","`",
    "j","J","h","H",
    "0","1","2","3","4","5","6","7","8","9","x","x",
    "=a","=e","=i","=u",
    "=A","=E","=I","=U",
    "^a","^e","^i","^u",
    "^A","^E","^I","^U",
    "x",
    "{r}", "[#", "#]", "[#_", "_#]",
    "",
    NULL,
  };

#define atfstr(s) destp+=strlen(strcpy(destp,s))

extern const char *file;
/*int a_lnum = 0;*/

unsigned char *
wcs2atf(wchar_t *wc, size_t len)
{
  static char *dest = NULL;
  static int dest_alloced = 0;
  char *destp = NULL;

  if (!wc)
    {
      if (dest)
	{
	  free(dest);
	  dest_alloced = 0;
	  dest = NULL;
	}
      return NULL;
    }

  if ((1+(len*2)) >= dest_alloced)
    {
      while ((1+(len*2)) >= dest_alloced)
	dest_alloced += 1024;
      dest = realloc(dest,dest_alloced);
    }
  destp = dest;

  while (*wc)
    {
      if (*wc < 128)
	*destp++ = (char)*wc++;
      else
	switch(*wc++)
	  {
	  case U_cdot:
	    atfstr(atf_strings[u_cdot]);
	    break;
	  case U_sz:
	    atfstr(atf_strings[u_sz]);
	    break;
	  case U_SZ:
	    atfstr(atf_strings[u_SZ]);
	    break;
	  case U_s0:
	    atfstr(atf_strings[u_s0]);
	    break;
	  case U_s1:
	    atfstr(atf_strings[u_s1]);
	    break;
	  case U_s2:
	    atfstr(atf_strings[u_s2]);
	    break;
	  case U_s3:
	    atfstr(atf_strings[u_s3]);
	    break;
	  case U_s4:
	    atfstr(atf_strings[u_s4]);
	    break;
	  case U_s5:
	    atfstr(atf_strings[u_s5]);
	    break;
	  case U_s6:
	    atfstr(atf_strings[u_s6]);
	    break;
	  case U_s7:
	    atfstr(atf_strings[u_s7]);
	    break;
	  case U_s8:
	    atfstr(atf_strings[u_s8]);
	    break;
	  case U_s9:
	    atfstr(atf_strings[u_s9]);
	    break;
	  case U_s_x:
	    atfstr(atf_strings[u_s_x]);
	    break;
	  case U_eng:
	    atfstr(atf_strings[u_eng]);
	    break;
	  case U_ENG:
	    atfstr(atf_strings[u_ENG]);
	    break;
	  case U_heth:
	    atfstr(atf_strings[u_heth]);
	    break;
	  case U_HETH:
	    atfstr(atf_strings[u_HETH]);
	    break;
	  case U_sadhe:
	    atfstr(atf_strings[u_sadhe]);
	    break;
	  case U_SADHE:
	    atfstr(atf_strings[u_SADHE]);
	    break;
	  case U_sin:
	    atfstr(atf_strings[u_sin]);
	    break;
	  case U_SIN:
	    atfstr(atf_strings[u_SIN]);
	    break;
	  case U_tet:
	    atfstr(atf_strings[u_tet]);
	    break;
	  case U_TET:
	    atfstr(atf_strings[u_TET]);
	    break;
	  case U_aleph:
	    atfstr(atf_strings[u_aleph]);
	    break;
	  case U_amacr:
	    atfstr(atf_strings[u_amacr]);
	    break;
	  case U_emacr:
	    atfstr(atf_strings[u_emacr]);
	    break;
	  case U_imacr:
	    atfstr(atf_strings[u_imacr]);
	    break;
	  case U_umacr:
	    atfstr(atf_strings[u_umacr]);
	    break;
	  case U_Amacr:
	    atfstr(atf_strings[u_Amacr]);
	    break;
	  case U_Emacr:
	    atfstr(atf_strings[u_Emacr]);
	    break;
	  case U_Imacr:
	    atfstr(atf_strings[u_Imacr]);
	    break;
	  case U_Umacr:
	    atfstr(atf_strings[u_Umacr]);
	    break;
	  case U_acirc:
	    atfstr(atf_strings[u_acirc]);
	    break;
	  case U_ecirc:
	    atfstr(atf_strings[u_ecirc]);
	    break;
	  case U_icirc:
	    atfstr(atf_strings[u_icirc]);
	    break;
	  case U_ucirc:
	    atfstr(atf_strings[u_ucirc]);
	    break;
	  case U_Acirc:
	    atfstr(atf_strings[u_Acirc]);
	    break;
	  case U_Ecirc:
	    atfstr(atf_strings[u_Ecirc]);
	    break;
	  case U_Icirc:
	    atfstr(atf_strings[u_Icirc]);
	    break;
	  case U_Ucirc:
	    atfstr(atf_strings[u_Ucirc]);
	    break;
	  case U_times:
	    atfstr(atf_strings[u_times]);
	    break;
	  case U_mod_r:
	    atfstr(atf_strings[u_mod_r]);
	    break;
	  case U_ulhsq:
	    atfstr(atf_strings[u_ulhsq]);
	    break;
	  case U_urhsq:
	    atfstr(atf_strings[u_urhsq]);
	    break;
	  case U_llhsq:
	    atfstr(atf_strings[u_llhsq]);
	    break;
	  case U_lrhsq:
	    atfstr(atf_strings[u_lrhsq]);
	    break;
	  default:
	    warning2(file,lnum,"unicode U+%04x not allowed in ATF",(int)wc[-1]);
	    break;
	  }
    }
  *destp = '\0';
  return (unsigned char *)dest;
}

unsigned char *
wcs2utf(const wchar_t*ws, size_t len)
{
  static unsigned char *dest;
  static int dest_alloced = 0;
  size_t max = 1+(len*6);

  if (!ws)
    {
      if (dest)
	{
	  free(dest);
	  dest_alloced = 0;
	  dest = NULL;
	}
      return NULL;
    }

  if (max >= dest_alloced)
    {
      while (max >= dest_alloced)
	dest_alloced += 1024;
      dest = realloc(dest,dest_alloced);
    }
  if (((size_t)-1 == wcstombs((char*)dest,ws,max)))
    {
      fprintf(stderr,"wcs2utf: illegal wide character in wchar string\n");
      return NULL;
    }
  else
    return dest;
}

wchar_t*
wcs_lcase(wchar_t*ws)
{
  wchar_t *ws_orig = ws;
  while (*ws)
    {
      if (iswupper(*ws))
	*ws = towlower(*ws);
      ++ws;
    }
  return ws_orig;
}
wchar_t*
wcs_ucase(wchar_t*ws)
{
  wchar_t *ws_orig = ws;
  while (*ws)
    {
      if (iswlower(*ws))
	*ws = towupper(*ws);
      ++ws;
    }
  return ws_orig;
}

#define is_detchar(wc) ((wc) >= 0xe000 && (wc) <= 0xe031)

wchar_t*
wcs_undet(wchar_t*ws)
{
  wchar_t *ws_orig;
  for (ws_orig = ws; *ws; ++ws)
    {
      if (*ws >= 0xe000 && *ws <= 0xe019)
	*ws = (*ws - (0xe000-'a'));
      else if (*ws >= 0xe027 && *ws <= 0xe030)
	*ws = (*ws - (0xe027-0x2080));
      else
	{
	  switch (*ws)
	    {
	    case 0xe01a:
	      *ws = 0x00e0;
	      break;
	    case 0xe01b:
	      *ws = 0x00e1;
	      break;
	    case 0xe01c:
	      *ws = 0x00e8;
	      break;
	    case 0xe01d:
	      *ws = 0x00e9;
	      break;
	    case 0xe01e:
	      *ws = 0x00ec;
	      break;
	    case 0xe01f:
	      *ws = 0x00ed;
	      break;
	    case 0xe020:
	      *ws = 0x00f9;
	      break;
	    case 0xe021:
	      *ws = 0x00fa;
	      break;
	    case 0xe022:
	      *ws = 0x014b;
	      break;
	    case 0xe023:
	      *ws = 0x0161;
	      break;
	    case 0xe024:
	      *ws = 0x1e2b;
	      break;
	    case 0xe025:
	      *ws = 0x1e63;
	      break;
	    case 0xe026:
	      *ws = 0x1e6d;
	      break;
	    default:
	      fprintf(stderr,"detchars: unknown PUA character %lx\n", (long unsigned int)*ws);
	      break;
	    }
	}
    }
  return ws_orig;
}

size_t
count_detchars(wchar_t *ws)
{
  size_t n = 0;
  while (*ws)
    {
      if (is_detchar(*ws))
	++n;
      ++ws;
    }
  return n;
}

unsigned char *
remap_detchars(unsigned char *s)
{
  static wchar_t *ws = NULL;
  size_t nbytes;
  ws = utf2wcs(s,&nbytes);
  if (ws)
    {
      size_t ndet = count_detchars(ws);
      if (ndet)
	{
	  /* allocate enough space for every detchar to
	     be an independent determinative */
	  wchar_t *ndest = malloc((nbytes+(ndet*2)+1)*sizeof(wchar_t)),*dptr;
	  size_t utflen,utfconv;
	  unsigned char *ret;
	  if (!ndest)
	    {
	      fprintf(stderr,"detchars: out of memory\n");
	      exit(2);
	    }
	  dptr = ndest;
	  while (*ws)
	    {
	      while (*ws && !is_detchar(*ws))
		*dptr++ = *ws++;
	      if (is_detchar(*ws))
		{
		  wchar_t *detptr;
		  *dptr++ = '{';
		  detptr = dptr;
		  while (is_detchar(*ws))
		    *dptr++ = *ws++;
		  *dptr = 0x0;
		  (void)wcs_undet(detptr);
		  while (*ws == '#' || *ws == '*' || *ws == '?' || *ws == '!')
		    *dptr++ = *ws++;
		  *dptr++ = '}';
		  if (*ws == '\'') /* m'd => {m}{d} */
		    ++ws;
		}
	    }
	  *dptr = 0x0;
	  utf2wcs(NULL,0);
	  utflen = wcstombs(NULL,ndest,0);
	  ret = malloc(utflen+1);
	  utfconv = wcstombs((char*)ret,ndest,utflen+1);
	  if (utfconv != utflen)
	    {
	      fprintf(stderr,"detchars: conversion to utf-8 failed\n");
	      exit(2);
	    }
	  ndest[utflen] = '\0';
	  free(ndest);
	  if (verbose)
	    {
	      FILE *Z = fopen("/tmp/Z","w");
	      fputs((const char *)ret,Z);
	      fclose(Z);
	    }
	  return ret;
	}
    }
  return NULL;
}
