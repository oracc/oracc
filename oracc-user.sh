#!/bin/bash
function fail {
    echo $*
    exit 1
}
uname=`uname`
if [ "$uname" = "Linux" ]; then
    sudo useradd -m -s /sbin/nologin oracc
    sudo passwd oracc
    sudo chmod -R g+rx /home/oracc
    sudo chmod -R go-w /home/oracc
    sudo chmod -R o-r /home/oracc
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
