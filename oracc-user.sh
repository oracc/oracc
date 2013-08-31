#!/bin/sh
function fail {
    echo $*
    exit 1
}
uname=`uname`
if [ "$uname" = "Linux" ]; then
    sudo useradd oracc
    sudo passwd oracc
    chmod -R go+rx /home/oracc
    chmod -R go-w /home/oracc
else
    if [ ! -d /Users/oracc ]; then
	fail 'oracc-user.sh: please create the oracc user on this machine before proceeding'
    fi
    chmod o+r /Users/oracc
fi
cp oraccenv.example oraccenv.sh
cat <<EOF
#########################################################################
#                                                                       #
#  You now need edit oraccenv.sh as explained in ORACC-INSTALL          #
#                                                                       #
#########################################################################
EOF
