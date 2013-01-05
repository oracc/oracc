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
	      fprintf(stderr,"detchars: unknown PUA character %lx\n", *ws);
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
	  dptr = ws;
	  while (*ws)
	    {
	      while (!is_detchar(*ws))
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
	  return ret;
	}
    }
  return NULL;
}
