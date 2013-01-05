#ifndef _BLOCKTOK_H
#define _BLOCKTOK_H
#include "tree.h"
#include "xmlnames.h"
struct block_token
{
  const char *name;
  enum e_type etype;
  const char *n;
  enum block_levels type;
  const char *full;
  const char *abbr;
  const char *nano;
};
extern struct block_token *blocktok(register const char*str,
				    register unsigned int len);
#endif /*_BLOCKTOK_H*/
