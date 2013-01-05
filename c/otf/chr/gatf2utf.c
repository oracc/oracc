/* This routine implements parsing of CFs which are a lite version of
   GDL Sumerian transliteration */
#define is_vowel(x) \
  (tolower(x)=='a'     \
    || tolower(x)=='e' \
    || tolower(x)=='i' \
    || tolower(x)=='o' \
    || tolower(x)=='u')

#undef cc
#define cc(_str) (const char*)_str

static int
has_vowel(const unsigned char *from, const unsigned char *to)
{
  while (from < to)
    if (is_vowel(*from))
      return 1;
    else
      ++from;
  return 0;
}

unsigned char *
gatf2utf(const unsigned char *lem, const char *f, ssize_t ln)
{
  static struct charset *cset = NULL;
  unsigned char *uni, *dst;
  const unsigned char *r_start, *r_end, *dig_start, *dig_end, *res;
  
  if (cset == NULL)
    {
      cset = get_charset(c_qse,m_normalized);
      chartrie_init(cset);
    }

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
	{
	  if (has_vowel(r_start, r_end))
	    dig_start = r_end;
	  else
	    r_end = dig_start = dig_end;
	}
      else
	dig_start = dig_end;

      if (!(res = /*natf2utf2(r_start, r_end, NULL, f, ln)*/
	    inctrie_map(cset->to_uni,
			(const char*)r_start,
			(const char*)r_end,0,
			CHARSET_ERR_FUNC,cset,f,ln)))
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
		  strncpy((char*)dst,(const char*)dig_start,dig_end-dig_start);
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
