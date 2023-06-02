#include <ctype128.h>
#include <psd_base.h>
#include "loadfile.h"
#include "list.h"
#include "hash.h"
#include "pool.h"
#include "sas.h"

#define xstrlen(x) strlen((const char *)x)
#define xstrchr(x,c) (unsigned char *)strchr((const  char *)x,c)
extern int verbose;
static unsigned char **
asa_split(struct sas_info*sip, struct sas_constraint *cp, 
	  int *lenp, unsigned char *s, unsigned char **ps)
{
  int nbits = 1;
  unsigned char **ptrs, *tmp = s;
  struct sas_ep_block *bp;

  while (*tmp && !isspace(*tmp))
    {
      if (*tmp++ == '-')
	++nbits;
      else if ((*tmp == '}' && tmp[1]  && tmp[1] != '-') /* predet */
	       || (*tmp == '{' && tmp > s && tmp[-1] != '-') /* postdet */)
	++nbits;
    }
  if (lenp)
    *lenp = nbits;
  bp = &sip->ep_blocks[sip->ep_blocks_alloced-1];
  if ((256 - bp->ptrs_used) < (nbits+1))
    {
      sip->ep_blocks = realloc(sip->ep_blocks,
			       ++sip->ep_blocks_alloced * sizeof(struct sas_ep_block));
      bp = &sip->ep_blocks[sip->ep_blocks_alloced-1];
      bp->ptrs_used = 0;
    }
  ptrs = &bp->ptrs[bp->ptrs_used];
  bp->ptrs_used += nbits;
  for (nbits = 0; *s && !isspace(*s); ++nbits)
    {
      if (*s == '{')
	*s++ = '\0';
      ptrs[nbits] = s;
      while (*s && !isspace(*s))
	{
	  if (*s == '-')
	    {
	      *s++ = '\0';
	      if (*s == '{')
		*s++ = '\0';
	      break;
	    }
	  else if (*s == '{')
	    {
	      *s++ = '\0';
	      break;
	    }
	  else if (*s == '}')
	    {
	      *s++ = '\0';
	      if (*s == '-')
		*s++ = '\0';
	      break;
	    }
	  else if (*s)
	    ++s;
	}
    }
  while (isspace(*s))
    *s++ = '\0';
  *ps = s;
  return ptrs;
}

