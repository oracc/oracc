#!/bin/sh
. ./oraccenv.sh
sudo ./preconfig.sh || exit 1
glibtoolize
aclocal
autoheader
automake
autoconf
./configure --prefix=/Users/stinney/orc
make
make install
