#!/bin/sh
function fail {
    echo $*
    exit 1
}
uname=`uname`
if [ "$uname" = "Linux" ]; then
    sudo useradd oracc
    passwd oracc
else
    if [ ! -d /Users/oracc ]; then
	fail 'oracc-user.sh: please create the oracc user on this machine before proceeding'
    fi
fi
cp oraccenv.example oraccenv.sh
cat <<EOF
#########################################################################
#                                                                       #
#  You now need edit oraccenv.sh as explained in ORACC-INSTALL          #
#                                                                       #
#########################################################################
EOF
