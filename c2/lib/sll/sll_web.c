

struct sllparts *
sll_split(const char *sc)
{
  char *s = strdup(sc);
  while (*s)
    {
      if (*s == 'o' && isdigit(s[1]))
	id = s;
      else if (sll_has_sign_indicator(*s))
	sn = s;
      else
	v = s;
      while (*s && '\t' != *s)
	++s;
      if (*s)
	*s++ = '\t';
    }
}

void
sll_uri_output(List *lp)
{
  if (wextension)
    {
      const char *v = NULL;
      const char *first_id = NULL;
      int i = 0;
      sll_html_header();
      for (v = list_first(lp); v; v = list_next(lp))
	{
	  const char *p = (i++ ? "; " : "");
		  
#if 0
	  /* In the perl version this line eliminated duplicates like a₆ occurring 4 times because of the U variants */
	  next if $known{$s}++;
#endif
	  if (!first_id)
	    first_id = oid;
	  sll_html_p(oid,sn,v,p);
	}
      if (!strstr(project, "epsd2"))
	sign_frame(first_id);
      html_trailer();
    }
  else
    {
      html_header();
      sign_frame(oid);
      html_trailer();
    }
}
