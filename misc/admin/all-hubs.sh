#!/bin/sh
if [ "${ORACC_BUILDS}" == "" ]; then
    echo ORACC_BUILDS not set in environment. Stop.
    exit 1
fi
if [ $(whoami) = "root" ]; then
  cd ${ORACC_BUILDS}
  for a in `agg-list-public-projects.sh` ; do
      if [ -r $a/02xml/project-data.xml ]; then
	  xsltproc $ORACC/lib/scripts/p3-hub.xsl $a/02xml/project-data.xml >$a/02www/hub.html
	  chown root:oracc $a/02www/hub.html
	  chmod g+w $a/02www/hub.html
	  chmod o+r $a/02www/hub.html
      fi
  done
else
    echo "You can only run this script as root. Stop."
    exit 1
fi
