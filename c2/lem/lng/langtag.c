#include <stdlib.h>
#include <ctype128.h>
#include "lng.h"
#include "pool.h"
#include "hash.h"
#include "mesg.h"

const char *fwhost = NULL;

struct lang_tag *default_langtag;

static Hash *langtag_hash;
static Hash *texttag_hash = NULL;
static Hash *noscript_hash = NULL;
static char *texttag_buf = NULL;
static Pool *langtag_pool;
static void langtag_error(const char *file, int lnum, 
			  const char *tag, const char *mess);
static const char *langtag_atf(const char *atf,
			       const char *file, int lnum);
static struct lang_tag *langtag_parse(const char *tag, 
				      const char *file, int lnum);

static int
all_alpha(const char *s)
{
  while (*s && isalpha(*s))
    ++s;
  return !*s;
}

static int
all_digit(const char *s)
{
  while (*s && isdigit(*s))
    ++s;
  return !*s;
}

static int
all_lower(const char *s)
{
  while (*s && islower(*s))
    ++s;
  return !*s;
}

static int
valid_lang(const char *s)
{
  return s && *s && (strlen(s) == 2 || strlen(s) == 3) && all_lower(s);
}

static int
valid_script(const char *s)
{
  return !s || !*s || ((strlen(s) == 4 && isupper(*s) && all_lower(s+1))
		       || (strlen(s) == 3 && all_digit(s)));
}

static int
valid_x(const char *s)
{
  return !s || !*s || (*s == 'x' && s[1] == '-' 
		       && strlen(&s[2]) >= 1 && strlen(&s[2]) < 9
		       && all_alpha(&s[2]));
}

void
langtag_init(void)
{
  langtag_pool = pool_init();
  langtag_hash = hash_create(1);
  default_langtag = langtag_create(NULL,"sux",NULL,NULL,0);
  noscript_hash = hash_create(1);
  texttag_init();
}

void
langtag_term(void)
{
  pool_term(langtag_pool);
  if (langtag_hash)
    {
      hash_free(langtag_hash, hash_xfree);
      langtag_hash = NULL;
    }

  if (noscript_hash)
    hash_free(noscript_hash, NULL);

  /*  free(default_langtag); */
}

static void
texttag_cat(char *tag,void *ignored)
{
  if (*texttag_buf)
    strcat(texttag_buf," ");
  strcat(texttag_buf,tag);
}

void
texttag_init(void)
{
  texttag_term();
  texttag_hash = hash_create(1);
}

/* Caller is responsible for freeing this */
char *
texttag_langs(void)
{
  if (texttag_hash->key_count)
    {
      texttag_buf = malloc(texttag_hash->key_count * 64);
      *texttag_buf = '\0';
      hash_exec2(texttag_hash,(hash_exec2_func*)texttag_cat);
      return texttag_buf;
    }
  else
    return NULL;
}

char *
tag_no_script(const char *tag)
{
  char *tmp = NULL, *script;

  if (tag)
    {
      char *ret = hash_find(noscript_hash,tag);
      if (ret)
	return ret;
      
      tmp = malloc(strlen(tag)+1);
      strcpy(tmp,tag);
      script = tmp + strlen(tmp);
      while (script > tmp && isdigit(script[-1]))
	--script;
      if (script > tmp && '-' == script[-1])
	script[-1] = '\0';
      hash_add(noscript_hash, pool_copy(tag,langtag_pool), (ret = pool_copy(tmp,langtag_pool)));
      free(tmp);
      return ret;
    }

  return NULL;
}

void
texttag_register(const char *tag)
{
  static int defined = 1;
  char *tmp = tag_no_script(tag);
  if (tmp)
    {
      if (!hash_find(texttag_hash, (unsigned char *)tmp))
	hash_add(texttag_hash, (unsigned char *)tmp, &defined);
      /* NEED TO FREE tmp SOMEHOW */
    }
}

void
texttag_term(void)
{
  if (texttag_hash)
    {
      hash_free(texttag_hash,NULL);
      texttag_hash = NULL;
    }
}

static struct lang_tag *
langtag_parse(const char *tag, const char *file, int lnum)
{
  struct lang_tag *tmp = NULL; 
  char *tagcopy = NULL;
  char *s = NULL;

  if (!tag || !*tag)
    {
      langtag_error(file,lnum,tag,"malformed language tag");
      return NULL;
    }
  else
    {
      tmp = calloc(1,sizeof(struct lang_tag));
      tagcopy = (char*)pool_copy((unsigned char *)tag, langtag_pool);
    }
  tmp->tag = (char *)pool_copy((unsigned char *)tag, langtag_pool);
  tmp->lang = s = tagcopy;
  while (*s && '-' != *s)
    ++s;
  while (*s)
    {
      if ('-' == *s)
	{
	  *s++ = '\0';
	  if ('x' == *s)
	    {
	      tmp->xlang = s;
	      if ('-' == s[1])
		s += 2;
	    }
	  else if (isdigit(*s)) /* by definition all Oracc script tags are numeric */
	    tmp->script = s;
	  else
	    {
	      langtag_error(file,lnum,tag,"bad language tag: expected format abc-x-abcdef-123");
	      break;
	    }
	}
      while (*s && '-' != *s)
	++s;	
    }

  if (!valid_lang(tmp->lang))
    {
      langtag_error(file,lnum,tag,"lang is required and must be 2 or 3 letters");
      free(tmp);
      tmp = NULL;
    }
  else if (!valid_x(tmp->xlang))
    {
      langtag_error(file,lnum,tag,"extension must be `x-' then up to 8 letters");
      free(tmp);
      tmp = NULL;
    }
  else if (!valid_script(tmp->script))
    {
      langtag_error(file,lnum,tag,"script must be 4 letters (Abcd) or 3 digits");
      free(tmp);
      tmp = NULL;
    }

  if (tmp && tmp->xlang && !strcmp(tmp->xlang, "x-syllabic"))
    {
      tmp->xlang = NULL;
      tmp->script = "947";
    }
  else if (tmp && tmp->xlang && !strcmp(tmp->xlang, "x-udgalnun"))
    {
      tmp->xlang = NULL;
      tmp->script = "948";
    }

  return tmp;
}

