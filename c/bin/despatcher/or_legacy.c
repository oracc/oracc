static void
legacy_urls(void)
{
  if (!strcmp(elements[0], "rpc") || 
      (!strcmp(elements[0], "cgi-bin") 
       && elements[1] 
       && (!strcmp(elements[1], "rpc")
	   || !strcmp(elements[1], "rpc.plx"))))
    {
      execl("/usr/bin/perl", "perl", "/usr/local/oracc/bin/rpc.plx", NULL);
      do404();
    }
  else if (!strcmp(elements, "atf"))
    {
      execl("/usr/bin/perl", "perl", "/usr/local/oracc/bin/atf.plx", "atf.plx", NULL);
      do404();
    }
}
