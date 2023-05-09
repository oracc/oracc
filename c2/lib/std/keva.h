#ifndef KEVA_H_
#define KEVA_H_

struct keva
{
  const char *k;
  const char *v;
};

typedef struct keva Keva;

Keva *keva_create(Memo *kevamem, const char *key, const char *value);

#endif/*KEVA_H_*/
