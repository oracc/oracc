#ifndef CTYPE128_
#define CTYPE128_

#include <ctype.h>

#define ORACC_isalnum(c) (((unsigned)(c))<128&&isalnum(((unsigned)(c))))
#define ORACC_isalpha(c) (((unsigned)(c))<128&&isalpha(((unsigned)(c))))
#define ORACC_isdigit(c) (((unsigned)(c))<128&&isdigit(((unsigned)(c))))
#define ORACC_islower(c) (((unsigned)(c))<128&&islower(((unsigned)(c))))
#define ORACC_isspace(c) (((unsigned)(c))<128&&isspace(((unsigned)(c))))
#define ORACC_isupper(c) (((unsigned)(c))<128&&isupper(((unsigned)(c))))

#define isalnum(c) ORACC_isalnum(c)
#define isalpha(c) ORACC_isalpha(c)
#define isdigit(c) ORACC_isdigit(c)
#define islower(c) ORACC_islower(c)
#define isspace(c) ORACC_isspace(c)
#define isupper(c) ORACC_isupper(c)

#endif/*CTYPE128_*/
