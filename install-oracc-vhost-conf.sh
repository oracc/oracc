#!/bin/sh

force=$1

function apachectl_start {
    echo apachectl start >>/etc/rc.d/rc.local
}

function edit_httpd_conf {
cat >>$HTTPD_CONF <<EOF
NameVirtualHost *:80
include $HTTPD_VDIR/oracc-vhost.conf
EOF
}

function quit {
    if [ "$force" = "" ]; then
	echo install-oracc-vhost-conf.sh: $*. Stop.
	exit 1
    fi
}
if [ -r /etc/httpd/conf/httpd.conf ]; then
    HTTPD_CONF=/etc/httpd/conf/httpd.conf
else
    if [ -r /private/etc/apache2/httpd.conf ]; then
	HTTPD_CONF=/private/etc/apache2/httpd.conf
    else
	quit "unable to locate httpd.conf"
    fi
fi
grep -q oracc-vhost $HTTPD_CONF && quit "oracc-vhost is already in httpd.conf"
if [ -d /etc/httpd/conf ]; then
    HTTPD_CONFDIR=/etc/httpd/conf
    HTTPD_VDIR=/etc/httpd/conf
else
    if [ -d /private/etc/apache2/extra ]; then
	HTTPD_CONFDIR=/private/etc/apache2
	HTTPD_VDIR=/private/etc/apache2/extra
    else
	quit "unable to locate httpd configuration directory"
    fi
fi
INSTDIR=`pwd`
pushd $HTTPD_CONFDIR || quit "unable to change directory to $HTTPD_CONFDIR"
SAVED_CONF=oracc-saved-`basename $HTTPD_CONF`
cp -a $HTTPD_CONF $SAVED_CONF  || quit "unable to save backup copy of $SAVED_CONF"
cp -a $INSTDIR/oracc-vhost.conf . || quit "unable to install oracc-vhost.conf"
chown root:root oracc-vhost.conf
chmod go-rw oracc-vhost.conf
popd
SEL=`which chcon`
if [ ! "$SEL" = "" ]; then
    ./selinux.sh
fi
# This can happen when we have a forced install
grep -q oracc-vhost $HTTPD_CONF || edit_httpd_conf
if [ -r /etc/rc.d/rc.local ]; then
    grep -q apachectl /etc/rc.d/rc.local || apachectl_start
fi
apachectl -t || quit "Please correct the problems with httpd.conf then type: apachectl restart"
echo You may now restart the webserver by typing: apachectl restart
