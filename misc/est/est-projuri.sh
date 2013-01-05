#!/bin/sh
xmlnl <02pub/project.xml | head -3 >02pub/projuri.xml
xmlnl <02pub/project.xml | grep xmd >>02pub/projuri.xml
xmlnl <02pub/project.xml | tail -2 >>02pub/projuri.xml
xsltproc -xinclude ${ORACC}/lib/scripts/est-projuri.xsl 02pub/projuri.xml >01bld/est/project.est
