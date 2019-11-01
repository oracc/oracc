#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include "hash.h"
#include "list.h"
#include "npool.h"
#include "memblock.h"
#include "words.h"
#include "misc.h"

int wordset_debug = 0;

static int defseps[256];
static int keyseps[256];

static int curr_seg_len = 0;

static const char *defsep_chars = ".,;:/";
static const char *keysep_chars = " .,;:/[](){}"; /* - */

static Uchar *getkey_next;

static struct mb *mb_w2_sets = NULL;
static struct mb *mb_w2_keys = NULL;
static struct npool *w2_pool = NULL;

static void getkey_init(void);
static const Uchar *getkey(const Uchar *str);

#if 0
static int
pct(int amount, int total)
{
  float i = amount;
  i /= total;
  i *= 100;
  return (int)(i+.5);
}
#endif

void
w2_init(void)
{
  if (!mb_w2_sets)
    {
      mb_w2_sets = mb_init(sizeof(struct w2_set), 32);
      mb_w2_keys = mb_init(sizeof(Uchar *), 4);
      w2_pool = npool_init();
      getkey_init();
    }
}

void
w2_term(void)
{
  if (mb_w2_sets)
    {
      mb_free(mb_w2_sets);
      mb_w2_sets = NULL;
      mb_free(mb_w2_keys);
      mb_w2_keys = NULL;
      npool_term(w2_pool);
      w2_pool = NULL;
    }
}

static enum w2_match
compare_sequences(struct w2_set *set1, int start1, int top1,
		  struct w2_set *set2, int start2, int top2)
{
  int hits = 0;
  int i;
  int seg_nkeys = 0;
  for (i = start2; i < top2; ++i)
    {
      int found = 0, j;
      for (j = start1; j < top1; ++j)
	{
	  if (!strcmp((char*)set2->keys[i], (char*)set1->keys[j]))
	    {
	      ++found;
	      break;
	    }
	}
      if (found)
	++hits;
      else
	return W2_NONE; /* a positive miss means this can't be a subset */
    }
  if (hits)
    {
      for (i = start1; i < set1->nkeys; ++i)
	if (NULL==set1->keys[i])
	  break;
	else
	  ++seg_nkeys;
    }
  if (wordset_debug)
    fprintf(stderr, "seg_nkeys = %d\n", seg_nkeys);
  if (hits >= seg_nkeys /*set1->nkeys*/)
    {
      set2->pct = 100; /*pct(hits,seg_nkeys)*/ /*set1->nkeys*/
      return W2_FULL;
    }
  else if (hits)
    {
      set2->pct = pct(hits,seg_nkeys /*set1->nkeys*/);
      return W2_PARTIAL;
    }
  else
    {
      set2->pct = 0;
      return W2_NONE;
    }
}

struct w2_set *
w2_create_set(const Uchar *word_set)
{
  struct w2_set *set = mb_new(mb_w2_sets);
  const Uchar *k;
  int index = 0;

  if (!strncmp((const char *)word_set, "(to be) ", strlen("(to be) ")))
    word_set += strlen("(to be) ");
  else if (!strncmp((const char *)word_set, "to ", strlen("to ")))
    word_set += strlen("to ");
  else if (!strncmp((const char *)word_set, "a ", strlen("a ")))
    word_set += strlen("a ");    
  
  while ((k = getkey(word_set)))
    ++set->nkeys;
  getkey(NULL);

  set->keys = mb_new_array(mb_w2_keys,set->nkeys);

  while ((k = getkey(word_set)))
    {
      if (!*k)
	set->keys[index++] = NULL;
      else
	set->keys[index++] = npool_copy(k, w2_pool);
    }
  getkey(NULL);
  if (wordset_debug)
    {
      int i;
      fprintf(stderr, "wordset for %s=", word_set);
      for (i = 0; i < set->nkeys; ++i)
	{
	  fprintf(stderr, "%s", set->keys[i]);
	  if ((i+1) < set->nkeys)
	    fprintf(stderr, "; ");
	}
      fprintf(stderr, "\n");
    }
  return set;
}

static void
getkey_init(void)
{
  const char *p = keysep_chars;
  while (*p)
    keyseps[(int)*p++] = 1;
  p = defsep_chars;
  while (*p)
    defseps[(int)*p++] = 1;
}

static int
white_square(const Uchar *s)
{
  int w = s[0] == 0xe2 && s[1] == 0x9f && (s[2] == 0xa6 || s[2] == 0xa7);
  /* fprintf(stderr, "%s: w = %d; %x.%x.%x\n", s, w,s[0],s[1],s[2]); */
  return w;
}

static void
copy_sans_non_key_chars(const Uchar *str, Uchar *bits)
{
  register const Uchar *src = str;
  register Uchar *dst = bits;
  while (*src)
    {
      if (*src < 128 || !white_square(src))
	*dst++ = *src++;
      else
	src += 3;
    }
  *dst = '\0';
}

static const Uchar *
getkey(const Uchar *str)
{
  static const Uchar *curr_str = NULL;
  static Uchar *bits = NULL;
  static int defsep = 0, bits_len = 0;
  Uchar *end;
  const Uchar *ret;

  if (!str)
    {
      curr_str = NULL;
      free(bits);
      bits = NULL;
      bits_len = 0;
      return NULL;
    }

  if (str != curr_str)
    {
      curr_str = str;
      if ((strlen((char*)curr_str)+1) > bits_len)
	{
	  bits_len += 1024;
	  bits = realloc(bits,bits_len);
	}
      copy_sans_non_key_chars(str,bits);
      getkey_next = bits;
      while (keyseps[(int)*getkey_next])
	++getkey_next;
    }

  if (defsep)
    {
      defsep = 0;
      return (const Uchar*)"";
    }

  if (*getkey_next)
    {
      ret = end = getkey_next;
      while (*end && !keyseps[(int)*end])
	++end;
      if (*end)
	{
	  if (defseps[(int)*end])
	    defsep = 1;
	  *end++ = '\0';
	  while (keyseps[(int)*end])
	    ++end;
	  getkey_next = end;
	}
      else
	getkey_next = end;
      return ret;
    }
  else
    return NULL;
}

static int
next_top(struct w2_set *set, int start)
{
  if (start >= set->nkeys)
    return -1;
  while (start < set->nkeys)
    {
      if (!set->keys[start])
	{
	  curr_seg_len = 0;
	  return start;
	}
      else
	++start;
    }
  return start;
}

/* return:
   0 : set2 cannot be a subset of set1 (no hits)
   1 : set2 is a partial subset of set1 (hits > 0 but < set1->nkeys)
   2 : set2 is a full match for set1 (hits == set1->nkeys)
 */
enum w2_match
w2_subset(struct w2_set *set1, struct w2_set *set2)
{
  enum w2_match best_res = W2_NONE;
  int best_pct = 0;
  int set1_top = -1, set2_top = -1, set1_start = 0, set2_start = 0;

  while ((set1_top = next_top(set1, (set1_start = ++set1_top))) > 0)
    {
      while ((set2_top = next_top(set2, (set2_start = ++set2_top))) > 0)
	{
	  int res = compare_sequences(set1, set1_start, set1_top,
				      set2, set2_start, set2_top);
	  if (res == 2)
	    return W2_FULL;
	  else if (res == 1)
	    {
	      if (set2->pct > best_pct)
		best_pct = set2->pct;
	      best_res = W2_PARTIAL;
	    }
	}
    }
  if (best_res == W2_PARTIAL)
    set2->pct = best_pct;
  return best_res;
}
