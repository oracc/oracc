#!/bin/sh
./mk-server.sh
gcc -g -DDEBUG -I/usr/local/include -c soapC.c
gcc -g -DDEBUG -I/usr/local/include -c soapClient.c
gcc -g -DDEBUG -I/usr/local/include -c ocalc-client.c
arch=`uname`
if [ "$arch" = 'Darwin' ]; then
## Mac OS X
    gcc -L/usr/local/lib -o ocalc-client -lgsoap ocalc-client.o soapC.o soapClient.o
else
## Linux
    gcc -L/usr/local/lib -o ocalc-client ocalc-client.o soapC.o soapClient.o -lgsoap -lm
fi

