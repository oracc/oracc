# 
# XML Site Architecture Project
# George MacKerron 2005
#
# Adapted for Oracc by Steve Tinney, 2010
#

if [ "$1" = "" ]; then
    echo esp-live.sh: must give project name as first argument
    exit 1
fi

espdev=01bld/espdev

if [ ! -d $espdev ]; then
    echo esp-live.sh: nothing to copy--expected to find website in $espdev
    exit 1
fi

if [ "$2" != "" ]; then
    if [ "$2" = "force" ]; then
	force=yes
    else
	echo esp-live.sh: third argument may only be 'force'
	exit 1
    fi
fi

project=$1
webdir=02www
mkdir -p $webdir
if [ ! -d $webdir ]; then
    echo esp2-live.sh: $webdir does not exist and cannot be created
    exit 1
fi

if [ "$force" != "yes" ]; then
    echo "Make development directory live -- are you sure?"
    echo "(type 'yes' or 'no' then press [Enter])"
    read CONFIRM
    if ! test $CONFIRM = yes
    then exit 1
    fi
fi

log=02www/last_esp2_cp.log
rm -f 02www/_test

# note: addition of / to $espdev here is deliberate
cp -Rv $espdev/* 02www | cut -d'>' -f2 | grep / | sed 's/ //' >$log
chmod -R o+r $webdir/
chmod o-r $log
echo esp2 portal is now live at http://$ORACC_HOST/$project

exit 0
