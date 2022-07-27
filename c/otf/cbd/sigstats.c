#include "gx.h"
#include "sx.h"
#include <stdlib.h>
#include <ctype128.h>

enum kf { KF_ENTRY , KF_SENSE , KF_NONE };

#define KF_CGP_OR_SENSE (ccp)(kftype==KF_ENTRY ? curr_cgp : f2p->sense)

#define JOINER ''
#define JOINER_s ""

static int one = 1;
static unsigned char *curr_cgp = NULL;
static int inst_cmp(const void *a, const void *b);
/*static int key_cmp(const void *a, const void *b);*/
static int str_cmp(const void *a, const void *b);

struct stats;
static char *stats_key_maker(struct npool *pool, const char *cgp, ...);

static char *keyfnc_b(struct stats *sip, struct f2*f2p, enum kf kftype);
static char *keyfnc_m(struct stats *sip, struct f2*f2p, enum kf kftype);

struct funcs {
  const char *name;
  char * (*keyfnc)(struct stats *sip, struct f2 *f2p, enum kf kftype);
} keyfncs[] = {
  { "b" , &keyfnc_b }, 
  { "m" , &keyfnc_m }, 
  { NULL , NULL }
};

int nfunc = sizeof(keyfncs) / sizeof(struct funcs);

struct stats {
  const unsigned char *label;
  struct npool *pool;
  Hash_table *senses;
  Hash_table **hashes;
} entry;

static char *
keyfnc_b(struct stats *sip, struct f2 *f2p, enum kf kftype)
{
  if (f2p->base)
    return stats_key_maker(sip->pool, KF_CGP_OR_SENSE, f2p->base, NULL);
  else
    return NULL;
}

static char *
keyfnc_m(struct stats *sip, struct f2 *f2p, enum kf kftype)
{
  if (f2p->morph)
    return stats_key_maker(sip->pool, KF_CGP_OR_SENSE, f2p->morph, NULL);
  else
    return NULL;
}

Hash_table **
stats_hashes_init(void)
{
  int i;
  Hash_table **hashes = malloc(nfunc * sizeof(Hash_table *));
  for (i = 0; i < nfunc; ++i)
    hashes[i] = hash_create(1024);
  return hashes;
}

void
stats_hashes_term(Hash_table **hp)
{
  int i = 0;
  for (i = 0; i < nfunc; ++i)
    hash_free(hp[i], NULL);
  free(hp);
}

static struct stats *
stats_sense_init(unsigned char *s)
{
  struct stats *ssp = calloc(1, sizeof(struct stats));
  ssp->label = s;
  ssp->hashes = stats_hashes_init();
  ssp->pool = entry.pool;
  return ssp;
}

static void
stats_sense_term(struct stats *ssp)
{
  stats_hashes_term(ssp->hashes);
  free(ssp);
}

static void
stats_entry_init(unsigned char *cgp)
{
  entry.pool = npool_init();
  entry.senses = hash_create(1024);
  entry.label = curr_cgp = npool_copy(cgp, entry.pool);
  entry.hashes = stats_hashes_init();
}

static void
stats_entry_term(void)
{
  curr_cgp = NULL;
  hash_free(entry.senses, (void (*)(void*))stats_sense_term);
  npool_term(entry.pool);
  stats_hashes_term(entry.hashes);
}

static char *
stats_key_maker(struct npool *pool, const char *cgp, ...)
{
  if (NULL != cgp)
    {
      char *buf = NULL;
      int len = 0;
      va_list va;
      const char *more, *tmp;
#if 0
      if (!strchr(cgp,'['))
	len = strlen(cgp) + 1;
#endif

      va_start(va, cgp);
      while ((more = va_arg(va, const char *)))
	len += strlen(more) + 1;
      va_end(va);
      buf = malloc(len+1);

#if 1
      *buf = '\0';
#else
      if (!strchr(cgp,'['))
	{
	  strcpy(buf, cgp);
	  strcat(buf, JOINER_s);
	}
      else
	*buf = '\0';
#endif
      
      va_start(va, cgp);
      while ((more = va_arg(va, const char *)))
	{
	  strcat(buf, more);
	  strcat(buf, JOINER_s);
	}
      va_end(va);
      buf[len-1] = '\0';
      tmp = (ccp)npool_copy((ucp)buf, pool);
      free(buf);
      return (char*)tmp;
    }
  return NULL;
}

