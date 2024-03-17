#ifndef _ATF2UTF8_H
#define _ATF2UTF8_H
unsigned char *atf2utf8(register const unsigned char *s, int rx);
void atf2utf8_init(void);
int is_signlist(register const unsigned char *s);
unsigned char *utf8_times(void);

#endif /*ATF2UTF8_H*/
