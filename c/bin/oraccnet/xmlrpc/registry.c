extern struct xmlrpc_method_info3 debug_server_info;
extern struct xmlrpc_method_info3 environment_server_info;
struct xmlrpc_method_info3 *methodlist[] = {
  &debug_server_info,
  &environment_server_info,
  NULL
};