static void
stats_add_key_insts(Hash_table *keys, const char *key, const char **ip)
{
  Hash_table *hp = NULL;

  if (!(hp = hash_find(keys, (ucp)key)))
    {
      hp = hash_create(1024);
      hash_add(keys, npool_copy((ucp)key, entry.pool), hp);
    }
  while (*ip)
    hash_add(hp, (ucp)*ip++, &one);
}

void
stats_collect(struct f2 *f2p, const char **insts)
{
  char *buf = NULL;
  int i;
  for (i = 0; keyfncs[i].name; ++i)
    {
      if ((buf = keyfncs[i].keyfnc(&entry, f2p, KF_ENTRY)))
	{
	  struct stats *sstats = NULL;
	  stats_add_key_insts(entry.hashes[i], buf, insts);
	  if (!(sstats = hash_find(entry.senses, f2p->sense)))
	    {
	      unsigned char *sense = npool_copy(f2p->sense, entry.pool);
	      sstats = stats_sense_init(sense);
	      hash_add(entry.senses, sense, sstats);
	    }
	  buf = keyfncs[i].keyfnc(sstats, f2p, KF_SENSE);
	  stats_add_key_insts(sstats->hashes[i], buf, insts);
	}
      /* OK to get NULL return; happens when there is no BASE and fnc is for b, for example */
    }
}

#if 0
static const char *
last_component(const char *key)
{
  const char *j = strrchr(key, JOINER);
  return j+1;
}
#endif

static void
stats_print_stats(struct stats *sip)
{
  const char **kp = NULL;
  int nk = 0, i, j;

  if (sip->senses)
    printf("@@%s\n", sip->label);
  else
    printf("@@@%s\n", sip->label);
  
  for (i = 0; i < nfunc; ++i)
    {
      kp = hash_keys2(sip->hashes[i], &nk);
      /* sort the keys, e.g., bases or morphs */
      qsort(kp, nk, sizeof(const char *), (__compar_fn_t)str_cmp);

      for (j = 0; j < nk; ++j)
	{
	  int ni = 0, k;
	  Hash_table *hp = NULL;
	  char **ip = NULL;
	  
	  hp = hash_find(sip->hashes[i], (ucp)kp[j]);
	  ip = (char **)hash_keys2(hp, &ni);

	  qsort(ip, ni, sizeof(const char *), (__compar_fn_t)inst_cmp);

	  printf("%s\t%s\t", keyfncs[i].name, kp[j]);
	  for (k = 0; k < ni; ++k)
	    {
	      if (k && k < ni)
		printf(" ");
	      printf("%s", ip[k]);
	    }
	  printf("\n");

	  free(ip);
	  hash_free(hp, NULL);
	}
      free(kp);
    }
}

void
stats_print(int ninsts)
{
  int i, nsk;
  const char **sensekeys = NULL;
#if 0
  struct stats **senses = NULL;
#endif

  sensekeys = hash_keys2(entry.senses, &nsk);
  qsort(sensekeys, nsk, sizeof(const char *), str_cmp);
#if 0
  senses = malloc(nsk * sizeof(struct stat *));
  for (i = 0; i < nsk; ++i)
    senses[i] = hash_find(entry.senses, sensekeys[i]);
#endif

  stats_print_stats(&entry);
  for (i = 0; i < nsk; ++i)
    stats_print_stats(hash_find(entry.senses, (ucp)sensekeys[i]));
  free(sensekeys);
}

