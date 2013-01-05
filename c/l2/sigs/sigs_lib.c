#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static const unsigned char *
_sigs_find(const unsigned char *lp, const unsigned char *chars)
{
  while (*lp)
    {
      size_t n = strcspn((char*)lp,(const char*)chars);
      const unsigned char *tmp = lp + n;
      if (!*tmp) /* end is terminating null */
	return tmp;
      else if (*tmp != '\\'
	       || (tmp[1] != '&' && tmp[1] != '(' && tmp[1] != '|'))
	/* char is not an escape; end is the special char */
	return tmp;
      else
	lp = tmp+1; /* char was escaped; look for next instance */
    }
  return lp;
}

const unsigned char *
sigs_field_begin(const unsigned char *lp, const unsigned char *field)
{
  if (field[1])
    {
      if ('#' == field[0] && '#' == field[1])
	{
	  const unsigned char *tmp = (unsigned char *)lp;
	  while (1)
	    {
	      if ((tmp = _sigs_find(tmp, (const unsigned char *)"#")))
		{
		  if (tmp[1] == '#')
		    return tmp;
		  else
		    ++tmp;
		}
	      else
		return NULL;
	    }
	}
      else
	{
	  fprintf(stderr, "sigs_field_begin: internal error: illegal field `%s'\n",field);
	  return NULL;
	}
    }
  else
    {
      lp = _sigs_find(lp,field);
      if (lp && *lp)
	return lp;
      else
	return NULL;
    }
}

const unsigned char *
sigs_field_end(const unsigned char *lp)
{
  return _sigs_find(lp, (const unsigned char *)"$#/+@");
}
