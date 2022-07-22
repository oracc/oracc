#include "gx.h"
#include "sx.h"
#include <stdlib.h>
#include <stdio.h>
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
	      if (sig_group)
		{
		  if (sig_cgp)
		    printf("@@%s\t%ld\n", ((struct sigdata*)(lp->first->data))->cgp_closed, lp->count);
		  for (i = 0, lrover = lp->first; i < lp->count; ++i, lrover = lrover->next)
		    {
		      printf("%s\t%s\n", ((struct sigdata*)(lrover->data))->sig,((struct sigdata*)(lrover->data))->insts);
		    }
		}
	      else
		{
		  for (i = 0, lrover = lp->first; i < lp->count; ++i, lrover = lrover->next)
		    {
		      printf("%s\n", ((struct sigdata*)(lrover->data))->sig);
		    }
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
sigmerge_print(unsigned char *curr_cgp, List *sigs)
{
  int i;
  List_node *lrover = NULL;
  printf("%s\t%ld\n", curr_cgp, sigs->count);
  for (i = 0, lrover = sigs->first; i < sigs->count; ++i, lrover = lrover->next)
    {
      printf("%s\n", (const char *)(lrover->data));
    }
}

static void
sigmerge_sig_line(unsigned char *sig, Hash_table *insts, List *sigs, struct npool *pool)
{
  const char ** pi = NULL;
  char *tmp;
  int i, ni, len = 0;
  
  pi = hash_keys2(insts, &ni);
  qsort(pi, ni, sizeof(const char *), (__compar_fn_t)strcmp);
  for (i = 0; i < ni; ++i)
    len += strlen(pi[i]);
  tmp = malloc(strlen((ccp)sig) + 10 + len + ni + 3); /* +10 for the frequency count */
  sprintf(tmp, "%s\t%d\t", sig, ni);
  for (i = 0; i < ni; ++i)
    {
      if (i && i < ni)
	strcat(tmp, " ");
      strcat(tmp, pi[i]);
    }
  list_add(sigs, npool_copy((ucc)tmp, pool));
  free(tmp);
  free(pi);
}

void
sigmerge(const char *f)
{
  FILE *f_in = NULL;
  unsigned char *lp = NULL, *curr_cgp = NULL;
  unsigned char *last_sig = NULL;
  Hash_table *insts = NULL;
  List *sigs = NULL;
  struct npool *pool = NULL;
  int ls_alloc = 4096;
  static int one;
  
  if (strcmp(f, "-"))
    f_in = xfopen(f, "r");
  else
    f_in = stdin;

  while ((lp = xgetline(f_in)))
    {
      if ('@' == lp[0] && '@' == lp[1])
	{
	  unsigned char *tmp = NULL;

	  if (curr_cgp)
	    {
	      if (last_sig && *last_sig)
		{
		  sigmerge_sig_line(last_sig, insts, sigs, pool);
		  if (insts)
		    hash_free(insts,NULL);		  
		  insts = hash_create(1024);
		}
	      sigmerge_print(curr_cgp, sigs);
	      npool_term(pool);
	      list_free(sigs,NULL);
	    }

	  tmp = (ucp)strchr((ccp)lp, '\t');
	  if (tmp)
	    *tmp = '\0';
	  pool = npool_init();
	  sigs = list_create(LIST_SINGLE);
	  curr_cgp = npool_copy((ucp)lp, pool);
	  if (last_sig)
	    *last_sig = '\0';
	}
      else
	{
	  unsigned char *sig = lp, *inst = NULL;
	  inst = (ucp)strchr((const char *)sig,'\t');
	  if (inst)
	    {
	      *inst++ = '\0';
	      if (strchr((ccp)inst, '\t'))
		{
		  fprintf(stderr, "sx -m : input lines only allowed <SIG><TAB><INSTS>\n");
		  exit(1);
		}
	      else
		{
		  if (last_sig && *last_sig)
		    {
		      if (strcmp((ccp)sig, (ccp)last_sig))
			{
			  sigmerge_sig_line(last_sig, insts, sigs, pool);
			  if (insts)
			    hash_free(insts,NULL);
			  insts = hash_create(1024);
			  while (strlen((ccp)sig) >= ls_alloc)
			    ls_alloc *= 2;
			  last_sig = realloc(last_sig, ls_alloc);
			  strcpy((char*)last_sig, (ccp)sig);
			}
		      hash_add(insts, npool_copy(inst, pool), &one);
		    }
		  else
		    {
		      while (strlen((ccp)sig) >= ls_alloc)
			ls_alloc *= 2;
		      last_sig = realloc(last_sig, ls_alloc);
		      strcpy((char*)last_sig, (ccp)sig);
		      if (insts)
			hash_free(insts, NULL);
		      insts = hash_create(1024);
		      hash_add(insts, npool_copy(inst, pool), &one);
		    }
		}
	    }
	  else
	    {
	      fprintf(stderr, "sx -m : input line has no INSTS\n");
	      exit(1);
	    }
	}
    }
  if (curr_cgp)
    {
      if (last_sig && *last_sig)
	{
	  sigmerge_sig_line(last_sig, insts, sigs, pool);
	  if (insts)
	    hash_free(insts,NULL);
	  insts = NULL;
	}
      sigmerge_print(curr_cgp, sigs);
      npool_term(pool);
      list_free(sigs, NULL);
    }
  free(last_sig);
  xgetline(NULL);
  xfclose(f,f_in);
}

static void
parse_sig(struct sigfile *ssp, unsigned char *s)
{
  unsigned char *c = NULL;
  struct sigdata *sdp = ssp_add_sdp(ssp);

  c = sdp->sig = s;
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
  sdp->sig_copy = npool_copy(s,ssp->pool);
  if (f2_parse((ucp)ssp->file,ssp->lnum,sdp->sig_copy,&sdp->f2,NULL,ssp->scp) > 0)
    {
      static struct cgp cgp;
      cgp.cf = sdp->f2.cf;
      cgp.gw = sdp->f2.gw;
      cgp.pos = sdp->f2.pos;
      sdp->cgp_closed = cgp_str(&cgp,0);
    }
}
