#!/bin/sh
libtoolize
aclocal
automake
autoconf
./configure --prefix=/usr/local/oracc
make
make install
