#!/bin/sh

function check_env_or_die {
    if [ "$2" = "" ]; then
	echo oracc-subst.sh: environment variable $1 is not set--add it to oraccenv.sh. Stop.
	exit 1
    fi
}

#ORACC=$1

if [ "$2" == "" ]; then
    ORACC=$ORACC_BUILDS
    in=$1
else
    in=$2
fi

xetex=`locate xetex | grep xetex$ | grep /bin/`
if [ "$xetex" == "" ]; then
    echo "$0: xetex not yet installed"
    TeXLive=missing-texlive
else
    TeXLive=`dirname $xetex`
fi

check_env_or_die ORACC_BUILDS $ORACC_BUILDS
check_env_or_die ORACC_HOME $ORACC_HOME
check_env_or_die ORACC_HOST $ORACC_HOST
check_env_or_die ORACC_MODE $ORACC_MODE
check_env_or_die ORACC_USER $ORACC_USER

if [ -d /Users ]; then
    ORADMIN=/Users/oradmin
elif [ -d /home ]; then
    ORADMIN=/home/oradmin
else
    echo "$0: unable to set ORADMIN--no /home or /Users"
fi

#in=$2

out=`basename $in .in`
pat1="s#\@\@ORACC\@\@#${ORACC}#g"
pat2="s#\@\@ORACC_HOST\@\@#${ORACC_HOST}#g"
pat3="s#\@\@ORACC_HOME\@\@#${ORACC_HOME}#g"
pat4="s#\@\@ORACC_BUILDS\@\@#${ORACC_BUILDS}#g"
pat5="s#\@\@ORACC_MODE\@\@#${ORACC_MODE}#g"
pat6="s#\@\@ORACC_USER\@\@#${ORACC_USER}#g"
pat7="s#\@\@TeXLive\@\@#${TeXLive}#g"
pat8="s#\@\@ORADMIN\@\@#${ORADMIN}#g"
pat="$pat1;$pat2;$pat3;$pat4;$pat5;$pat6;$pat7;$pat8"
rm -f $out
perl -C0 -pe "$pat" <$in >$out
chmod -w $out
