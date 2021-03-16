#include <string.h>
#include <wchar.h>
#include <wctype.h>
#include "libstemmer.h"
#include "atf.h"
#include "charsets.h"
#include "selib.h"
#include "mapaccent.c"

int use_unicode = 0;
struct sb_stemmer *stemmer = NULL;
extern FILE *f_mangletab;

FILE *
create_mangle_tab(const char *project, const char *index)
{
  return xfopen(se_file(project, index, "mangle.tab"), "w");
}

const unsigned char *
keymangler(const unsigned char *key, int manglerules, char *f, size_t l, struct est *estp, const char *prefix)
{
  const unsigned char *tmp;
  wchar_t*wtmp = NULL;
  size_t wlen;

  if (estp)
    est_add(key, estp);

  if ('x' == key[0] && '\0' == key[1])
    return key;
  
  if ((manglerules&KM_ATF2UTF) && !use_unicode)
    {
      /* FIXME: NEED TO MAKE THIS LANGUAGE-SENSITIVE */
      struct charset *cset = get_charset(c_qse,m_normalized);
      if (NULL == cset)
	{
	  fprintf(stderr,"keymangler: failed to set charset for c_qse\n");
	}
      else
	{
	  chartrie_init(cset);
	  if (manglerules&KM_GRAPHEME)
	    {
	      tmp = gatf2utf(key,f,l);
	    }
	  else
	    {
	      tmp = inctrie_map(cset->to_uni,(const char *)key,
				(const char *)(key+strlen((const char *)key)),0,
				NULL/*CHARSET_ERR_FUNC*/,cset, f, l);
	    }
	}
    }
  else
    tmp = key;

  if (!(wtmp = utf2wcs(tmp,&wlen)))
    return NULL;

  if (manglerules&KM_REDUCE)
    {
      wchar_t *wdst = wtmp, *wstart = wtmp;
      /* strip initial leading zeroes */
      while (*wtmp && '0' == *wtmp)
	++wtmp;
      while (*wtmp)
	{
	  if ((iswalnum(*wtmp) /* && *wtmp != 702 */) /* drop aleph !! NEW: KEEP ALEPH */
	      || (*wtmp >= 0x2080 && *wtmp <= 0x2089)
	      || (*wtmp == '-' && (manglerules&KM_HYPHOK))
	      || (*wtmp == '@'))
	    *wdst++ = *wtmp++;
	  else
	    {
	      ++wtmp;
	      /* we just skipped a non-alphanumeric character; see if
		 we there are leading zeroes to drop following it */
	      while (*wtmp && '0' == *wtmp)
		++wtmp;
	    }
	}
      *wdst = '\0';
      wtmp = wstart;
    }
  
  if (manglerules&KM_FOLD)
    {
      wchar_t *wdst = wtmp, *wstart = wtmp;
      while (*wtmp)
	{
	  if (iswupper(*wtmp))
	    *wdst++ = towlower(*wtmp++);
	  else
	    *wdst++ = *wtmp++;
	}
      *wdst = '\0';
      wtmp = wstart;
    }
  
  if (manglerules&KM_2VOWEL)
    {
      wchar_t *wdst = wtmp, *wstart = wtmp;
      while (*wtmp)
	{
	  if (*wtmp < sizeof(mapaccent) && mapaccent[*wtmp])
	    *wdst++ = mapaccent[*wtmp++];
	  else
	    *wdst++ = *wtmp++;
	}
      *wdst = '\0';
      wtmp = wstart;
    }
  
  tmp = wcs2utf(wtmp,wlen);

  if (manglerules&KM_STEM)
    tmp = sb_stemmer_stem(stemmer,(const sb_symbol*)tmp,strlen((const char *)tmp));

#if 0
  if (strcmp((const char*)key,(const char*)tmp))
    fprintf(stderr, "%s => %s\n", key, tmp);
  fprintf(stderr, "f_mangletab = %p\n", f_mangletab);
#endif

  if (prefix && f_mangletab && strcmp((const char*)key,(const char*)tmp))
#if 1
    fprintf(f_mangletab, "%s\t%s\n", key, tmp);
#else
    fprintf(f_mangletab, "%s:%s\t%s\n", prefix, key, tmp);
#endif
  
  return tmp;
}

void
km_use_stemmer()
{
  if (!stemmer)
    stemmer = sb_stemmer_new("en", NULL);
}
void
km_end_stemmer()
{
  sb_stemmer_delete(stemmer);
  stemmer = NULL;
}
