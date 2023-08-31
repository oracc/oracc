#!/bin/sh

atf=$1
mkdir -p 01log
xval=0

if [ -d "00src" ]; then
    srcglo=`ls -1 00src/*.glo | grep -v '~' | grep glo`
else
    srcglo=""
fi

for a in $srcglo ; do
    cbdpp.plx -c $a
    if [ $? != 0 ]; then xval=1; fi
done

libglo=`ls -1 00lib/*.glo | grep -v '~' | grep glo`
for a in $libglo ; do
    globase=`basename $a`
    if [ ! -r "00src/$globase" ]; then
	cbdpp.plx -c $a
	if [ $? != 0 ]; then xval=1; fi
    fi
done

if [ $xval = 1 ]; then
    echo "oracc check: some glossaries failed test; not checking .atf"
    exit 1
fi

if [[ $atf = "atf" ]]; then
    if [ -d "00atf" ]; then
	batch=`oraccopt . atf-batch`
	oxx=`oraccopt . atf-oxx`
	if [ "$oxx" == "yes" ]; then
	    oxexec=oxx
	else
	    oxexec=ox
	fi
	if [ "$batch" = "yes" ]; then
	    for a in 01bld/atf-batch-*.lst ; do
		$oxexec -cm -I$a
	    done
	else
	    files=`find 00atf -type f | grep '.\(ods\|.atf\)$' | grep -v ods.atf`
	    $oxexec -cm $files
	fi
    fi
fi
