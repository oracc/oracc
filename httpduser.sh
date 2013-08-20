#!/bin/sh
u=`grep -q _www /etc/passwd && /bin/echo -n _www`
if [ "$u" = "" ]; then
    u=`grep -q apache /etc/passwd && /bin/echo -n apache`
fi
/bin/echo -n $u
