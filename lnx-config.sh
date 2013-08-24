#!/bin/sh
libtoolize
aclocal
autoheader
automake
autoconf
./configure --prefix=$ORACC
make $1
./create-oracc-vhost-conf.sh
