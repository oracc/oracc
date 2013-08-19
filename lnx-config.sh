#!/bin/sh
. ./oraccenv.sh $1 || exit 1
./preconfig.sh || exit 1
libtoolize
aclocal
autoheader
automake
autoconf
./configure --prefix=$ORACC
make
make install
