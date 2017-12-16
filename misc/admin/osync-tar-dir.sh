#!/bin/sh

function fix_links {
    if [[ $fixlinks = "yes" ]]; then
	cd $ORACC_BUILDS
	fixlinks=fixlinks-$$.sh
	echo '#!/bin/sh' >$fixlinks
	find $project/* -maxdepth 1 -type l -ls | osync-fixlinks.plx >>$fixlinks
	for a in bld xml ; do
	    if [[ $a = "bld" ]]; then
		find $a/$project/* -maxdepth 1 -type l -ls | osync-fixlinks.plx >>$fixlinks
	    elif [[ $a = "xml" ]]; then
		find $a/$project/oml -type l -ls | osync-fixlinks.plx >>$fixlinks
	    fi
	done
	chmod +x $fixlinks
	sudo ./fixlinks-$$.sh
    fi
}

function remove_old_project {
    if [ -d $project ]; then
	echo "osync-tar.sh: removing old version of $project"
	sudo rm -fr $project
	for a in bld pub tmp www xml ; do
	    d=$ORACC_BUILDS/$a/$project
	    s=$there:$oracc/$a/$project
	    (cd $ORACC_BUILDS/$a ;
	     if [ -d $d ]; then
		 echo "osync-tar.sh: removing old version of $d"
		 sudo rm -fr $d
	     fi
	    )
	done
    fi
}

there=$1
if [[ $there == "" ]]; then
    echo "osync-tar-dir.sh: must give remote machine name on command line"
    exit 1
fi

cd $ORACC_BUILDS

if [[ $ORACC_BUILDS == "/home/oracc" ]]; then
    fixlinks=no
else
    fixlinks=yes
fi

for tarball in tar/*.tar.gz ; do
    project=`basename $tarball .tar.gz`
    remove_old_project $project
    echo "osync-tar-dir.sh: unpacking $tarball"
    tar zxpf $tarball
    touch $project/.sync
    fix_links
done

for a in tar/*.tar.gz ; do
    project=`basename $a .tar.gz`
    for a in `list-subprojects.sh $project` ; do
	sudo $ORACC_BUILDS/bin/serve-index.sh $a $there
    done
    sudo $ORACC_BUILDS/bin/serve-index.sh $project $there
done
