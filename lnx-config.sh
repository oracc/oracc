#!/bin/sh
libtoolize
aclocal
autoheader
automake
autoconf
./configure --prefix=/usr/local/oracc
make
make install
