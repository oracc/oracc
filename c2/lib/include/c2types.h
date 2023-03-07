#ifndef C2TYPES_H_
#define C2TYPES_H_ 1

#include <limits.h>
#include <stdint.h>

typedef unsigned int Bitfield;

typedef char Boolean;
#undef TRUE
#undef FALSE
#define FALSE           (Boolean)0
#define TRUE            (Boolean)1

typedef unsigned char Uchar;	/* for 255 char text */
typedef char Signed8;		/* for small signed numbers */
typedef unsigned char Unsigned8;/* for small unsigned numbers */
typedef int Integer;		/* best size for machine */
typedef unsigned int Uint;
typedef long int Int;		/* best size for avoiding 16/32 problems */
typedef int16_t Signed16;
typedef uint16_t Unsigned16;
typedef int32_t Signed32;
typedef uint32_t Unsigned32;

#endif /*C2TYPES_H_*/
