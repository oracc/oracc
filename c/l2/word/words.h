#ifndef _WORDS_H
#define _WORDS_H

enum w2_match { W2_NONE , W2_PARTIAL , W2_FULL , W2_UNKNOWN };

enum verb_noise { vn_to , vn_be , vn_cause , vn_come , vn_become , 
		  vn_make , vn_with , vn_none };

struct tab
{
    const char *name;
    enum verb_noise vn;
};

struct w2_set
{
  Uchar *literal;
  Uchar **keys;
  int nkeys;
  int pct;
};

extern enum w2_match w2_subset(struct w2_set *set1, struct w2_set *set2);
extern struct w2_set * w2_create_set(const unsigned char *words);
extern struct tab *common (register const char *str, register unsigned int len);
extern void w2_init(void);
extern void w2_term(void);

#endif /*_WORDS_H*/
