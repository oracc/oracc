/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: xmem.h,v 0.3 1997/09/08 14:50:01 sjt Exp $
*/

#ifndef XMEM_H_
#define XMEM_H_ 1

#define yfree(p) free(p),p=NULL
#define xfree(p) xxfree((void**)p)
extern void xxfree (void **);
extern void *xmalloc (size_t);
extern void *xcalloc (size_t,size_t);
extern void *xrealloc (void*, size_t);

#endif /* XMEM_H_*/

