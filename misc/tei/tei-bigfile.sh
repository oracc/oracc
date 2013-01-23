#!/bin/sh
if [ ! -e 00lib/config.xml ]; then
    echo tei-project.sh: must run from a project or subproject directory
    exit 1
fi
if [ "$1" = "" ];
then
    echo tei-project.sh: must give project name as argument
    exit 1
fi
project=$1
projxml=02pub/tei-project.xml
echo '<teiCorpus ' >$projxml
echo '  xmlns="http://www.tei-c.org/ns/1.0" xmlns:tei="http://www.tei-c.org/ns/1.0" xml:base="../02xml/00tei/"' \
    >>$projxml
echo '  xmlns:xi="http://www.w3.org/2001/XInclude">' >>$projxml
(cd 02xml/00tei ; \
    for a in *.xml ; do echo '<xi:include href="'$a'"/>' \
    >>${ORACC_HOME}/$project/$projxml ; done )
echo '</teiCorpus>' >>$projxml
#mv -f 01tmp/tei-project.xml 02pub/tei-project.xml
