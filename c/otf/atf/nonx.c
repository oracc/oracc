#include <stdio.h>
#include <string.h>
#include <ctype128.h>
#include <unistd.h>
#include <stdlib.h>
#include "nonx.h"
#include "cdf.h"
#include "pool.h"
#include "warning.h"
#include "tree.h"

int no_strict_dollar;

extern const char *textid;

void
nonx_link_data(struct nonx_link *nonxp, struct node *tmp)
{
  if (nonxp->pre)
    appendChild(tmp,textNode((unsigned const char *)nonxp->pre));
  appendChild(tmp,rawTextNode((unsigned const char *)" <xh:a href=\""));
  appendChild(tmp,textNode((unsigned const char *)nonxp->url));
  appendChild(tmp,rawTextNode((unsigned const char *)"\">"));
  appendChild(tmp,textNode((unsigned const char *)nonxp->text));
  appendChild(tmp,rawTextNode((unsigned const char *)"</xh:a> "));
  if (nonxp->post)
    appendChild(tmp,textNode((unsigned const char *)nonxp->post));
}

struct nonx_link *
nonx_link(unsigned char *l, char *http)
{
  char *end = NULL;
  struct nonx_link *nlp = calloc(1,sizeof(struct nonx_link));
  
  /* Format is:

     $ (Some pre-text http://bla.com[[link-display-text]] some post-text)

     If the [[]] is empty the URL is displayed as the link text.

  */
  while (isspace(*l))
    ++l;
  nlp->pre = (const char *)l;
  end = http;
  while (isspace(end[-1]))
    --end;
  if (isspace(*end))
    {
      *end = '\0';
      nlp->url = http;
      l = (unsigned char *)http;
      if ((http = strstr((char *)l, "[[")))
	{
	  end = strstr(http, "]]");
	  if (end)
	    {
	      *http = '\0';
	      nlp->text = http + 2;
	      if (nlp->text == end)
		{
		  nlp->text = nlp->url;
		  end += 2;
		}
	      else
		{
		  *end++ = '\0';
		  ++end;
		}
	      while (isspace(*end))
		++end;
	      nlp->post = end;
	    }
	  else
	    warning("link without closing ']]'");
	}
      else
	warning("link without contained text opener '[['");
    }
  else if (http > nlp->pre)
    warning("link must have space before 'http'");

  return nlp;
}

static int
last_tok(unsigned char *end)
{
  while (*end)
    {
      if (')' == *end || '!' == *end || '?' == *end || '*' == *end
	  || isspace(*end))
	++end;
      else
	return 0;
    }
  return 1;
}

