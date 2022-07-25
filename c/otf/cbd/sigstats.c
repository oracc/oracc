#include "gx.h"
#include "sx.h"
#include <stdlib.h>
#include <ctype128.h>

#define JOINER ''
#define JOINER_s ""

static int one = 1;
static unsigned char *curr_cgp = NULL;

struct stats;
static char *stats_key_maker(struct npool *pool, const char *cgp, ...);
static char *keyfnc_eb(struct stats *sip, struct f2*f2p);

struct stats {
  const char *name;
  struct npool *pool;
  Hash_table *hash;
  char * (*keyfnc)(struct stats *sip, struct f2 *f2p);
} stats[] = {
  { "eb" , NULL, NULL, &keyfnc_eb }, 
  { NULL , NULL, NULL, NULL }
};

static char *
keyfnc_eb(struct stats *sip, struct f2 *f2p)
{
  return stats_key_maker(sip->pool, (ccp)curr_cgp, f2p->base, NULL);
}

static void
stats_entry_init_one(struct stats *sip, struct npool *pool)
{
  sip->pool = pool;
  sip->hash = hash_create(1024);
}

static void
stats_entry_init(unsigned char *cgp)
{
  int i;
  stats[0].pool = npool_init();
  stats[0].hash = hash_create(1024);
  curr_cgp = npool_copy(cgp, stats[0].pool);
  for (i = 1; stats[i].name; ++i)
    stats_entry_init_one(&stats[i], stats[0].pool);
}

static void
stats_entry_term_one(struct stats *sip)
{
  hash_free(sip->hash, NULL);
}

static void
stats_entry_term(void)
{
  int i;
  curr_cgp = NULL;
  npool_term(stats[0].pool);
  for (i = 0; stats[i].name; ++i)
    stats_entry_term_one(&stats[i]);
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
      len = strlen(cgp) + 1;
      va_start(va, cgp);
      while ((more = va_arg(va, const char *)))
	len += strlen(more) + 1;
      va_end(va);
      buf = malloc(len+1);
      strcpy(buf, cgp);
      strcat(buf, JOINER_s);
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
stats_add_key_insts(struct stats *sip, const char *key, const char **ip)
{
  Hash_table *hp = NULL;

  if (!(hp = hash_find(sip->hash, (ucp)key)))
    {
      hp = hash_create(1024);
      hash_add(sip->hash, npool_copy((ucp)key, sip->pool), hp);
    }
  while (*ip)
    hash_add(hp, (ucp)*ip++, &one);
}

void
stats_collect(struct f2 *f2p, const char **insts)
{
  char *buf = NULL;
  int i;
  for (i = 0; stats[i].name; ++i)
    {
      if ((buf = stats[i].keyfnc(&stats[i], f2p)))
	stats_add_key_insts(&stats[i], buf, insts);
      else
	fprintf(stderr, "sigstats: NULL return from stats_key_maker\n");
    }
}

static const char *
last_component(const char *key)
{
  const char *j = strrchr(key, JOINER);
  return j+1;
}

static void
stats_print_one(struct stats *sip)
{
  const char **k = NULL;
  int nk = 0, i;
  k = hash_keys2(sip->hash, &nk);
  for (i = 0; i < nk; ++i)
    {
      int ni = 0, j;
      Hash_table *hp = NULL;
      char **ip = NULL;

      hp = hash_find(sip->hash, (ucp)k[i]);
      ip = (char **)hash_keys2(hp, &ni);

      /* Possibly qsort ip here; cmp fnc needs to skip projects and sort by PQX */

      printf("eb\t%s\t", last_component(k[i]));
      for (j = 0; j < ni; ++j)
	{
	  if (j && j < ni)
	    printf(" ");
	  printf("%s", ip[j]);
	}
      printf("\n");

      hash_free(hp, NULL);
    }
}

void
stats_print(int ninsts)
{
  int i;
  printf("@@%s\t%d\n", curr_cgp, ninsts);
  for (i = 0; stats[i].name; ++i)
    stats_print_one(&stats[i]);
}

void
sigstats(const char *f)
{
  FILE *f_in = NULL;
  unsigned char *lp = NULL;
  char *iid_copy;
  struct npool *pool = NULL;
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
#if 0
              npool_term(pool);
	      hash_free(h_eb, NULL);
              hash_free(iid_hash, NULL);
#endif
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
	      iid_copy = (char*)npool_copy(inst, pool);
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
		  /* stats_eb(curr_cgp, &f2, (const char **)instp, pool); */
		}
	    }
	}
    }
  if (curr_cgp)
    {
      stats_print(ninsts);
      stats_entry_term();
#if 0
      npool_term(pool);
      hash_free(h_eb, NULL);
      hash_free(iid_hash, NULL);
#endif
    }
  xgetline(NULL);
  xfclose(f,f_in);
  sig_context_term();
}
