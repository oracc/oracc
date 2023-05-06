#!/bin/sh
. ./oraccenv.sh
libtoolize
aclocal
autoheader
automake --add-missing
autoconf
./configure --prefix=$ORACC
make $1
#./create-oracc-vhost-conf.sh
