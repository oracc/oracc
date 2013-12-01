extern struct xmlrpc_method_info3 build_server_info;
extern struct xmlrpc_method_info3 debug_server_info;
extern struct xmlrpc_method_info3 deploy_server_info;
extern struct xmlrpc_method_info3 environment_server_info;
extern struct xmlrpc_method_info3 ox_server_info;
extern struct xmlrpc_method_info3 status_server_info;
struct xmlrpc_method_info3 *methodlist[] = {
  &build_server_info,
  &debug_server_info,
  &deploy_server_info,
  &environment_server_info,
  &ox_server_info,
  &status_server_info,
  NULL
};
