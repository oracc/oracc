#!/bin/sh
${ORACC}/bin/agg-test.sh || exit 1
mkdir -p ${ORACC}/agg/projects
cd ${ORACC}/agg/projects
xsl=${ORACC}/lib/scripts
echo '<projects>' >all-projects.xml
if [ -d "${ORACC}/etc/projects" ]; then
    for a in `find ${ORACC}/etc/projects -follow -name 'config.xml'`; do
	echo "<xi:include xmlns:xi=\"http://www.w3.org/2001/XInclude\" href=\"$a\"/>" \
	    >>all-projects.xml
    done
fi
for a in `find ${ORACC}/xml -name 'config.xml'|grep -v ood`; do
    echo "<xi:include xmlns:xi=\"http://www.w3.org/2001/XInclude\" href=\"$a\"/>" \
	>>all-projects.xml
done
echo '</projects>' >>all-projects.xml

xsltproc --xinclude $xsl/agg-public-projects.xsl \
    all-projects.xml >public-projects.xml

agg-project-dates.plx public-projects.xml
xsltproc ${ORACC}/lib/scripts/oai-records.xsl public-projects-ds.xml >public-projects.oai

agg-projects-json.plx
xsltproc $xsl/xpd-JSON.xsl public-projects.xml >${ORACC}/www/json/projectlist.json

${ORACC}/bin/agg-thumbs.sh

mkdir -p ${ORACC}/www/agg
#cp -a ${ORACC}/agg/projects/images/* ${ORACC}/www/agg/

chmod +w ${ORACC}/www/projectlist.html
xsltproc -xinclude $xsl/agg-html-projects.xsl public-projects.xml \
    >${ORACC}/www/projectlist.html
#xsltproc -xinclude $xsl/agg-index.xsl public-projects.xml \
#    >${ORACC}/www/projectlist.html
chmod -w ${ORACC}/www/projectlist.html
chmod -R o+r ${ORACC}/www/agg/ ${ORACC}/www/projectlist.html ${ORACC}/www/json/projectlist.json \
    ${ORACC}/www/json/projects.json

