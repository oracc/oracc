/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: messages.h,v 0.5 1997/09/08 14:49:57 sjt Exp s $
*/

#ifndef _MESSAGES_H_
#define _MESSAGES_H_ 1

#include <bit_mac.h>
#include <exitcode.h>
#include <file_io.h>
#include <dra.h>

#define	MSG_PROGRESS	0x01
#define MSG_MESSAGE	0x02
#define MSG_WARNING	0x04
#define MSG_ERROR	0x08
#define MSG_FATAL	0x10
#define MSG_STDERR	0x20
#define MSG_DEBUG	0x40
#define MSG_UERROR	0x80

#define MSG_DEFAULTS	MSG_MESSAGE|MSG_WARNING|MSG_ERROR|MSG_FATAL|MSG_STDERR|MSG_UERROR

#define MSG_SET(bits)		BIT_SET(_message_flags,(bits))
#define MSG_CLR(bits)		BIT_CLR(_message_flags,(bits))
#define MSG_ISSET(bits)		BIT_ISSET(_message_flags,(bits))
#define MSG_TYPE(bits)		BIT_ONLY(_msg_invoke_flag,(bits))

#define progress	_msg_invoke_flag=MSG_PROGRESS,_message_kernel_nofile
#undef message
#define message		_msg_invoke_flag=MSG_MESSAGE,_message_kernel_nofile
#define xmessage       	_msg_invoke_flag=MSG_MESSAGE,_message_kernel_nofile
#define mwarning 	_msg_invoke_flag=MSG_WARNING,_message_kernel
#define error   	_msg_invoke_flag=MSG_ERROR,_message_kernel
#define usage_error   	_msg_invoke_flag=MSG_UERROR,_message_kernel
#define fatal()		_msg_invoke_flag=MSG_FATAL,_message_kernel(ewfile(__FILE__,__LINE__), "fatal program error")
#define debug   	_msg_invoke_flag=MSG_DEBUG,_message_kernel_nofile

#define HISTORY_CLEAN	EXIT_CLEAN
#define HISTORY_WARNING	EXIT_WARNING
#define HISTORY_ERROR	EXIT_ERROR

extern int history;
extern Boolean doing_debug;
extern Boolean err_newline;
extern Boolean exit_on_error;
extern void messages_to_stdout (void);
extern Unsigned16 _message_flags;
extern Unsigned16 _msg_invoke_flag;
extern FILE *_msg_log_file;
extern void _message_kernel (struct File *, const char *, ...);
extern void _message_kernel_nofile (const char *, ...);
extern struct File *ewfile (const char *fn, long ln);
#endif /*MESSAGES_H_*/
