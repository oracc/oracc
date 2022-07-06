#include "gx.h"
#include "sx.h"
#include <stdlib.h>
#include <ctype128.h>

static void parse_sig(struct sigfile *ssp, unsigned char *s);

static int
ssp_key_cmp(const void *k1, const void *k2)
{
  return strcmp(*(char * const *)k1,*(char * const *)k2);
}

static struct sigfile *
init_sigfile(const char *file, unsigned char*ftext)
{
  struct sigfile *ssp = calloc(1,sizeof(struct sigfile));
  ssp->file = file;
  ssp->lnum = 0;
  ssp->ftext = ftext;
  ssp->sdp_alloc = 1024;
  ssp->sigs = calloc(ssp->sdp_alloc,sizeof(struct sigdata));
  ssp->pool = npool_init();
  ssp->scp = sig_context_init();
  return ssp;
}

static struct sigdata *
ssp_add_sdp(struct sigfile *ssp)
{
  if (ssp->sdp_next == ssp->sdp_alloc)
    {
      ssp->sdp_alloc *= 2;
      ssp->sigs = realloc(ssp->sigs, ssp->sdp_alloc * sizeof(struct sigdata));
      memset(&ssp->sigs[ssp->sdp_next], '\0', (ssp->sdp_alloc/2)*sizeof(struct sigdata));
    }
  return &ssp->sigs[ssp->sdp_next++];
}

void
sigdump(struct sigfile *ssp)
{
  int i;
  for (i = 0; i < ssp->nkeys; ++i)
    {
      if (ssp->keys[i])
	{
	  /*printf("%s\n", ssp->keys[i]);*/
	  List *lp = hash_find(ssp->index, ssp->keys[i]);
	  if (lp)
	    {
	      int i;
	      List_node *lrover;
	      for (i = 0, lrover = lp->first; i < lp->count; ++i, lrover = lrover->next)
		{
		  printf("%s\n", ((struct sigdata*)(lrover->data))->sig);
		}
	    }
	}
      else
	{
	  fprintf(stderr, "sigdump(): iteration overran number of keys\n");
	}
    }
}

void
sigindex(struct sigfile *ssp)
{
  int i;
  if (ssp->index)
    return;
  ssp->index = hash_create(1024);
  for (i = 0; i < ssp->nsigs; ++i)
    {
      if (ssp->sigs[i].sig && ssp->sigs[i].cgp_closed)
	{
	  List *lp = hash_find(ssp->index,ssp->sigs[i].cgp_closed);
	  if (!lp)
	    {
	      lp = list_create(LIST_SINGLE);
	      hash_add(ssp->index,ssp->sigs[i].cgp_closed,lp);
	    }
	  list_add(lp, &ssp->sigs[i]);
	}
    }
  ssp->keys = (unsigned char **)hash_keys(ssp->index);
  ssp->nkeys = ssp->index->key_count;
  qsort(ssp->keys, ssp->nkeys, sizeof(unsigned char *), ssp_key_cmp);
}

struct sigfile *
sigload(const char *file)
{
  struct sigfile *ssp = NULL;
  ssize_t fsize = 0;
  unsigned char *ftext = NULL;
  unsigned char *ftext_post_bom, **lines, **next;
  struct sigdata *end = NULL;

  ftext = slurp("sigload",file,&fsize);
  if (ftext)
    {
      ftext_post_bom = check_bom(ftext);
      if (!ftext_post_bom)
	return NULL;
      
      (void)vchars(ftext_post_bom,fsize);
      next = lines = setup_lines(ftext_post_bom);
      if (!strncmp((ccp)*next,"@fields",strlen((ccp)"@fields")))
	{
	  unsigned char *tmp = *next;
	  tmp += strlen("@fields"); /* just in case there is ever a project called 'fields' */
	  if (isspace(*tmp) || !*tmp)
	    ++next;
	}
      
      /* Read all the sigs into an array */
      ssp = init_sigfile(file,ftext);
      while (*next)
	{
	  ++ssp->lnum;
	  parse_sig(ssp,*next);
	  ++next;
	}
      end = ssp_add_sdp(ssp);
      end->sig = NULL;
      ssp->nsigs = ssp->sdp_next - 1;
    }

  return ssp;
}

static void
parse_sig(struct sigfile *ssp, unsigned char *s)
{
  unsigned char *c = NULL;
  struct sigdata *sdp = ssp_add_sdp(ssp);
  sdp->sig = s;
  c = sdp->copy = npool_copy(s,ssp->pool);
  while (*c && '\t' != *c)
    ++c;
  if (*c)
    {
      *c++ = '\0';
      if (isdigit(*c))
	{
	  sdp->rank = atoi((ccp)c);
	  while (isdigit(*c))
	    ++c;
	  if (*c && *c != '\t')
	    {
	      vwarning2(ssp->file,ssp->lnum, "bad sig format: expected tab after rank\n");
	      return;
	    }
	  *c++ = '\0';
	}
      if (isdigit(*c))
	{
	  sdp->count = atoi((ccp)c);
	  while (isdigit(*c))
	    ++c;
	  if (*c && *c != '\t')
	    {
	      vwarning2(ssp->file,ssp->lnum, "bad sig format: expected tab after count\n");
	      return;
	    }
	  *c++ = '\0';
	}
      if (*c)
	sdp->insts = (char*)c;
    }
  if (f2_parse((ucp)ssp->file,ssp->lnum,sdp->copy,&sdp->f2,NULL,ssp->scp) > 0)
    sdp->cgp_closed = cgp_str(sdp->f2.cf, sdp->f2.gw, sdp->f2.pos, 0);
}
