#!/bin/sh
. ./oraccenv.sh
glibtoolize
aclocal
autoheader
automake
autoconf
./configure --prefix=/Users/stinney/orc
make
make install