struct nonx*
parse_nonx(unsigned char *l)
{
  static struct nonx nx;
  /*unsigned char *parenc = NULL; */
  int unknown_toks = 0;

  memset(&nx,'\0',sizeof(struct nonx));

  if (!*l)
    {
      nx.strict = 0;
      nx.scope = nonxtok("empty", 5);
      nx.ref = uc("none");
      return &nx;
    }

  if ('(' == *l)
    {
      register unsigned char *end = l+xxstrlen(l);
      char *http = NULL;
      
      nx.strict = 0;
      ++l;
      while (isspace(end[-1]))
	--end;
      if (')' == end[-1])
	{
	  *--end = '\0';
	  /*parenc = end;*/
	}
      else
	{
	  warning("unclosed parenthesis in $-line");
	  if (*end)
	    *end = '\0';
	}
      /* pre-empt the normal  $ (...) parser if we are dealing
	 with $ (image N = <text>) */
      if (!strncmp("image",(char*)l,5) && isspace(l[5]))
	{
	  register unsigned char *img_num = l+6;
	  while (isspace(*img_num))
	    ++img_num;
	  if (isdigit(*img_num))
	    {
	      nx.scope = nonxtok("image", 5);
	      l = img_num;
	      while (*l && !isspace(*l))
		++l;
	      if (*l)
		{
		  char save = *l, *savep = (char *)l;
		  *l++ = '\0';
		  while (isspace(*l))
		    ++l;
		  if (*l == '=')
		    {
		      char refbuf[128];
		      sprintf(refbuf,"%s@%s",textid,img_num);
		      nx.ref = pool_copy((unsigned char *)refbuf);
		      *savep = save;
		      ++l;
		      while (isspace(*l))
			++l;
		      nx.literal = l;
		      return &nx;
		    }
		  else
		    warning("image comment without '='");
		}
	      else
		warning("image comment without '=' part");
	      return NULL;
	    }
	}
      else if ((http = strstr((const char *)l, "http")))
	{
	  nx.link = nonx_link(l, http);
	  return &nx;
	}
    }
  else
    nx.strict = 1;
  while (*l)
    {
      if (isdigit(*l))
	{
	  static unsigned char n[128];
	  register unsigned char *np = n;
	  *n = '\0';
	  while (isdigit(*l) && np - n < 127)
	    *np++ = *l++;
	  if ('-' == *l && np - n < 127)
	    {
	      *np++ = *l++;
	      while (isdigit(*l) && np - n < 127)
		*np++ = *l++;
	      *np = '\0';
	      if (np[-1] == '-')
		{
		  vwarning("%s: malformed extent in $-line",n);
		  return NULL;
		}
	      else
		nx.number = n;
	    }
	  else 
	    {
	      *np = '\0';
	      nx.number = n;
	    }
	}
      else if (islower(*l))
	{
	  register unsigned char *end = l;
	  struct nonx_token *nxp;
	  unsigned char save, flags[4], *flagsp = flags;
	  int flags_offset = 0;

	  while (islower(*end))
	    ++end;
	  if (nx.strict && last_tok(end))
	    {
	      while ((*end == '!' || *end == '?' || *end == '*')
		     && flagsp - flags < 4)
		*flagsp++ = *end++;
	      *flagsp = '\0';
	      if (*flags)
		{
		  flags_offset = flagsp-flags;
		  xstrcpy(nx.flags,flags);
		}
	    }
	  if (!*end || isspace(*end) || ')' == *end)
	    {
	      save = *(end-flags_offset);
	      *(end-flags_offset) = '\0';
	      nxp = nonxtok((const char *)l,(end-flags_offset)-l);
	      *(end-flags_offset) = save;
	      if (nxp)
		{
		  switch (nxp->class)
		    {
		    case x_extent:
		      nx.extent = nxp;
		      if (nx.extent->of_flag)
			{
			  register unsigned char *ofp = l;
			  while (isspace(*ofp))
			    ++ofp;
			  if (!xstrncmp(ofp,"of",2))
			    l = ofp+2;
			}
		      break;
		    case x_scope:
		      nx.scope = nxp;
		      if (!strncmp(cc(l),"seal ",5))
			{
			  /* if there is a following token it's an impression
			     notation */
			  *end = save;
			  while (isspace(*end))
			    ++end;
			  /*DOCME: impression legal as well as digit*/
			  if (isdigit(*end) || isupper(*end))
			    {
			      register unsigned char *tok = end;
			      nx.scope = nonxtok("impression", 10);
			      while (*end && !isspace(*end))
				++end;
			      save = *end;
			      *end = '\0';
			      nx.number = pool_copy(tok);
			      *end = save;
			    }
			  else if (!strncmp((const char*)end,"impression",10))
			    {
			      nx.scope = nonxtok("impression", 10);
			      end += 10;
			    }
			  else if (*end == '=')
			    {
			      ++end;
			      nx.scope = nonxtok("impression", 10);
			      while (isspace(*end))
				++end;
			      nx.ref = pool_copy(end);
			      end += strlen((const char *)nx.ref);
			    }
			}
		      else if (!strncmp(cc(l),"top ",3))
			{
			  unsigned char *e = end;
			  while (isspace(*e))
			    ++e;
			  if (!strncmp(cc(e),"of ",3))
			    {
			      warning("'top of' should be 'start of'");
			      nx.extent = nonxtok("start",5);
			      end = e + 3;
			      nx.scope = NULL;
			    }
			}
		      else if (!strncmp(cc(l),"docket ",7))
			{
			  if (isdigit(*end) || isupper(*end))
			    {
			      register unsigned char *tok = end;
			      while (*end && !isspace(*end))
				++end;
			      save = *end;
			      *end = '\0';
			      nx.number = pool_copy(tok);
			      *end = save;
			    }
			  else
			    {
			      /* silently default to 1 */
			      nx.number = uc("1");
			    }
			}
		      if (!nx.extent && !nx.number)
			{
			  if (nx.scope->type == x_surface)
			    nx.number = (unsigned char *)"0";
			  else
			    nx.number 
			      = (unsigned char *)(nx.scope->pl_flag ? "n" : "1");
			}
		      break;
		    case x_state:
		    case x_rule:
		      nx.state = nxp;
		      break;
		    case x_qual:
		      nx.qual = nxp;
		      break;
		    case x_ignore:
		    case x_image_class:
		    case x_impression:
		    case x_docket:
		    case x_empty_class:
		      break;
		      /* no default means compiler will warn about
			 unhandled enum values */
		    }
		}
	      else
		{
		  ++unknown_toks;
		  if (nx.strict)
		    {
		      vwarning("%s: bad token in $-line", l);
		      return NULL;
		    }
		}
	      l = end;
	    }
	  else
	    {
	      ++unknown_toks;
	      if (nx.strict)
		{
		  vwarning("%s: bad token in $-line", l);
		  return NULL;
		}
	      while (*end && !isspace(*end) && ')' != *end)
		++end;
	      l = end;
	    }
	}
      else if (isspace(*l))
	while (isspace(*l))
	  ++l;
      else if (*l == '?' || *l == '!')
	++l; /* ignore query and remark in $-lines */
      else
	{
	  if (nx.strict)
	    {
	      vwarning("%s: bad character in $-line",l);
	      return NULL;
	    }
	  ++unknown_toks;
	  ++l;
	}
    }
#if 0
  if (parenc)
    *parenc = ')';
#endif
  if (!unknown_toks && nx.strict)
    {
      if (!nx.extent && !nx.number)
	nx.number = (unsigned char *)"n";
      if (!nx.scope)
	nx.scope = nonxtok("lines",5);
    }
  if (no_strict_dollar)
    nx.strict = 0;
  else if (!unknown_toks && !nx.strict && nx.scope && nx.state)
    {
      notice("$-line meets strict criteria; remove parens");
      nx.strict = -1;
    }
  return &nx;
}

