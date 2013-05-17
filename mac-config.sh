#!/bin/sh
glibtoolize
aclocal
automake
autoconf
./configure --prefix=/usr/local/oracc
make
make install
