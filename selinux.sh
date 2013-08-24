#!/bin/sh
function quit {
    echo selinux.sh: $1. Stop.
    exit 1;
}
function needfile {
    -r $1 || quit no such file $1
}
. ./oraccenv.sh
FC1=/etc/selinux/targeted/modules/active/file_contexts
FC2=/etc/selinux/targeted/contexts/files/file_contexts
needfile $FC1
needfile $FC2
-w $FC1 || quit $FC1 is not writable
-w $FC2 || quit $FC2 is not writable
grep oracc $FC1 && quit Oracc already installed in $FC1
grep oracc $FC2 && quit Oracc already installed in $FC2
mkdir -p $ORACC/{bld,xml,pub}
cat >>$FC1 <<EOF
/usr/sbin/semanage fcontext -a -t httpd_sys_content_t "$ORACC/bin(/.*)?"
/usr/sbin/semanage fcontext -a -t httpd_sys_content_t "$ORACC/bld(/.*)?"
/usr/sbin/semanage fcontext -a -t httpd_sys_content_t "$ORACC/lib(/.*)?"
/usr/sbin/semanage fcontext -a -t httpd_sys_content_t "$ORACC/pub(/.*)?"
/usr/sbin/semanage fcontext -a -t httpd_sys_content_t "$ORACC/xml(/.*)?"
/usr/sbin/semanage fcontext -a -t httpd_sys_content_t "$ORACC/www(/.*)?"
/usr/sbin/semanage fcontext -a -t httpd_sys_script_exec_t "$ORACC/www/cgi-bin(/.*)?"
/usr/sbin/semanage fcontext -a -t httpd_sys_script_exec_t "$ORACC/bin(/.*)?"
EOF
cat >>$FC2 <<EOF
/usr/sbin/semanage fcontext -a -t httpd_sys_content_t "$ORACC/bin(/.*)?"
/usr/sbin/semanage fcontext -a -t httpd_sys_content_t "$ORACC/bld(/.*)?"
/usr/sbin/semanage fcontext -a -t httpd_sys_content_t "$ORACC/lib(/.*)?"
/usr/sbin/semanage fcontext -a -t httpd_sys_content_t "$ORACC/pub(/.*)?"
/usr/sbin/semanage fcontext -a -t httpd_sys_content_t "$ORACC/xml(/.*)?"
/usr/sbin/semanage fcontext -a -t httpd_sys_content_t "$ORACC/www(/.*)?"
/usr/sbin/semanage fcontext -a -t httpd_sys_script_exec_t "$ORACC/www/cgi-bin(/.*)?"
/usr/sbin/semanage fcontext -a -t httpd_sys_script_exec_t "$ORACC/bin(/.*)?"
EOF
/sbin/restorecon -R -v $ORACC
