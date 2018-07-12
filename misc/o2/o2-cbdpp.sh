#/bin/sh

projtype=`oraccopt . type`
g2=`oraccopt . g2`
if [ "$g2" == "yes" ]; then
    if [ -d "00src" ]; then
	srcglo=`ls -1 00src/*.glo | grep -v '~' | grep glo`
    else
	srcglo=""
    fi
    for a in $srcglo ; do
	if [ "$projtype" == "superglo" ]; then
	    cbdpp.plx -nosigs -announce $a
	    # there can only be one .glo in a superglo so print its name and exit
	    /bin/echo -n $a | sed s/00src/01tmp/
	    exit 0
	else
	    cbdpp.plx -announce $a
	fi
    done
    libglo=`ls -1 00lib/*.glo | grep -v '~' | grep glo`
    if [ "$projtype" == "superglo" ]; then
	true
    else
	for a in $libglo ; do
	    cbdpp.plx -sigs -nopsus $a
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
		cbdpp.plx -announce $a
	    fi
	fi
    done
fi
