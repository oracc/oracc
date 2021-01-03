#!/bin/sh
# exec >01log/o2-atf.log ; exec 2>&1
l2p1-pre-atf.sh
if [ -d 00atf ]; then
    batch=`oraccopt . atf-batch`
    if [ "$batch" = "yes" ]; then
	### note this does not yet work with scores; needs o2-atfsources to work with .ods
	o2-atfsources.plx
	atfbatch.plx
	atfinstall.plx
	o2-atf.plx -batch
    else
	# rebuild files that have changed
	o2-atfsources.plx
	o2-atf.plx
	LIST=`(cd 00atf ; find . -type f -name '*.atf' -print0 | xargs -0 grep -l '^@score' | head -1)`
	if [ "$LIST" != "" ]; then
	    o2-unscore.sh
	fi
	atfsplit.plx -update -install -base 01bld 00atf/*.atf
    fi
fi
