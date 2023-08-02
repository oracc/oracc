unsigned char *
ucode2utf8(unsigned const char *ucode)
{
  int nchars = 1, last = 0, nhw = 0;
  wchar_t *wp = NULL;
  unsigned char *utmp = strdup(ucode), *s, *t;
  for (s = utmp; *s; ++s)
    if ('.' == *s)
      ++nchars;
  wp = malloc((nchars+1) * sizeof(wchar_t));
  for (s = utmp; *s; ++s)
    {
      t = s;
      while (*s && '.' != *s)
	++s;
      if (*s)
	*s = '\0';
      else
	last = 1;
      if ('0'==t[0] && 'x'==t[1])
	t += 2;
      else if ('x' == t[0])
	++t;
      hw[nhw++] = (wchar_t)hex2int(t);
      if (last)
	break;
    }
  hw[nhw] = L'\0';  
  s = pool_copy(wcs2ucs(hw, nhw), pool);
  free(hw);
  return s;
}
