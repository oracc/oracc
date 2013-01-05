/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: bit_mac.h,v 0.3 1997/09/08 14:49:55 sjt Exp $
*/

#ifndef BIT_MAC_H_
#define BIT_MAC_H_ 1

/* Useful general macros for bit-fiddling */
#define BIT_CLR(flags,bit)	(flags &= ~(bit))
#define BIT_SET(flags,bit)	(flags |= (bit))
#define BIT_ISSET(flags,bit)	(flags & (bit))
#define BIT_ONLY(flags,bit)  	((flags & (bit)) == (bit))

#define BIT_CLEAR BIT_CLR

#endif /* BIT_MAC_H_ */
