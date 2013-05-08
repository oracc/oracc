#include <string.h>
#include <ctype128.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "inctrie.h"
extern char *file;
extern size_t lnum;
static struct inctrie *subdig_trie, *empty_trie;

static const char *subdig_tab[] =
  {
    "0", "\xe2\x82\x80",
    "1", "\xe2\x82\x81",
    "2", "\xe2\x82\x82",
    "3", "\xe2\x82\x83",
    "4", "\xe2\x82\x84",
    "5", "\xe2\x82\x85",
    "6", "\xe2\x82\x86",
    "7", "\xe2\x82\x87",
    "8", "\xe2\x82\x88",
    "9", "\xe2\x82\x89",
    "x", "\xe2\x82\x93",
    NULL
  };

void
subdig_init(void)
{
  int i;
  subdig_trie = inctrie_create();
  for (i = 0; subdig_tab[i]; i += 2)
    inctrie_insert(subdig_trie, subdig_tab[i], subdig_tab[i+1], 0);
  empty_trie = inctrie_create();
}

void
subdig_term(void)
{
  inctrie_free(subdig_trie);
  subdig_trie = NULL;
  inctrie_free(empty_trie);
  empty_trie = NULL;
}

const unsigned char *
subdig(const unsigned char *str, const unsigned char *end)
{
  return inctrie_map(subdig_trie,
		     (const char *)str,
		     (const char *)end,
		     1,NULL,NULL,file,lnum);
}

const unsigned char *
cpydig(const unsigned char *str, const unsigned char *end)
{
  return inctrie_map(empty_trie,
		     (const char *)str,
		     (const char *)end,
		     1,NULL,NULL,file,lnum);
}