struct sas_info *
sas_asa_load(const char *fname)
{
  static unsigned char *s;
  struct sas_info *sip = calloc(1, sizeof(struct sas_info));
  size_t flen;
  struct sas_alias *ap = NULL;
  static int one = 1;

  sip->name = strcpy(malloc(strlen(fname)+1),fname);
  sip->cand = hash_create(1024);
  sip->nodumb = hash_create(1024);
  sip->post = hash_create(1);
  
  s = sip->file = loadfile((unsigned char *)fname,&flen);

  if (!sip->file)
    return NULL;
  else if (verbose)
    fprintf(stderr,"sas_asa_load: load smart aliases %s\n",fname);

  sip->flen = flen;
  sip->alias_mem = malloc((flen/4)*sizeof(struct sas_alias));
  sip->constraint_mem = calloc(flen/4,sizeof(struct sas_constraint));
  sip->ep_blocks = malloc(sizeof(struct sas_ep_block));
  sip->ep_blocks_alloced = 1;
  sip->ep_blocks[0].ptrs_used = 0;

  while (*s)
    {
      unsigned char *eqarrow = NULL, *eol = NULL;
      switch (*s)
	{
	case '#':
	  if (!(eol = xstrchr(s,'\n')))
	    eol = s + xstrlen(s);
	  else
	    ++eol;
	  s = eol;
	  break;
	case '\t':
	case ' ':
	  while (isspace(*s))
	    ++s;
	  if (*s == '\n')
	    ++s;
	  else
	    {
	      struct sas_constraint *cp = &sip->constraint_mem[sip->constraint_used++];
	      unsigned char *square;

	      if (!ap)
		{
		  fprintf(stderr,"NULL ap\n");
		  exit(2);
		}
	      if (!ap->constraints)
		ap->constraints = list_create(LIST_SINGLE);
	      list_add(ap->constraints,cp);

	      if (!(eol = xstrchr(s,'\n')))
		eol = s + xstrlen(s);
	      else
		*eol++ = '\0';
	      if ((square = xstrchr(s,'[')))
		{
		  cp->type = sc_cons_lem;
		  if (*s == '!')
		    {
		      cp->u.l.neg = 1;
		      ++s;
		    }
		  cp->u.l.cf = s;
		  *square++ = '\0';
		  cp->u.l.gw = square;
		  if ((square = xstrchr(square,']')))
		    {
		      *square++ = '\0';
		      if (*square)
			{
			  cp->u.l.pos = square;
			  while (*square && !isspace(*square))
			    ++square;
			  if (isspace(*square))
			    *square = '\0';
			}
		      else
			;/*ERROR*/
		    }
		  else
		    ;/*ERROR*/
		}
	      else
		{
		  int nparse = 0;
		  cp->type = sc_cons_env;
		env_parser:
		  {
		    int neg = 0;
		    ++nparse;
		    if (*s == '!')
		      {
			neg = 1;
			++s;
		      }
		    if (*s == '<')
		      {
			cp->u.e.pre_neg = neg;
			cp->u.e.pre = asa_split(sip,cp,&cp->u.e.pre_len,s+1,&s);
		      }
		    else if (*s == '>')
		      {
			cp->u.e.pos_neg = neg;
			cp->u.e.pos = asa_split(sip,cp,&cp->u.e.pos_len,s+1,&s);
		      }
		    else
		      ;/*ERROR*/
		  }
		  while (isspace(*s))
		    ++s;
		  if (*s)
		    {
		      if (nparse == 1)
			goto env_parser;
		      else
			;/*ERROR*/
		    }
		}
	    }
	  s = eol;
	  break;
	default:
	  if (!(eol = xstrchr(s,'\n')))
	    eol = s + xstrlen(s);
	  else
	    *eol++ = '\0';
	  if ((eqarrow = (unsigned char *)strstr((const char *)s, "=>")))
	    {
	      unsigned char *e = eqarrow;
	      while (isspace(e[-1]))
		--e;
	      *e = '\0';
	      ++eqarrow;
	      ++eqarrow;
	      while (isspace(*eqarrow))
		++eqarrow;
	      e = eol - 1;
	      while (isspace(e[-1]))
		--e;
	      if (*e)
		*e = '\0';
	      hash_add(sip->post,s,eqarrow);
	      s = eol;
	    }
	  else
	    {
	      unsigned char *head = s;
	      while (*s && !isspace(*s))
		++s;
	      if (*s)
		{
		  *s++ = '\0';
		  ap = &sip->alias_mem[sip->alias_used++];
		  if ('=' == *head)
		    {
		      ap->global = 1;
		      ++head;
		    }
		  else
		    ap->global = 0;
		  ap->head = head;
		  ap->constraints = NULL;
		  while (*s && *s != '\n')
		    {
		      unsigned char *cand = s;
		      List *candlist;
		      while (*s && !isspace(*s))
			++s;
		      if (' ' == *s || '\t' == *s)
			*s++ = '\0';
		      if ('-' == *cand)
			{
			  /* This relation is not used in dumb aliasing;
			     record it as, e.g., "e>eg2" */
			  char *tmp = malloc(strlen(cand)+strlen(head)+2);
			  ++cand;
			  sprintf(tmp,"%s>%s",cand,head);
			  hash_add(sip->nodumb,xstrdup(tmp),&one);
			  free(tmp);
			}
		      else if ('+' == *cand) /* A new thing after dumb/smart merge the incomers from dumb are marked with + */
			++cand;
		      if ((candlist = hash_find(sip->cand,cand)))
			list_add(candlist, ap);
		      else
			{
			  candlist = list_create(LIST_SINGLE);
			  list_add(candlist,ap);
			  hash_add(sip->cand,cand,candlist);
			}
		    }
		  *s++ = '\0';
		}
	      else
		;/*ERROR*/
	    }
	  s = eol;
	  break;
	}
    }
  return sip;
}

static void
free_alias(struct sas_alias *ap)
{
  if (ap)
    {
      if (ap->constraints)
	list_free(ap->constraints, NULL);
    }
}

static void
sas_list_free(List *lp)
{
  list_free(lp, NULL);
}

void
sas_asa_unload(struct sas_info *sip)
{
  int i;
  free((char*)sip->name);
  hash_free(sip->cand,(hash_free_func*)sas_list_free);
  hash_free(sip->post,NULL);
  free(sip->file);
  for (i = 0; i < sip->alias_used; ++i)
    free_alias(&sip->alias_mem[i]);
  free(sip->alias_mem);
  free(sip->constraint_mem);
  free(sip->ep_blocks);
  free(sip);
}
