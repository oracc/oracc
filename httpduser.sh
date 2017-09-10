#!/bin/sh
u=`grep -q _www /etc/passwd && /bin/echo -n _www`
if [ "$u" = "" ]; then
    u=`grep -q www-data /etc/passwd && /bin/echo -n www-data`
    if [ "$u" = "" ]; then
	u=`grep -q apache /etc/passwd && /bin/echo -n apache`
    fi
fi
/bin/echo -n $u
