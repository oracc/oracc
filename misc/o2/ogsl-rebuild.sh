#!/bin/sh
if [ -r .nobuild ]; then
    echo 'You are not allowed to build ogsl here; it can only be built on build-oracc. Stop.'
    exit 1
fi
sl-oid.sh
sl-xml.plx ogsl
sl-group.plx
xsltproc ${ORACC}/lib/scripts/sl-letters-file.xsl 02xml/sl-grouped.xml >02pub/letters.tab
sl-signlist.sh
#sl-db.plx ogsl
sl-db2.plx ogsl
(cd 02pub/sl ; ln -sf ogsl-db2.tsv sl.tsv)
echo '#letters' >>02pub/sl/sl.tsv
sed 's/	/;let	/' <02pub/letters.tab >>02pub/sl/sl.tsv
#sl-index -boot
slix -boot
xsltproc -stringparam project ogsl ${ORACC}/lib/scripts/sl-HTML.xsl 02xml/ogsl-sl.xml >02www/ogsl.html
o2-portal.sh
cp -p 00lib/alias-*.txt 02pub
(cd ${ORACC_BUILDS}/xml/ogsl ; rm -f ogsl.xml ; ln -sf ogsl-sl.xml ogsl.xml)
pubfiles.sh ogsl
serve-project.sh ogsl
# host=`hostname`
# if [[ $host = "build-oracc" ]]; then
#    echo Rebuilding ogsl on Munich build server ...
#    sudo -u ogsl ssh -p 22000 ogsl@oracc.ub.uni-muenchen.de git pull
#    sudo -u ogsl ssh -p 22000 ogsl@oracc.ub.uni-muenchen.de \
#	/home/oracc/bin/orc oracc build
# fi
