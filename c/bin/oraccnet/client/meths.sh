#!/bin/sh
cat <<EOF >meths.g
%{
#include <xmlrpc-c/base.h>
#include "oraccnet.h"
EOF
for a in ../lib/m_*_client.c; do
    m=`basename $a .c | cut -d_ -f2`
    echo "extern struct client_method_info ${m}_client_info;" >>meths.g
done
cat <<EOF >>meths.g
%}
struct meths_tab;
%%
EOF
for a in ../lib/m_*_client.c; do
    m=`basename $a .c | cut -d_ -f2`
    echo "$m, &${m}_client_info" >>meths.g
done
