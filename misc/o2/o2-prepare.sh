#!/bin/sh
o2-cnf.sh
o2-lst.sh
o2-cat.sh
o2-update-lem.sh
o2-atf.sh
if [ -r 01bld/destfiles.lst ]; then
    o2-tr-lst.plx <01bld/destfiles.lst >01bld/lists/have-xtr.tab
fi
