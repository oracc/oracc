#!/bin/sh
if [ "$ORACC" = "" ];
then
    echo doc/install.sh: must set ORACC environment variable first
    exit 1
fi
umask 0022
WWW=${ORACC}/www
mkdir -p $WWW/downloads/oracc ; chmod -R o+r $WWW/downloads
rm -fr ${WWW}/doc
mkdir -p ${WWW}/css ${WWW}/js ${WWW}/doc
(cd ${WWW}/doc ; ln -sf ${ORACC}/www wwwhome)
cp -f 00css/*.css ${WWW}/css
#cp -f 00js/*.js ${WWW}/js
cp -f index.html ${WWW}/doc
chmod -w ${WWW}/css/*
#chmod -w ${WWW}/js/*
chmod -w ${WWW}/doc/index.html
# Note that ns must come before builder because builder
# links to some html generated in ns
PATH=${ORACC}/bin:$PATH ; export PATH
for a in ns builder developer manager steerer sysadmin install user ; do \
    mkdir -p ${WWW}/doc/$a; chmod o+r ${WWW}/doc/$a ; \
    (cd $a ; [ -e install.sh ] && ./install.sh) ; \
done
chmod -R o+r ${WWW}/ns
(cd wwwhome ; ./install.sh)
(cd ood ; ./install.sh)
(cd $WWW/doc ; find . -name '*.html' | xargs htmlbase.plx -project doc)
00tools/estindex.sh ${WWW}/doc
00tools/docperms.sh ${WWW}
