#include <sys/unistd.h>
#include <ctype128.h>
#include "hash.h"
#include "npool.h"
#include "runexpat.h"
#include "sas.h"
#include "gsl.h"

#if 0
#include "xvalue.c"
#endif

#ifndef strdup
#define strdup(a) (unsigned char *)strcpy(malloc(strlen((const char *)a)+1),(const char *)a)
#endif

static int psl_initialized = 0;

#if 1
static const char *sl_fn = "/home/oracc/xml/ogsl/ogsl-sl.xml";
#else
static const char *sl_fn = "/home/oracc/lib/data/ogsl.xml";
#endif

static Hash_table *psl, *psl_id, *psl_v, *cuneify_tab, *hex_tab;
static unsigned char *sign_sn,*curr_sn = NULL,*curr_unitary_sn = NULL, *curr_form = NULL, *curr_utf8;
static struct npool *psl_pool;

static unsigned char *sname_buf;
static int sname_buf_alloced;

static int deprecated_sign = 0;

static unsigned char *sname_append(const unsigned char *sn,const unsigned char *app);

static int
deprecated(const char *type,const char **atts)
{
  const char *d = (const char *)findAttr(atts,"deprecated");
  if (d && (!strcmp(d,"1") || !strcmp(d,"yes")))
    {
      if (!strcmp(type,"sign"))
	deprecated_sign = 1;
      return 1;
    }
  return 0;
}

#if 0
static unsigned char *
local_v(unsigned char *v)
{
  static unsigned char buf[128];
  if ((strlen((char*)curr_form) + strlen((char*)v) + 3) > 128) {
    abort();
  } else {
    sprintf((char*)buf,"%s(%s)",v,curr_form);
  }
  return buf;
}
#endif

static void
sH(void *userData, const char *name, const char **atts)
{
  static const char *curr_xid = NULL;
  static const char *n = "";
  if (!strcmp(name,"sign"))
    {
      if (!deprecated("sign",atts))
	{
	  sign_sn = curr_unitary_sn = curr_sn = npool_copy((unsigned char *)findAttr(atts,"n"),psl_pool);
	  curr_xid = (char*)npool_copy((const unsigned char *)get_xml_id(atts), psl_pool);
	  hash_add(psl,curr_sn,(void*)n);
	  hash_add(psl_id,curr_sn,(char *)curr_xid);
	}
    }
  else if (!strcmp(name,"form"))
    {
      const char *utf8 = findAttr(atts,"utf8"), *form_xid;
      curr_unitary_sn = curr_form = npool_copy((unsigned char *)findAttr(atts,"n"),psl_pool);
      curr_xid = form_xid = (char*)npool_copy((const unsigned char *)get_xml_id(atts), psl_pool);
      if (!form_xid || !*form_xid)
	curr_xid = form_xid = (char*)npool_copy((unsigned char *)findAttr(atts,"ref"),psl_pool);

      if (!hash_find(psl,curr_form))
	{
	  hash_add(psl,curr_form,(void*)n);
	  if (form_xid)
	    hash_add(psl_id,curr_form,(char*)form_xid);
	}

#if 0
      curr_sn = npool_copy(curr_form,psl_pool);
#else
      curr_sn = npool_copy(sname_append(curr_form,sign_sn),
			   psl_pool);
#endif
      hash_add(psl,curr_sn,&n);
      if (*utf8)
	hash_add(cuneify_tab,
		 npool_copy((unsigned char *)findAttr(atts,"n"),psl_pool),
		 npool_copy((unsigned char *)utf8,psl_pool));

    }
  else if (!strcmp(name,"list"))
    {
      if (curr_sn)
	{
	  const unsigned char *n = npool_copy((unsigned char *)findAttr(atts,"n"),psl_pool);
	  hash_add(psl, n, curr_sn);
	  hash_add(psl_id, n, (char*)curr_xid);
	}
    }
  else if (!strcmp(name,"v"))
    {
      if (!deprecated_sign && !deprecated("value",atts))
	{
	  unsigned char *sn = NULL;
	  const char *utf8 = findAttr(atts,"utf8");
	  unsigned char *curr_val = npool_copy((unsigned char *)findAttr(atts,"n"),psl_pool);
	  hash_add(psl_id,curr_val,(void*)curr_xid);
	  if (curr_form)
	    {
	      /* If this value is not known yet allow it as the default */
	      if (!(sn = hash_find(psl_v,curr_val)))
		{
		  if (!*utf8 && curr_utf8)
		    utf8 = (char*)curr_utf8;
		  hash_add(psl_v, curr_val,
			   npool_copy(psl_bounded_sname(curr_sn),psl_pool));
		  if (*utf8)
		    hash_add(cuneify_tab,
			     npool_copy((unsigned char *)findAttr(atts,"n"),psl_pool),
			     npool_copy((unsigned char *)utf8,psl_pool));
		}
	    }
	  if (!(sn = hash_find(psl_v,curr_val)))
	    {
	      hash_add(psl_v, curr_val,
		       npool_copy(psl_bounded_sname(curr_sn),psl_pool));
	      if (*utf8)
		{
		  if (!(hash_find(cuneify_tab, curr_val)))
		    {
		      hash_add(cuneify_tab, curr_val,
			       npool_copy((unsigned char *)utf8,psl_pool));
		    }
		}
	    }
#if 0
	  /* If curr_val already has a sign name don't overwrite that;
	     the form qualification has to be used to access values
	     that are duplicated in sign and form */
	  else
	    {
	      hash_add(psl_v,curr_val,
		       npool_copy(psl_bounded_sname(sname_append(curr_sn,sn)),
				  psl_pool));
	    }
#endif
	}
    }
  else if (!strcmp(name,"utf8"))
    {
      const char *h = findAttr(atts, "hex");
      /* fprintf(stderr, "%s => %s\n", h, sign_sn); */
      hash_add(hex_tab, npool_copy((const unsigned char *)h, psl_pool), curr_unitary_sn);
      charData_discard();
    }
}

