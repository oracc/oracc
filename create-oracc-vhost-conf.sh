#!/bin/sh

function checkenv {
    if [ "$2" == "" ]; then
	echo create-oracc-vhost-conf.sh: environment variable $1 is not set. Stop.
	exit 1
    fi
}

checkenv ORACC $ORACC
checkenv ORACC_HOST $ORACC_HOST

if [ -d /var/log/httpd ]; then
    LOGDIR=/var/log/httpd
else
    if [ -d /private/var/log/apache2 ]; then
	LOGDIR=/private/var/log/apache2
    else
	echo create-oracc-vhost-conf.sh: nowhere to write log files. Stop.
	exit 1
    fi
fi

# NameVirtualHost *:80
# include oracc-vhost.conf

cat >oracc-vhost.conf <<EOF
<VirtualHost *:80>
    ServerAdmin root@$ORACC_HOST
    ServerName oracc
    DocumentRoot "$ORACC/www"
    ScriptAliasMatch ^/cgi-bin/(.*$) "$ORACC/www/cgi-bin/$1"
    ErrorLog "$LOGDIR/oracc-error_log"
    CustomLog "$LOGDIR/oracc-access_log" common
    <Directory "/usr/local/oracc/www">
               Options Indexes FollowSymLinks MultiViews
               AllowOverride None
               Order allow,deny
               Allow from all
    </Directory>
    <Directory "$ORACC/www/cgi-bin">
               AllowOverride None
               Options ExecCGI
               Order allow,deny
               Allow from all
    </Directory>
    RewriteEngine on
    RewriteLog "$LOGDIR/oracc-rewrite.log"
    RewriteLogLevel 2
    RewriteCond $ORACC/www/%{REQUEST_FILENAME} -f
    RewriteRule ^(.+)$ - [L]
    RewriteCond $ORACC/www/%{REQUEST_FILENAME}/index.html -f
    RewriteRule ^(.+)/?$ $1/index.html [L]
    RewriteRule \.(?:css|js|html|xml)$ - [L]
    RewriteRule ^(.*)$ /cgi-bin/oracc-resolver$1 [T=application/x-httpd-cgi,L]
</VirtualHost>
EOF
