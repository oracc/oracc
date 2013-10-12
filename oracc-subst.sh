#!/bin/sh

function check_env {
    if [ "$2" = "" ]; then
	echo oracc-subst.sh: environment variable $1 is not set--add it to oraccenv.sh. Stop.
	exit 1
    fi
}

ORACC=$1

check_env_or_die ORACC_BUILDS $ORACC_BUILDS
check_env_or_die ORACC_HOME $ORACC_HOME
check_env_or_die ORACC_HOST $ORACC_HOST


in=$2
out=`basename $in .in`
pat1="s#\@\@ORACC\@\@#${ORACC}#g"
pat2="s#\@\@ORACC_HOST\@\@#${ORACC_HOST}#g"
pat3="s#\@\@ORACC_HOME\@\@#${ORACC_HOME}#g"
pat4="s#\@\@ORACC_BUILDS\@\@#${ORACC_BUILDS}#g"
pat="$pat1;$pat2;$pat3;$pat4"
rm -f $out
perl -C0 -pe "$pat" <$in >$out
chmod -w $out