static void
eH(void *userData, const char *name)
{
  if (!strcmp(name,"utf8"))
    {
      unsigned char *cdata = (unsigned char *)charData_retrieve();
      if (curr_unitary_sn)
	{
	  if (*cdata == '0' && !cdata[1]) {
	    curr_utf8 = NULL;
	    hash_add(cuneify_tab,curr_unitary_sn,npool_copy((unsigned char *)"",psl_pool));
	  } else {
	    curr_utf8 = npool_copy(cdata,psl_pool);
	    hash_add(cuneify_tab,curr_unitary_sn,curr_utf8);
	  }
	}
    }
  else if (!strcmp(name,"sign"))
    {
      curr_unitary_sn = NULL;
      deprecated_sign = 0;
    }
  else if (!strcmp(name,"form"))
    {
      curr_unitary_sn = sign_sn;
      curr_form = NULL;
    }
}

static const unsigned char *
no_bs(const unsigned char *maybe_bs)
{
  static unsigned char *buf = NULL;
  static int buf_len = 0;
  const unsigned char *bs = NULL;
  if (!maybe_bs)
    {
      if (buf)
	{
	  free(buf);
	  buf = NULL;
	  buf_len = 0;
	}
      return NULL;
    }
  else if ((bs = (unsigned char *)strchr((char*)maybe_bs,'\\')))
    {
      while (buf_len < (strlen((char*)maybe_bs)+1))
	buf = realloc(buf,buf_len+=128);
      strcpy((char*)buf,(char*)maybe_bs);
      buf[bs - maybe_bs] = '\0';
      return buf;
    }
  else
    return maybe_bs;
}

void
psl_init()
{
  const char *fname[2];

  if (!psl_initialized)
    {
      fname[0] = sl_fn;
      fname[1] = NULL;
      cuneify_tab = hash_create(2000);
      psl = hash_create(2000);
      psl_v = hash_create(8000);
      psl_id = hash_create(12000);
      psl_pool = npool_init();
      hex_tab = hash_create(2000);
      if (!xaccess(fname[0],R_OK,0))
	runexpat(i_list, fname, sH, eH);
      else
	fprintf(stderr,"gsl: %s not found\n",sl_fn);
      psl_initialized = 1;
    }
}

