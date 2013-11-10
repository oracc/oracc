#!/bin/sh
function quit {
    echo selinux.sh: $*. Stop.
    exit 1;
}
function needfile {
    [ -r $1 ] || quit no such file $1
}
. ./oraccenv.sh
FC1=/etc/selinux/targeted/modules/active/file_contexts
FC2=/etc/selinux/targeted/contexts/files/file_contexts
FC1local=/etc/selinux/targeted/modules/active/file_contexts.local
FC2local=/etc/selinux/targeted/contexts/files/file_contexts.local
# .local can be empty so first check for file_context
needfile $FC1
needfile $FC2
grep oracc $FC1 && quit Oracc already installed in $FC1
# now switcheroo to .local versions
[ -w $FC1local ] || quit $FC1 is not writable
[ -w $FC2local ] || quit $FC2 is not writable
mkdir -p $ORACC/{bld,xml,pub}
cat oracc.fc >>$FC1
cat oracc.fc >>$FC1local
cat oracc.fc >>$FC2
cat oracc.fc >>$FC2local
/sbin/restorecon -R -v $ORACC

# httpd_enable_homedirs=1 allows httpd to access /home/oracc.
# httpd_execmem=1 allows httpd to run java
# httpd_can_network_connect=1 allows xslt scripts to use document('http://build.oracc.org/datestamp')
setsebool -P httpd_enable_homedirs=1 httpd_execmem=1 httpd_can_network_connect=1
