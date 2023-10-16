#ifndef XMLIFY_H_
#define XMLIFY_H_	1

#ifndef uccp
#define uccp unsigned const char *
#endif

typedef const unsigned char * (*xmlify_ptr)(const unsigned char *c);
extern const unsigned char *xmlify_not(const unsigned char *c);
extern const unsigned char *xmlify_yes(const unsigned char *c);
extern const char *xmlify_char(int c);

extern xmlify_ptr xmlify;

#endif /* XMLIFY_H_ */