void
psl_term()
{
  hash_free(cuneify_tab,NULL);
  hash_free(psl,NULL);
  hash_free(psl_id,NULL);
  hash_free(psl_v,NULL);
  npool_term(psl_pool);
  hash_free(hex_tab,NULL);
  psl_initialized = 0;
  psl_bounded_sname(NULL);
  psl_get_sname(NULL);
  free(sname_buf);
  sname_buf_alloced = 0;
  (void)no_bs(NULL);
}

unsigned char *
psl_bounded_sname(const unsigned char *v)
{
  static unsigned char *buf = NULL;
  static int buf_alloced = 0;
  if (v)
    {
      while ((strlen((char*)v)+3) > buf_alloced)
	{
	  buf_alloced += 64;
	  buf = realloc(buf,buf_alloced*sizeof(char));
	}
      sprintf((char*)buf,"#%s#",v);
      return buf;
    }
  else if (buf)
    {
      free(buf);
      buf_alloced = 0;
      buf = NULL;
    }
  return NULL;
}

const unsigned char *
psl_cuneify(const unsigned char *g)
{
  return g ? hash_find(cuneify_tab,g) : NULL;
}

const char *
psl_get_id(const unsigned char *v)
{
  return hash_find(psl_id, v);
}

const unsigned char *
psl_get_sname(const unsigned char *value)
{
  static unsigned char *sbuf = NULL;
  static int sbuf_alloced = 0;
  if (value)
    {
      unsigned char *sname;
      sname = hash_find(psl_v,value);
      if (sname)
	{
	  char *plus = NULL;
	  while (strlen((char*)sname) >= sbuf_alloced)
	    {
	      sbuf_alloced += 256;
	      sbuf = realloc(sbuf,sbuf_alloced*sizeof(char));
	    }
	  strcpy((char *)sbuf,(char*)sname+1);
	  if ((plus = strstr((const char *)sbuf,"++")))
	    *(strchr((char*)sbuf,'+')) = '\0';
	  else
	    *(strchr((char*)sbuf,'#')) = '\0';
	  return sbuf;
	}
      else
	{
	  unsigned char *p = hash_find(psl, value);
	  if (p && !*p)
	    return value;
	  else
	    return (const unsigned char *)p;
	}
    }
  else if (sbuf)
    {
      free(sbuf);
      sbuf_alloced = 0;
      sbuf = NULL;
    }
  return NULL;
}

int
psl_is_sname(const unsigned char *sname)
{
  const unsigned char *nobs = no_bs(sname);
  void *ret = hash_find(psl,nobs);
  return (ret != NULL);
}

int
psl_is_value(const unsigned char *value)
{
  return value ? (intptr_t)hash_find(psl_v,no_bs(value)) : 0;
}

/* FIXME: This should be much more rigorous ... */
int
psl_looks_like_sname(const unsigned char *str)
{
  while (*str)
    if (isupper(*str))
      return 1;
    else
      ++str;
  return 0;
}

/* Compare two values to see if they belong to the same sign;
   in fact, values may also be sign-names */
int
psl_same_sign(const unsigned char *v1, const unsigned char *v2)
{
  const char *s1 = hash_find(psl_id, v1), *s2 = hash_find(psl_id, v2);
  if (s1 && s2)
    return !strcmp(s1, s2);
  else
    return !strcmp((const char *)v1, (const char *)v2);
}

unsigned char *
psl_hex_to_sign(const char *hex)
{
  unsigned char *res = hash_find(hex_tab, (const unsigned char *)hex);
  return res;
}

static unsigned char *
sname_append(const unsigned char *sn,const unsigned char *app)
{
  while (strlen((const char*)sn)+strlen((const char *)app)+3 >= sname_buf_alloced)
    {
      sname_buf_alloced += 1024;
      sname_buf = realloc(sname_buf,sname_buf_alloced);
    }
  sprintf((char*)sname_buf,"%s++%s",sn,app);
  return sname_buf;
}

void
gsl_map_ids(struct sas_map *m)
{
  int i;
  for (i = 0; m[i].v; ++i)
    {
      if (!(m[i].a = (unsigned char *)psl_get_id(m[i].v)))
	m[i].a = m[i].v;
    }
}