/* FIFO stack of $-lines for use in aligning $-lines in translations */
int dollar_fifo = 0;
static char const **fifo;
static int fifo_used, fifo_alloced, fifo_read;
#define FIFO_SIZE 64
void
dollar_init(void)
{
  if (!fifo)
    {
      fifo = malloc(FIFO_SIZE*sizeof(const char *));
      fifo_alloced = FIFO_SIZE;
      fifo_used = fifo_read = 0;
    }
  else
    dollar_clear();
}
const char *
dollar_add(const char *xid)
{
  if (fifo_used == fifo_alloced)
    {
      fifo_alloced += FIFO_SIZE;
      fifo = realloc(fifo, fifo_alloced * sizeof(const char *));
    }
  return fifo[fifo_used++] = xid;
}
const char *
dollar_get(void)
{
  if (fifo_read < fifo_used)
    return fifo[fifo_read++];
  else
    return NULL;
}
void
dollar_reset(void)
{
  fifo_read = 0;
}
int
dollar_clear(void)
{
  int ret = fifo_used - fifo_read;
  fifo_used = fifo_read = 0;
  return ret;
}
int
dollar_term(void)
{
  int ret = dollar_clear();
  free(fifo);
  fifo = NULL;
  fifo_alloced = 0;
  return ret;
}
const char *
dollar_peek(void)
{
  if (fifo_read < fifo_used)
    return fifo[fifo_read];
  else
    return NULL;
}
