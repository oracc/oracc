#/bin/sh

projtype=`oraccopt . type`
rm -fr 01log/glo.err
touch 01log/glo.err
if [ -d "00src" ]; then
    srcglo=`ls -1 00src/*.glo | grep -v '~' | grep glo`
else
    srcglo=""
fi
for a in $srcglo ; do
    if [ "$projtype" == "superglo" ]; then
	log=01log/`basename $a .glo`.log
	exec 3>&1 4>&2 1>$log 2>&1
	cbdpp.plx -nosigs $a
	exec 1>&3 2>&4
	if [ -s $log ]; then
	    echo $log >01log/glo.err
	fi
	# there can only be one .glo in a superglo so print its name and exit
	/bin/echo -n $a | sed s/00src/01tmp/
	exit 0
    else
	cbdpp.plx $a
    fi
done
libglo=`ls -1 00lib/*.glo | grep -v '~' | grep glo`
if [ "$projtype" == "superglo" ]; then
    true
else
    for a in $libglo ; do
	log=01log/`basename $a .glo`.log
	exec 3>&1 4>&2 1>$log 2>&1
	cbdpp.plx -sigs -nopsus $a
	exec 1>&3 2>&4
	if [ -s $log ]; then
	    echo $log >>01log/glo.err
	fi
    done
fi
for a in $libglo ; do
    globase=`basename $a`
    if [ ! -r "00src/$globase" ]; then
	if [ "$projtype" == "superglo" ]; then
	    cbdpp.plx -nosigs -announce $a
	    # there can only be one .glo in a superglo so print its name and exit
	    /bin/echo -n $a | sed s/00lib/01tmp/
	    exit 0
	else
	    log=01log/`basename $globase .glo`.log
	    if [ ! -s $log ]; then
		exec 3>&1 4>&2 1>$log 2>&1
		cbdpp.plx $a
		exec 1>&3 2>&4
		if [ -s $log ]; then
		    echo $log >>01log/glo.err
		fi
	    else
		echo "$0: 01log/$logfile is non-empty"
		exit 1
	    fi
	fi
    fi
done
exit 0
