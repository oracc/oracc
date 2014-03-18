#!/bin/sh
soapcpp2 -c ocalc.h
gcc -I/usr/local/include -c soapC.c
gcc -I/usr/local/include -c soapServerLib.c
gcc -I/usr/local/include -c soapServer.c
gcc -I/usr/local/include -c ocalc-server.c
arch=`uname`
if [ "$arch" = 'Darwin' ]; then
## Mac OS X
    gcc -L/usr/local/lib -lgsoap -o oracc-calc.cgi soapServer.o soapC.o ocalc-server.o
else
## Linux
    gcc -L/usr/local/lib -o oracc-calc.cgi soapServer.o soapC.o ocalc-server.o -lgsoap -lm
fi
