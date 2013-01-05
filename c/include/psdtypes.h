/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: psdtypes.h,v 0.3 1997/09/08 14:49:58 sjt Exp $
*/

#ifndef PSDTYPES_H_
#define PSDTYPES_H_ 1

#include <limits.h>
#include <stdint.h>

/**PSD programs rarely use basic data-types---only for communication
 * of flag values via int's etc. This header defines the types which
 * are used, and gives aliases for the ...MAX values to avoid 
 * system dependencies wherever possible.
 */
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

#if defined (_QC) || defined (__TURBOC__)
typedef char Void_p;
#else
typedef void Void_p;
#endif

#endif /*PSDTYPES_H_*/
