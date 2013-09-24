#ifndef ORACCNET_H_
#define ORACCNET_H_

#include <xmlrpc-c/base.h>

struct call_info
{
  const char *clientIP;
  char *serverURL;
  char *session;
  char *method;
  char **methodargs;
  int nargs;
  char *user;
  char *password;
  char *project;
  char *version;
  void *request_data;
  void *response_data;
  struct file_data *files;
  struct file_data *files_last;
  int status;
  char *statusstr;
  int wait_seconds;
};

struct client_method_info;
typedef xmlrpc_value* (*client_call)(xmlrpc_env *const envP, struct client_method_info *cmi);
typedef void (*client_action)(xmlrpc_env *const envP, struct client_method_info *cip, xmlrpc_value *resultP);

struct client_method_info
{
  const char *name;
  client_call call;
  client_action action;
  int *wait_seconds;
  struct call_info *instance;
};

struct file_data
{
  const unsigned char *data;
  const unsigned char *name;
  unsigned char *      path; /* this is computed on the server and is only relevant there */
  int 		       size;
  const unsigned char *what;
  struct file_data *   next;
};

struct meths_tab
{
  const char *name;
  struct client_method_info *info;
};

extern char session_template[];
extern const char *varoracc;

extern void callinfo_append_arg(struct call_info *cip, const char *arg, const char *sep, const char *val);
extern struct call_info *callinfo_clone(struct call_info *cip);
extern struct call_info *callinfo_new(void);
extern xmlrpc_value *callinfo_pack(xmlrpc_env *envP, struct call_info *cip);
extern struct call_info *callinfo_unpack(xmlrpc_env *envP, xmlrpc_value *s);

extern xmlrpc_value *file_b64(xmlrpc_env * const envP, const char *path, const char *name, const char *what);
extern void file_dump(xmlrpc_env * const envP, xmlrpc_value *const log, const char *fname);
extern struct file_data *file_find(struct call_info *cip, const char *what);
extern xmlrpc_value *file_pack(xmlrpc_env * const envP, const char *file_what, const char *file_name);
extern void file_save(struct call_info *cip, const char *dir);
extern struct file_data *file_unpack(xmlrpc_env * const envP, xmlrpc_value * const fstruct);

extern xmlrpc_value *generic_request(xmlrpc_env *const envP, struct client_method_info *cmi);

extern xmlrpc_value *request_common(xmlrpc_env *const envP, const char *type, const char *fmt, va_list ap);
extern xmlrpc_value *request_error(xmlrpc_env *const envP, const char *fmt, ...);
extern xmlrpc_value *request_exec(xmlrpc_env * const envP, const char *path, const char *name, struct call_info *cip);
extern xmlrpc_value *request_status(xmlrpc_env *const envP, const char *fmt, ...);

extern char *sesh_file(const char *basename);
extern void sesh_init(xmlrpc_env * const envP, xmlrpc_value * const s, int with_tmpdir);
extern void sesh_set_template(const char *template);

extern void dieIfFaultOccurred (xmlrpc_env * const envP);
extern void dieIfFaultOccurred3 (xmlrpc_env * const, const char *, int);
extern struct meths_tab *meths(register const char *str, register unsigned int len);

extern int trace;
#define trace() if(trace){fprintf(stderr, "%s:%d\n", __FILE__, __LINE__);}
#define dieIfFaultOccurred(e) dieIfFaultOccurred3(e, __FILE__, __LINE__)

#endif/*ORACCNET_H_*/
