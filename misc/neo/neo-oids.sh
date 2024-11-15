#!/bin/sh
h=`hostname`
sigoid.plx 01bld/project.sig >01bld/sigoid.tab
for l in `cut -f1 01bld/sigoid.tab | sort -u` ; do
    if [ "$l" != "sux" ]; then
	echo neo-oids.sh processing $l
	mkdir -p 01bld/$l
	grep ^$l 01bld/sigoid.tab >01bld/$l/need-oid.tab
	if [ "$h" == "build-oracc" ]; then
	    echo calling oid.plx -a -x -p neo -dom $l on build-oracc
	    cat 01bld/$l/need-oid.tab | oid.plx -a -x -p neo -dom $l >/dev/null
	fi
    else
	echo neo-oids.sh not processing sux
    fi
done
