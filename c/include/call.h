/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: call.h,v 0.3 1997/09/08 14:49:55 sjt Exp $
*/

#ifndef CALL_H_
#define CALL_H_ 1

#if CALL_WITH_FORK
#elif CALL_WITH_SPAWN
#include <process.h>
#elif CALL_WITH_SYSTEM
#endif

#define CALL_SYS_RET(_i) ((_i)&0x000000ff)
#define CALL_PRG_RET(_i) ((_i)&0x0000ff00)

extern int call (const char *obligatory_arg, ...);
extern int call_shell (const char *obligatory_arg, ...);

#endif /* CALL_H_ */
