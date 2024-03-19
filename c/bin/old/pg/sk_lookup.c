static int
sk_lookup(const char *k)
{
  static char field[128];
  char *f = field;
  int i;
  int vbar = 0;

 retry:
  {
    while (*k && *k != ',' && *k != '|')
      *f++ = *k++;
    *f = '\0';
    vbar = '|' == *k;
        
    for (i = 0; i < sip->nmapentries; ++i)
      if (!strcmp(field,(const char*)sip->fields[i].n))
	break;
    
    if (i == sip->nmapentries)
      {
	if (vbar)
	  {
	    ++k;
	    f = field;
	    goto retry;
	  }
	else
	  {
	    fprintf(stderr,"pg: field %s not in fields list\n", field);
	    exit(1);
	  }
      }
    /* fprintf(stderr, "pg: found field %s\n", field); */
    return sip->fields[i].field_index;
  }
}
