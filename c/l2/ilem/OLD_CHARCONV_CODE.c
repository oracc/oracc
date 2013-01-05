#ifdef  CHARCONV

/* This routine implements parsing of CFs which are a lite version of
   GDL Sumerian transliteration */
static unsigned char *
slow_lem_utf8(const unsigned char *lem, const char *f, ssize_t ln)
{
  unsigned char *uni, *dst;
  const unsigned char *r_start, *r_end, *dig_start, *dig_end, *res;
  
  dst = uni = malloc(6 * strlen(cc(lem)));
  while (lem)
    {
      if (*lem == '\\')
	++lem;

      /* implement modifier/allograph skipping here */
      while (*lem > 0x80 || (*lem && !isalpha(*lem)))
	*dst++ = *lem++;

      if (!*lem)
	break;

      r_start = lem;
      if (!(r_end = (unsigned char *)strpbrk(cc(r_start),".-&@~+\\")))
	r_end = lem + strlen(cc(lem));
      
      dig_end = r_end;

      while (isdigit(r_end[-1]))
	--r_end;

      if (r_end < dig_end)
	dig_start = r_end;
      else
	dig_start = dig_end;

      if (!(res = natf2utf2(r_start, r_end, NULL, f, ln)))
	{
	  free(uni);
	  return NULL;
	}
      else
	{
	  if (dig_end > dig_start)
	    {
	      if (!is_signlist(res))
		{
		  /* subdig() appends to the natf2utf buffer */
		  if (!(res = subdig(dig_start, dig_end)))
		    {
		      free(uni);
		      return NULL;
		    }
		  else
		    {
		      strcpy((char*)dst,cc(res));
		      dst += strlen(cc(dst));
		    }
		}
	      else
		{
		  strcpy((char*)dst,cc(res));
		  dst += strlen(cc(dst));
		  strncpy((char*)dst,(char*)dig_start,dig_end-dig_start);
		  dst += dig_end - dig_start;
		}
	    }
	  else
	    {
	      strcpy((char*)dst,cc(res));
	      dst += strlen(cc(dst));
	      if (dst[-1] == 'x')
		{
#define SUB_X_STR  "ₓ"
		  strcpy((char*)dst-1,SUB_X_STR);
		  dst += strlen(SUB_X_STR)-1;
		}
	    }
	}
      lem = dig_end;
    }
  *dst = '\0';
  return uni;
}

static unsigned char *
lem_utf8(const unsigned char *lem, const char *f, ssize_t ln)
{
  const unsigned char *uni;
  if (!lem)
    return NULL;

  if (strpbrk(cc(lem),"0123456789"))
    {
      unsigned char *slow_uni = slow_lem_utf8(lem, f, ln);
      if (slow_uni)
	{
	  unsigned char *ret = pool_copy(slow_uni);
	  free(slow_uni);
	  return ret;
	}
      else
	return pool_copy(lem);
    }
  else
    {
      uni = natf2utf2(lem,lem+strlen((const char *)lem), NULL, f, ln);
      if (uni)
	return pool_copy(uni);
      else
	return pool_copy(lem);
    }
}

#endif/*CHARCONV*/
