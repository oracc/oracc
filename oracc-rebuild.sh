#!/bin/sh
make clean
autoreconf -fvi
make CFLAGS=-g >make.log 2>&1
make DESTDIR=@@ORACC@@/test install
