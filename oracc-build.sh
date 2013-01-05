#!/bin/sh
make clean
make CFLAGS="-g -fPIC" >make.log 2>&1
make DESTDIR=/usr/local/oracc/test install
