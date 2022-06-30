#include <ctype128.h>
#include "gx.h"

unsigned char *
cgp_cgp_str(struct cgp *c, int spread)
{
  return cgp_str(c->cf, c->gw, c->pos, spread);
}

void
cgp_entry(struct cgp *c, struct entry *e)
{
  e->cf = c->cf;
  e->gw = c->gw;
  e->pos = c->pos;
}

unsigned char *
cgp_entry_str(struct entry *e, int spread)
{
  return cgp_str(e->cf, e->gw, e->pos, spread);
}

/* Caller should zero the struct cgp * if desired */
void
cgp_parse(struct cgp *c, unsigned char *s)
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
        warning("syntax error in CGP: missing [ at start of GW");
      else
        {
          *t = '\0';
          c->gw = t+1;
          if (strchr((ccp)s,'[') || strchr((ccp)s,']'))
            warning("syntax error in @entry: too many [ or ]");
          while (isspace(t[-1]))
            --t;
          *t = '\0';
        }
    }
  else
    warning("syntax error in CGP: expected ']' to end GW");
  if (c->pos && !cbdpos((ccp)c->pos, strlen((ccp)c->pos)))
    vwarning("bad POS %s", c->pos);
    
}

unsigned char *
cgp_str(unsigned const char *cf, unsigned const char *gw, unsigned const char *pos, int spread)
{
  char *tmp = malloc(3+(spread*2)+strlen((ccp)cf)+strlen((ccp)gw)+strlen((ccp)pos));
  if (spread)
    sprintf(tmp, "%s [%s] %s", cf, gw, pos);
  else
    sprintf(tmp, "%s[%s]%s", cf, gw, pos);
  return (ucp)tmp;
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
