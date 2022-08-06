#include <ctype128.h>
#include <npool.h>
#include <unistd.h>
#include "gx.h"

static List *cgplist = NULL;

static struct npool *common_pool = NULL;

struct loctok *
loctok(YYLTYPE *lp, struct entry *e, unsigned char *tok)
{
  struct loctok *ltp = mb_new(e->owner->loctokmem);
  ltp->l = *lp;
  ltp->tok = tok;
  return ltp;
}

struct tag *
tag_init(YYLTYPE l, struct entry *e, const char *name, unsigned char *val)
{
  struct tag *tp;
  tp = mb_new(e->owner->tagmem);
  tp->l = l;
  tp->name = name;
  tp->val = val;
  return tp;
}

void
common_init(void)
{
  common_pool = npool_init();
}

void
common_term(void)
{
  npool_term(common_pool);
  common_pool = NULL;
}

List *
cgp_get_all(void)
{
  if (cgplist)
    {
      List *tmp = cgplist;
      cgplist = NULL;
      return tmp;
    }
  return NULL;
}

struct cgp*
cgp_get_one(void)
{
  if (cgplist)
    {
      struct cgp*ret = list_first(cgplist);
      list_free(cgplist, NULL);
      cgplist = NULL;
      return ret;
    }
  return NULL;
}

void
cgp_save(unsigned char *cf, unsigned char *gw, unsigned char *pos)
{
  struct cgp *c = mb_new(curr_cbd->cgpmem);
  cgp_init(c, cf, gw, pos);
  if (!cgplist)
    cgplist = list_create(LIST_SINGLE);
  list_add(cgplist, c);
}

void
cgp_init(struct cgp *c, unsigned char *cf, unsigned char *gw, unsigned char *pos)
{
  c->cf = cf;
  c->gw = gw;
  c->pos = pos;
  c->tight = cgp_str(c, 0);
  c->loose = cgp_str(c, 1);
}

#if 0
const unsigned char *
cgp_entry_str(struct entry *e, int spread)
{
  return cgp_str(e->cgp, spread);
}

/* Caller should zero the struct cgp * if desired */
void
cgp_parse(struct cgp *c, unsigned char *s, locator *lp)
{
  unsigned char *t = NULL;
  
  while (isspace(*s))
    ++s;
  c->cf = s;
  t = s + strlen((ccp)s);
  while (t > s && isspace(t[-1]))
    --t;
  if (*t)
    *t = '\0';
  while (t > s && !isspace(t[-1]) && ']' != t[-1])
    --t;
  c->pos = t;
  while (t > s && isspace(t[-1]))
    --t;
  if (']' == t[-1])
    {
      --t;
      *t = '\0';
      while (t > s && '[' != *t)
        --t;
      if (t == s)
        vwarning2(lp->file,lp->line,"syntax error in CGP: missing [ at start of GW");
      else
        {
          *t = '\0';
          c->gw = t+1;
          if (strchr((ccp)s,'[') || strchr((ccp)s,']'))
            vwarning2(lp->file,lp->line,"syntax error in @entry: too many [ or ]");
          while (isspace(t[-1]))
            --t;
          *t = '\0';
        }
    }
  else
    vwarning(lp->file,lp->line,"syntax error in CGP: expected ']' to end GW");
  if (c->pos && !cbdpos((ccp)c->pos, strlen((ccp)c->pos)))
    vwarning2(lp->file,lp->line,"bad POS %s", c->pos);
}
#endif

const unsigned char *
cgp_str(struct cgp *cp, int loose)
{
  char *tmp = NULL;
  unsigned char*tmp2;

  if (!cp)
    return NULL;

  if (!cp->cf || !cp->gw || !cp->pos)
    return NULL;

  if (loose && cp->loose)
    return cp->loose;
  if (!loose && cp->tight)
    return cp->tight;
  
  tmp =  malloc(3+(loose*2)+strlen((ccp)cp->cf)+strlen((ccp)cp->gw)+strlen((ccp)cp->pos));
  if (loose)
    sprintf(tmp, "%s [%s] %s", cp->cf, cp->gw, cp->pos);
  else
    sprintf(tmp, "%s[%s]%s", cp->cf, cp->gw, cp->pos);
  tmp2 = npool_copy((ucp)tmp, common_pool);
  free(tmp);
  return tmp2;
}

