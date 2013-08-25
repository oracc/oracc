#!/bin/sh

function apachectl_restart {
    echo apachectl start >>/etc/rc.d/rc.local
}

function quit {
    echo install-oracc-vhost-conf.sh: $*. Stop.
    exit 1
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
cd $HTTPD_CONFDIR || quit "unable to change directory to $HTTPD_CONFDIR"
SAVED_CONF=oracc-saved-`basename $HTTPD_CONF`
cp -a $HTTPD_CONF $SAVED_CONF  || quit "unable to save backup copy of $SAVED_CONF"
cp -a $INSTDIR/oracc-vhost.conf . || quit "unable to install oracc-vhost.conf"
chown root:root oracc-vhost.conf
chmod go-rw oracc-vhost.conf
SEL=`which chcon`
if [ ! "$SEL" = "" ]; then
    chcon -t httpd_config_t oracc-vhost.conf
fi
cat >>$HTTPD_CONF <<EOF
NameVirtualHost *:80
include $HTTPD_VDIR/oracc-vhost.conf
EOF
if [ -r /etc/rc.d/rc.local ]; then
    grep -q apachectl /etc/rc.d/rc.local || apachectl_start
fi
apachectl -t || quit "Please correct the problems with httpd.conf then type: apachectl restart"
echo You may now restart the webserver by typing: apachectl restart