void
sigstats(const char *f)
{
  FILE *f_in = NULL;
  unsigned char *lp = NULL;
  char *iid_copy;
  int ninsts = 0, lnum = 0;
  struct sig_context *scp;
  struct f2 f2;

  if (strcmp(f, "-"))
    f_in = xfopen(f, "r");
  else
    f_in = stdin;

  scp = sig_context_init();
    
  while ((lp = xgetline(f_in)))
    {
      ++lnum;
      if ('@' == lp[0] && '@' == lp[1])
	{
          unsigned char *tmp = NULL;

          if (curr_cgp)
            {
              stats_print(ninsts);
	      stats_entry_term();
            }

          tmp = (ucp)strchr((ccp)lp, '\t');
          if (tmp)
            *tmp++ = '\0';
	  /* tmp is now pointing at the number of sigs that follow this cgp; we don't use that atm */
	  tmp = (ucp)strchr((ccp)tmp, '\t');
	  if (tmp)
	    {
	      *tmp++ = '\0';
	      /* now tmp is pointing at the instance count for the cgp */
	      ninsts = atoi((ccp)tmp);
	    }

#if 1
	  /* per entry initialization here */
	  stats_entry_init(lp+2);
#else
          pool = npool_init();
          h_eb = hash_create(1024);
#endif
       	}
      else
        {
          unsigned char *sig = lp, *freq, *inst = NULL;
          freq = (ucp)strchr((const char *)sig,'\t');
          if (freq)
            {
              *freq++ = '\0';
              inst = (ucp)strchr((ccp)freq, '\t');
	      if (inst)
		{
		  *inst++ = '\0';
		  if (strchr((ccp)inst, '\t'))
		    {
		      fprintf(stderr, "sx -m : input lines only allowed <SIG><TAB><INSTS>\n");
		      exit(1);
		    }
		}
	    }
	  if (inst)
	    {
	      iid_copy = (char*)npool_copy(inst, entry.pool);
	      memset(&f2, '\0', sizeof(struct f2));
	      if (f2_parse((ucp)f,lnum,lp,&f2,NULL,scp) > 0)
		{
		  /* This is where we iterate over all the stats aggregators */
		  char **instp = NULL, *s;
		  int i;
		  for (i = 0, s = iid_copy; *s; )
		    {
		      if (!isspace(*s))
			{
			  ++i;
			  while (*s && !isspace(*s))
			    ++s;
			}
		      while (isspace(*s))
			++s;
		    }

		  instp = malloc(++i * sizeof(char*));		  
		  for (i = 0, s = iid_copy; *s; )
		    {
		      if (!isspace(*s))
			{
			  instp[i++] = s;
			  while (*s && !isspace(*s))
			    ++s;
			}
		      if (isspace(*s))
			{
			  *s++ = '\0';
			  while (isspace(*s))
			    ++s;
			}
		    }
		  instp[i] = NULL;
		  stats_collect(&f2, (const char **)instp);
		  /* The instances are added to hashes so this array of pointers is done with */
		  free(instp);
		}
	    }
	}
    }
  if (curr_cgp)
    {
      stats_print(ninsts);
      stats_entry_term();
    }
  xgetline(NULL);
  xfclose(f,f_in);
  sig_context_term();
}

static int
inst_cmp(const void *va,const void *vb)
{
  int ret = 0;
  int ai, bi;
  const char *a = *(char*const*)va;
  const char *b = *(char*const*)vb;
  const char *ap, *bp;

  ap = strchr(a, ':');
  if (ap)
    a = ap;
  bp = strchr(b, ':');
  if (bp)
    b = bp;
  
  ret = strcmp(a,b);
  if (!ret)
    return 0;
  
  ai = atoi(a+1);
  bi = atoi(b+1);
  if (ai != bi)
    return ai - bi;

  a = strchr(a,'.');
  b = strchr(b,'.');
  if (a && b)
    {
      ai = atoi(a+1);
      bi = atoi(b+1);
      if (ai != bi)
	return ai - bi;

      a = strchr(a+1,'.');
      b = strchr(b+1,'.');
      if (a && b)
	{
	  ai = atoi(a+1);
	  bi = atoi(b+1);
	  if (ai != bi)
	    return ai - bi;
	  else
	    return 0;
	}
      else
	{
	  if (a)
	    return 1;
	  else
	    return -1;
	}
    }
  else
    {
      if (a)
	return 1;
      else
	return -1;
    }
  return 0;
}


#if 0
static int
key_cmp(const void *va,const void *vb)
{
  const char *a = *(char*const*)va;
  const char *b = *(char*const*)vb;

  if (!strchr(a, JOINER))
    {
      if (!strchr(b, JOINER))
	return 0;
      else
	return -1;
    }
  else if (!strchr(b, JOINER))
    return 1;

  return strcmp(a, b);
}
#endif

static int
str_cmp(const void *a, const void *b)
{
  return strcmp(*(char* const*) a, *(char* const*) b);
}

