#!/bin/sh
if [ "$1" == "" ]; then
    echo change-project.sh: must give project as single argument
    exit 1
fi
perl -pi -e "s,^#project.*\s*\$,#project: $1\n," *.atf
