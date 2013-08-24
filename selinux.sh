#!/bin/sh
SEM=`which semanage`
if [ "$SEM" = "" ]; then
    echo 'selinux.sh: no semanage installed (do you need to yum install policycoreutils-python?)'
    exit 1
fi
/usr/sbin/semanage fcontext -a -t httpd_user_script_exec_t "$ORACC/bin(/.*)?"
/usr/sbin/semanage fcontext -a -t httpd_sys_content_t "$ORACC/bld(/.*)?"
/usr/sbin/semanage fcontext -a -t httpd_sys_content_t "$ORACC/lib(/.*)?"
/usr/sbin/semanage fcontext -a -t httpd_sys_content_t "$ORACC/pub(/.*)?"
/usr/sbin/semanage fcontext -a -t httpd_sys_content_t "$ORACC/xml(/.*)?"
/usr/sbin/semanage fcontext -a -t httpd_user_content_t "$ORACC/www(/.*)?"
/usr/sbin/semanage fcontext -a -t httpd_sys_script_exec_t "$ORACC/www/cgi-bin(/.*)?"
