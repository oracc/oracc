#ifndef ORACCNET_H_
#define ORACCNET_H_

struct call_info
{
  char *clientIP;
  char *serverURL;
  char *project;
  char *version;
  char *user;
  char *password;
  char *session;
  void *method_data;
};

/* This template is for the static parts of a method request; the
   method-specific parts of the template get scanned separately */
#define CALL_TEMPLATE	sssssss

typedef xmlrpc_value* (*client_call)(xmlrpc_env *const envP, const char *serverURL);
typedef void (*client_action)(xmlrpc_env *const envP, xmlrpc_value *resultP);

struct client_method_info
{
  const char *name;
  client_call call;
  client_action action;
  int *wait_seconds;
  struct call_info *instance;
};

struct meths_tab
{
  const char *name;
  struct client_method_info *info;
};

extern int debug_wait;
extern xmlrpc_value *debug_call(xmlrpc_env *const envP, const char *serverURL);
extern void debug_action(xmlrpc_env *const envP, xmlrpc_value *resultP);

extern void dieIfFaultOccurred (xmlrpc_env * const envP);
extern struct meths_tab *meths(register const char *str, register unsigned int len);

#endif/*ORACCNET_H_*/
