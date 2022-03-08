#!/bin/sh
function progress {
    echo $*
    $*
}

progress o2-cnf.sh
progress o2-lst.sh
progress o2-cat.sh
if [ -r 00etc/linkbase.xml ]; then
    cp -a 00etc/linkbase.xml 01bld
fi
if [ -r 01bld/linkbase.xml ]; then
    scoregen.plx -link
fi
progress o2-xml.sh
progress o2-web.sh
progress o2-prm.sh
progress o2-finish.sh
