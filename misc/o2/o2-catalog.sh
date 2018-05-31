#!/bin/sh
function progress {
    echo $*
    $*
}

progress o2-cnf.sh
progress o2-lst.sh
progress o2-cat.sh
progress o2-xml.sh
progress o2-web.sh
progress o2-prm.sh
progress o2-finish.sh
