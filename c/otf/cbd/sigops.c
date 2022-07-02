#include "gx.h"
#include "sx.h"
#include <stdlib.h>
#include <ctype128.h>

static void parse_sig(struct sigfile *ssp, unsigned char *s);

static struct sigfile *
init_sigfile(const char *file, unsigned char*ftext)
{
  struct sigfile *ssp = malloc(sizeof(struct sigfile));
  ssp->file = file;
  ssp->lnum = 0;
  ssp->ftext = ftext;
  ssp->sdp_alloc = 1024;
  ssp->sigs = calloc(ssp->sdp_alloc,sizeof(struct sigdata));
  ssp->pool = npool_init();
  return ssp;
}

struct sigfile *
sigload(const char *file)
{
  struct sigfile *ssp = NULL;
  ssize_t fsize = 0;
  unsigned char *ftext = NULL;
  unsigned char *ftext_post_bom, **lines, **next;

  ftext = slurp("sigload",file,&fsize);
  ftext_post_bom = check_bom(ftext);
  if (!ftext_post_bom)
    return NULL;

  (void)vchars(ftext_post_bom,fsize);
  next = lines = setup_lines(ftext_post_bom);

  /* Read all the sigs into an array */
  ssp = init_sigfile(file,ftext);
  while (*next)
    {
      ++ssp->lnum;
      parse_sig(ssp,*next);
      ++next;
    }
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
  f2_parse((ucp)ssp->file,ssp->lnum,c,&sdp->f2,NULL,ssp->scp);
}
