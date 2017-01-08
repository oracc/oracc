#!/bin/sh
o2-cat.sh
echo '}' >>01bld/metadata.json
o2-xml.sh
o2-web.sh
o2-prm.sh
