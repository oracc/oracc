#ifndef KEY_
#define KEY_

struct keypair
{
  char *key;
  char *val;
  char *url;
};

extern struct keypair *key_parse(unsigned char *lp);
extern void key_init(void);
extern void key_term(void);

#endif/*KEY_*/
