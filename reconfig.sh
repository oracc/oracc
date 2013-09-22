#!/bin/sh
./configure  --prefix=$ORACC ; make CFLAGS=-g ; make install
