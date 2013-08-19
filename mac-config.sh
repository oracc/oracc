#!/bin/sh
. ./oraccenv.sh
./preconfig.sh || exit 1
glibtoolize
aclocal
autoheader
automake
autoconf
./configure --prefix=/usr/local/oracc
make
make install