static void
langtag_error(const char *file, int lnum, const char *tag, const char *mess)
{
#if 1
  if (file && *file)
    mesg_vwarning(file, lnum, "%s: %s\n", tag, mess);
  else
    mesg_warning("%s: %s\n", tag, mess);
#else
  if (file && *file)
    fprintf(stderr,"%s:%d: %s: %s\n", file, lnum, tag, mess);
  else
    fprintf(stderr,"%s: %s\n", tag, mess);
#endif
}

static const char *
langtag_atf(const char *atf, const char *file, int lnum)
{
  struct langatf *atf_lang = NULL;

  if ((atf_lang = lang_atf(atf,strlen(atf))))
    {
      if (!strcmp(atf_lang->lang,"-"))
	{
	  if ('0' == *atf || '1' == *atf)
	    langtag_error(file,lnum,"%0 and %1 are deprecated",atf);
	  else
	    langtag_error(file,lnum,"unhandled atflang tag %s",atf);
	  return NULL;
	}
      else
	return atf_lang->lang;
    }
  else
    return NULL;
}

const char *
langtag_compose(struct lang_tag *def, const char *lang,
		const char *xlang, const char *script, const char *x)
{
  char buf[128];
  if (lang)
    strcpy(buf,lang);
  else
    strcpy(buf,def->lang);
  if (xlang)
    sprintf(buf+strlen(buf),"-%s",xlang);
  else if (def->xlang)
    sprintf(buf+strlen(buf),"-%s",def->xlang);
  if (script)
    sprintf(buf+strlen(buf),"-%s",script);
  else if (def->script)
    sprintf(buf+strlen(buf),"-%s",def->script);
  if (x)
    sprintf(buf+strlen(buf),"-%s",x);
  else if (def->x)
    sprintf(buf+strlen(buf),"-%s",def->x);
  return (char *)pool_copy((unsigned char *)buf,langtag_pool);
}

struct lang_tag *
langtag_create(struct lang_context *base, const char *tag, 
	       int *taglenp, const char *file, int lnum)
{
  struct lang_tag *ret = NULL;
  const char *tagend = NULL, *slash = NULL, *atf = NULL;
  const char *fulltag = NULL;
  int percent = 0, taglen = 0;
  static char tagbuf[64];

  if (!tag)
    return NULL;

  if (*tag == '%')
    {
      ++tag;
      ++percent;
    }

  for (tagend = tag; isalnum(*tagend) || '/' == *tagend || '-' == *tagend; ++tagend)
    if ('/' == *tagend)
      slash = tagend;

  taglen = (slash ? (slash-tag) : (tagend-tag));
  strncpy(tagbuf,tag,taglen);
  tagbuf[taglen] = '\0';

  if (taglen == 2 && !strcmp(tagbuf, "fw"))
    {
      if (base)
	fwhost = base->core->name;
      else
	fwhost = NULL;
      *taglenp = percent + taglen;
      return NULL;
    }

  if ((atf = langtag_atf(tagbuf,file,lnum)))
    {
      strcpy(tagbuf,atf);
    }

  if (slash)
    {
      ++slash;
      if ('g' == *slash)
	strcat(tagbuf, "-020");
      else if ('n' == *slash)
	strcat(tagbuf, "-949");
      else
	langtag_error(file,lnum,"%s: bad writing system tag, must be 'g' or 'n'",slash);
      taglen += 2;
    }

  if (base)
    {
      struct lang_tag *lt = langtag_parse(tagbuf, file, lnum);
      if (lt)
	{
	  langtag_inherit(lt,base->tag);
	  fulltag = langtag_compose(lt,NULL,NULL,NULL,NULL);
	  if (!(ret = hash_find(langtag_hash,(unsigned char*)fulltag)))
	    {
	      if ((ret = langtag_parse(fulltag, file, lnum)))
		hash_add(langtag_hash, (unsigned char *)ret->tag, ret);
	      else
		ret = hash_find(langtag_hash,(unsigned char *)"sux");
	    }
	  free(lt);
	}
    }
  else
    {
      if (!(ret = hash_find(langtag_hash,(unsigned char*)tagbuf)))
	{
	  if ((ret = langtag_parse(tagbuf, file, lnum)))
	    {
	      struct langcore *lc = langcore_of(ret->lang);
	      if (lc && !ret->script)
		ret->script = lc->script;
	      hash_add(langtag_hash, (unsigned char *)ret->tag, ret);
	    }
	  else
	    ret = hash_find(langtag_hash,(unsigned char *)"sux");
	}
    }

  if (taglenp)
    *taglenp = percent + taglen;

  return ret;
}

/* eliminating script inheriting means that %akk means %akk-020;
   this seems better than allowing %akk to default to %akk-949 */
void
langtag_inherit(struct lang_tag *to, struct lang_tag *from)
{
  if (!strcmp(to->lang,from->lang))
    {
      if (!to->xlang && strcmp(to->lang, "sux"))
	to->xlang = from->xlang;
#if 0
      if (!to->script)
	to->script = from->script;
#endif
    }
}
