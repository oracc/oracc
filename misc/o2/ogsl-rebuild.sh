#!/bin/sh
sl-oid.sh
sl-xml.plx ogsl
sl-group.plx
xsltproc ${ORACC}/lib/scripts/sl-letters-file.xsl 02xml/sl-grouped.xml >02pub/letters.tab
sl-signlist.sh
sl-db.plx ogsl
sl-index -boot
xsltproc -stringparam project ogsl ${ORACC}/lib/scripts/sl-HTML.xsl 02xml/ogsl-sl.xml >02www/ogsl.html
o2-portal.sh
cp -p 00lib/alias-*.txt 02pub
(cd 02xml ; rm -f ogsl.xml ; ln -sf ogsl-sl.xml ogsl.xml)
pubfiles.sh ogsl
# host=`hostname`
# if [[ $host = "build-oracc" ]]; then
#    echo Rebuilding ogsl on Munich build server ...
#    sudo -u ogsl ssh -p 22000 ogsl@oracc.ub.uni-muenchen.de git pull
#    sudo -u ogsl ssh -p 22000 ogsl@oracc.ub.uni-muenchen.de \
#	/home/oracc/bin/orc oracc build
# fi
