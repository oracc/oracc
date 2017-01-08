#!/bin/sh
. ./oraccenv.sh
sudo ./preconfig.sh || exit 1
glibtoolize
aclocal
autoheader
automake
autoconf
./configure --prefix=~/orc
make
make install
