static int
count_entries(const char *tmp, const char *option)
{
  int i;
  for (i = 1; *tmp; ++tmp)
    {
      if (',' == *tmp)
	{
	  ++i;
	  while (isspace(tmp[1]))
	    ++tmp;
	  if (',' == tmp[1])
	    {
	      fprintf(stderr, "ce_xmd2: %s/00lib/config.xml: empty field in `%s'\n",
		      project, option);
	      while (isspace(tmp[1]) || ',' == tmp[1])
		++tmp;
	    }
	}
    }
  return i;
}

static void
set_entries(const char **entries, const char *option)
{
  int i;
  char *tmp;

  tmp = malloc(strlen(option)+1);
  (void)strcpy(tmp, option);
  for (i = 0, entries[0] = tmp; *tmp; )
    {
      if (',' == *tmp)
	{
	  *tmp++ = '\0';
	  while (isspace(tmp[1]))
	    ++tmp;
	  if (',' == tmp[1])
	    {
	      fprintf(stderr, "ce_xmd2: %s/00lib/config.xml: empty field in `%s'\n",
		      project, option);
	      while (isspace(tmp[1]) || ',' == tmp[1])
		++tmp;
	    }
	  if (*tmp)
	    entries[++i] = tmp;
	}
      else
	++tmp;
    }
  entries[++i] = NULL;
}
