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
#(cd coredata-master ; cp -a doc2 $ORACC/www)
#curl -L -o ${ORACC}/bin/dropbox.py https://www.dropbox.com/download?dl=packages/dropbox.py
#chmod +x ${ORACC}/bin/dropbox.py
chown -R oracc:oracc /home/oracc
chmod -R o+r $ORACC/lib/data $ORACC/xml/ogsl $ORACC/www/doc2
echo -n '<usermode>'$ORACC_MODE'</usermode>' >$ORACC/lib/data/usermode.xml
