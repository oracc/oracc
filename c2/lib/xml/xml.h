#ifndef XML_H_
#define XML_H_	1

#ifndef uccp
#define uccp unsigned const char *
#endif

struct xmlhelper {
  FILE *fp;
};

typedef struct xmlhelper Xmlhelper;

extern Xmlhelper *xmlh_init(FILE *fp);
extern const unsigned char *xmlify(const unsigned char *);

#endif /* XML_H_ */
