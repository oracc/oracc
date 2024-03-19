extern int l2;

static unsigned char *
adjust_s(unsigned char *stop, unsigned char *s)
{
  unsigned char *t = s;
  while (t > stop && isspace(t[-1]))
    --t;
  if (isspace(*t))
    *t = '\0';
  while (isspace(s[1]))
    ++s;
  return s;
}

static int
is_lang_id(const char *s)
{
  while (isalnum(*s) || '-' == *s)
    ++s;
  if (*s == '.')
    ++s;
  return 'x' == *s;
}

struct item *
xis_load(int *nitems)
{
  unsigned char *buf = NULL, *s;
  size_t buflen;
  if (listfile)
    buf = loadfile((unsigned const char *)listfile,&buflen);
  else
    buf = loadstdin(&buflen);
  s = buf;
  s = buf;
  /* we know that each entry takes up at least 8 bytes */
  items = malloc(((buflen / 8) + 1) * sizeof(struct item));
  while (s - buf < buflen)
    {
      char *colon = NULL, *dot;
      unsigned char *orig_s = s;
      if (l2)
	{
	  /* if the list is qualified, pg can ignore the
	     project parts and just look up the items by
	     ID because it is only using the project's
	     own sortinfo */
	  colon = strchr((char*)s,':');
	  if (colon)
	    {
	      orig_s = s;
	      s = (unsigned char *)++colon;
	    }
	}
      if (*s == 'P' || *s == 'Q' || *s == 'X' || is_lang_id((char *)s))
	{
	  items[items_used].s = orig_s;
	  while (*s && '\n' != *s)
	    ++s;
	  s = adjust_s(buf,s);
	  *s++ = '\0';
#if 0
	  if (colon)
	    {
	      if ((dot = strchr((const char*)items[items_used].qpq,'.')))
		*dot = '\0';
	    }
#endif
	  items[items_used].pq = (unsigned char*)strdup(colon 
							? colon
							: (char*)items[items_used].s);
	  if ((dot = strchr((const char *)items[items_used].pq,'.')))
	    {
	      *dot++ = '\0';
	      items[items_used].lkey = atoi(dot);
	      /* fprintf(stderr,"scanned lkey value %d from %s\n",items[items_used].lkey,s); */
	    }
	  else
	    items[items_used].lkey = 0;
	  ++items_used;
	}
      else
	{
	  fprintf(stderr,"pg: bad list `%s'\n",listfile);
	  exit(1);
	}
    }
  *nitems = items_used;
  return items;
}
