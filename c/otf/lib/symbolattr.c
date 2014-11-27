#include <unistd.h>
#include <string.h>
#include <psd_base.h>
#include <runexpat.h>
#include <fname.h>
#include <hash.h>
#include <npool.h>
#include <memblock.h>
#include "./warning.h"
#include "symbolattr.h"

static Hash_table *symbolattr_hash;
static struct mb *symbolattr_mem;
static struct npool *symbolattr_pool;

void
symbolattr_init(void)
{
  if (!symbolattr_hash)
    {
      symbolattr_hash = hash_create(1);
      symbolattr_mem = mb_init(sizeof(struct symbolattr), 16);
      symbolattr_pool = npool_init();
    }
}

struct symbolattr *
symbolattr_get(const char *txtid, const char *sym)
{
  char buf[32];
  struct symbolattr *sa = NULL;
  sprintf(buf,"a_%s#%s",txtid,sym);
  sa = hash_find(symbolattr_hash, (unsigned char *)buf);
  if (sa)
    ; /*fprintf(stderr,"found symbolattr %s => { symbol=%s; qualified_id=%s; pname=%s; }\n", buf, sa->symbol, sa->qualified_id, sa->pname);*/
  else
    fprintf(stderr,"symbolattr %s not found\n", buf);
  return sa;
}

void
symbolattr_map(const char *txtid,const char *from, const char *to)
{
  char buf[32];
  struct symbolattr *sa = NULL;
  sprintf(buf,"a_%s#%s",txtid,to);
  sa = hash_find(symbolattr_hash, (unsigned char *)buf);
  if (sa)
    {
      sprintf(buf,"a_%s#%s",txtid,from);
      hash_add(symbolattr_hash, npool_copy((unsigned char *)buf, symbolattr_pool),sa);
      /*fprintf(stderr,"mapping %s => %s \n", buf, to);*/
    }
  else
    fprintf(stderr,"symbol %s not found; key: symbol-alias %s=>%s disabled\n", buf, from, to);
}

void
symbolattr_put(const char *txtid, const char *sym, const char *idp, const char *pname)
{
  struct symbolattr *sa = mb_new(symbolattr_mem);
  char buf[32];
  sa->symbol = (char*)npool_copy((unsigned char *)sym, symbolattr_pool);
  sa->qualified_id = (char*)idp;
  sa->pname = (char*)pname;
  sprintf(buf,"a_%s#%s",txtid,sym);
  /*fprintf(stderr,"saving symbolattr %s => { symbol=%s; qualified_id=%s; pname=%s; }\n", buf, sym, idp, pname);*/
  hash_add(symbolattr_hash, npool_copy((unsigned char *)buf,symbolattr_pool),sa);
}

void
symbolattr_term(void)
{
  if (symbolattr_hash)
    {
      hash_free(symbolattr_hash, NULL);
      mb_term(symbolattr_mem);
      npool_term(symbolattr_pool);
      symbolattr_hash = NULL;
    }
}
