%{
#include <string.h>
#include <xmlrpc-c/base.h>
#include "oraccnet.h"
extern struct client_method_info debug_client_info;
extern struct client_method_info environment_client_info;
extern struct client_method_info ox_client_info;
extern struct client_method_info status_client_info;
%}
struct meths_tab;
%%
debug, &debug_client_info
environment, &environment_client_info
ox, &ox_client_info
status, &status_client_info
