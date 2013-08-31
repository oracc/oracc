#!/bin/sh
function fail {
    echo $*
    exit 1
}
uname=`uname`
if [ "$uname" = "Linux" ]; then
    sudo useradd oracc
    passwd oracc
fi
cd ~/oracc || fail "unable to change to user oracc's home directory"
mkdir src
cd src
git clone  https://github.com/oracc/oracc.git .
cp oraccenv.example oraccenv.sh
cat <<EOF
#########################################################################
#                                                                       #
#  Now edit oraccenv.sh as explained in ORACC-INSTALL                   #
#                                                                       #
#########################################################################
EOF
