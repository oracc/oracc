#ifndef ORACCNET_H_
#define ORACCNET_H_

#include <xmlrpc-c/base.h>

struct call_info
{
  char *clientIP;
  char *serverURL;
  char *session;
  char *method;
  char **methodargs;
  char *user;
  char *password;
  char *project;
  char *version;
  void *request_data;
  void *response_data;
  struct file_data *files;
  struct file_data *files_last;
};

#define METHOD_ARGS_MAX	16

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

extern struct call_info *callinfo_new(void);
extern xmlrpc_value *callinfo_pack(xmlrpc_env *envP, struct call_info *cip);
extern struct call_info *callinfo_unpack(xmlrpc_env *envP, xmlrpc_value *s);

extern xmlrpc_value *file_pack(xmlrpc_env * const envP, const char *file_what, const char *file_name);
extern void file_save(struct call_info *cip, const char *dir);
extern struct file_data *file_unpack(xmlrpc_env * const envP, xmlrpc_value * const fstruct);

extern void sesh_init(xmlrpc_env * const envP, xmlrpc_value * const s, int with_tmpdir);
extern void sesh_set_template(const char *template);
extern void dieIfFaultOccurred (xmlrpc_env * const envP);
extern struct meths_tab *meths(register const char *str, register unsigned int len);

#endif/*ORACCNET_H_*/
