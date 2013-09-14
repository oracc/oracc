#!/bin/sh
rm registry.c
for a in ../lib/m_*_server.c; do
    m=`basename $a .c | cut -d_ -f2`
    echo "extern struct xmlrpc_method_info3 ${m}_server_info;" >>registry.c
done
echo 'struct xmlrpc_method_info3 *methodlist[] = {' >>registry.c
for a in ../lib/m_*_server.c; do
    m=`basename $a .c | cut -d_ -f2`
    echo "  &${m}_server_info," >>registry.c
done
echo '  NULL' >>registry.c
echo '};' >>registry.c
