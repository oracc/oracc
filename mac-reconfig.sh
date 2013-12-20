#!/bin/sh
. ./oraccenv.sh
glibtoolize
aclocal
autoheader
automake
autoconf
./configure --prefix=/usr/local/oracc
make
make install
