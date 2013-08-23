#!/bin/sh
. ./oraccenv.sh
make install
cd $ORACC
mkdir -p $ORACC/00any
cd $ORACC/00any
curl -L -o coredata.zip https://github.com/oracc/coredata/archive/master.zip
unzip coredata
mkdir -p $ORACC/lib/data
cp -a coredata-master/cats/catpnums.vec $ORACC/lib/data
mkdir -p $ORACC/xml/ogsl
cp -a coredata-master/sign/ogsl-sl.xml $ORACC/xml/ogsl
