#!/bin/sh
o2-cnf.sh
lx-lists.sh
o2-lst.sh
# if [ config-opt=lx-lists ]; (cd 01bld ; mv lists o2lists ; mv lxlists lists)
o2-cat.sh
o2-update-lem.sh
o2-atf.sh
if [ -r 01bld/destfiles.lst ]; then
    cut -f1,3 02pub/atf-data.tab | sed 's/^.*://' | sed 's#@[/a-z0-9]\+##' >01bld/lists/have-xtr.tab
fi
#    o2-tr-lst.plx <01bld/destfiles.lst >01bld/lists/have-xtr.tab
