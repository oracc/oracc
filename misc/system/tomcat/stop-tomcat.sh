#!/bin/sh
JAVA_HOME=/usr/java/jre1.6.0_18
CATALINA_HOME=/usr/local/oracc/lib/java/tomcat
export CATALINA_HOME
export JAVA_HOME
(cd $CATALINA_HOME ; ./bin/shutdown.sh)
