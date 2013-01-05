/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: options.h,v 0.4 1997/09/08 14:49:58 sjt Exp s $
*/

#ifndef OPTIONS_H_
#define OPTIONS_H_ 1

#include "pgetopt.h"

extern int _internal_argc;
extern char *const*_internal_argv;
extern Unsigned16 _msg_invoke_flag;
extern const char *V_opt;

extern void banner (void);
extern void help (void);
extern void default_options (int argc, char*const*argv, const char *user_opts);
extern void user_options (int argc, char*const*argv, const char *user_opts);
extern void options (int argc, char *const *argv, const char *legal_opts);
extern int opts (int optchar, char *optarg);
extern void usage (void);

#if defined(__EMX__)
#undef __GNU_LIBRARY__
#define __GNU_LIBRARY__ 1
#include <unistd.h>
#else
extern int optind, opterr;
extern char *optarg;
extern int getopt (int argc, char *const *argv, const char *legal_opts);
#endif

extern const char *usage_string;
extern Boolean doing_debug;
extern const char *copyright_string;

#endif
