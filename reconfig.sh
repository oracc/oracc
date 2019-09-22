#!/bin/sh
. ./oraccenv.sh
libtoolize
aclocal
autoheader
automake
autoconf
./configure --prefix=$ORACC
#make CFLAGS=-g
#make install