unsigned char *
check_bom(unsigned char *s)
{
  if (s[0] == 0xef && s[1] == 0xbb && s[2] == 0xbf)
    return s+3;
  else if ((s[0] == 0x00 && s[1] == 0x00 && s[2] == 0xfe && s[3] == 0xff)
	   || (s[0] == 0xff && s[1] == 0xfe && s[2] == 0x00 && s[3] == 0x00)
	   || (s[0] == 0xfe && s[1] == 0xff)
	   || (s[0] == 0xff && s[1] == 0xfe))
    {
      fprintf(stderr,"unhandled UTF-format (I only understand UTF-8)\n");
      return NULL;
    }
  else
    return s;
}

unsigned char **
setup_lines(unsigned char *ftext)
{
  unsigned char **p,**ret;
  register unsigned char*s = ftext;
  int nlines = 0;
  while (*s)
    {
      if (*s == '\r')
        {
          ++nlines;
          if (s[1] != '\n')
            *s++ = '\n';     /* for MAC \r, map to \n */
          else
            s+=2;            /* for DOS \r\n, skip \n */
        }
      else if (*s == '\n') /* UNIX */
        {
          ++nlines;
          ++s;
        }
      else
        {
          ++s;
        }
    }
  if (s[-1] != '\n' && s[-1] != '\r')
    ++nlines;
  ++nlines; /* NULL ptr to terminate */
  ret = p = malloc(nlines*sizeof(unsigned char *));
  s = ftext;
  while (*s)
    {
      *p++ = s;
      while (*s && '\n' != *s)
        ++s;
      if (*s == '\n')
        {
          if (s > ftext && s[-1] == '\r')
            {
              s[-1] = '\0';
              ++s;
            }
          else
            *s++ = '\0';
        }
    }
  *p = NULL;
  return ret;
}

unsigned char *
slurp(const char *caller, const char *fname, ssize_t *fsizep)
{
  struct stat finfo;
  int fdesc;
  ssize_t fsize;
  unsigned char *ftext = NULL;
  if (-1 == stat(fname,&finfo))
    {
      fprintf(stderr,"%s: stat failed on %s\n",caller,fname);
      return NULL;
    }
  if (!S_ISREG(finfo.st_mode))
    {
      fprintf(stderr,"%s: %s not a regular file\n",caller,fname);
      return NULL;
    }
  fsize = finfo.st_size;
  if (!fsize)
    {
      fprintf(stderr,"%s: %s: empty file\n",caller,fname);
      return NULL;
    }
  if (NULL == (ftext = malloc(fsize+1)))
    {
      fprintf(stderr,"%s: %s: couldn't malloc %d bytes\n",
              caller,fname,(int)fsize);
      return NULL;
    }

  fdesc = open(fname,O_RDONLY);
  if (fdesc >= 0)
    {
      ssize_t ret = read(fdesc,ftext,fsize);
      close(fdesc);
      if (ret == fsize)
        ftext[fsize] = '\0';
      else
        {
          fprintf(stderr,"%s: read %d bytes failed\n",caller,(int)fsize);
	  free(ftext);
          return NULL;
        }
    }
  else
    {
      fprintf(stderr, "%s: %s: open failed\n",caller,fname);
      free(ftext);
      return NULL;
    }

  file = errmsg_fn ? errmsg_fn : fname;
  if (fsizep)
    *fsizep = fsize;
  return ftext;
}

unsigned char *
tok(unsigned char *s, unsigned char *end)
{
  while (isspace(*s))
    ++s;
  if (*s)
    {
      unsigned char *e = s;
      while (!isspace(*e))
	++e;
      *end = *e;
      if (*e)
	*end = '\0';
    }
  else
    *end = '\0';     
  return s;
}

void
untab(unsigned char *s)
{
  while (*s)
    if ('\t' == *s)
      *s++ = ' ';
    else
      ++s;
}

