#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <psd_base.h>
#include <runexpat.h>
#include <fname.h>
#include <hash.h>
#include <npool.h>
#include <memblock.h>
#include "./warning.h"
#include "key.h"

static struct mb *key_mem;
static struct npool *key_pool;

void
key_init(void)
{
  if (!key_mem)
    {
      key_mem = mb_init(sizeof(struct keypair), 16);
      key_pool = npool_init();
    }
}

struct keypair *
key_parse(unsigned char *lp)
{
  struct keypair *kp = mb_new(key_mem);
  lp = kp->key = (char*)npool_copy(lp, key_pool);
  while (*lp && (*lp >128 || !isspace(*lp)))
    ++lp;
  if (*lp)
    {
      *lp++ = '\0';
      while (*lp && (*lp > 128 || isspace(*lp)))
	++lp;
      kp->val = (char*)lp;
      while (*lp && (*lp >128 || !isspace(*lp)))
	++lp;
      if (*lp)
	*lp++ = '\0';
      while (*lp && (*lp > 128 || isspace(*lp)))
	++lp;
      if (*lp)
	kp->url = lp;
      while (*lp && (*lp >128 || !isspace(*lp)))
	++lp;
      if (*lp)
	*lp = '\0';
      if (!kp->url && kp->val && !strncmp(kp->val,"http",4))
	{
	  kp->url = kp->val;
	  kp->val = "";
	}
    }
  return kp;
}

void
key_term(void)
{
  if (key_mem)
    {
      mb_term(key_mem);
      npool_term(key_pool);
      key_mem = NULL;
    }
}
