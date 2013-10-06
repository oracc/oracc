#!/bin/sh
FLAGS=p:

## Options
args=`getopt $FLAGS $*`
echo args=$args
for args; do
    case "$args" in
	-p ) project=$2
	     shift ; shift ;;
	-- ) shift; break ;;
    esac
done

if [ $? != 0 -o $# != 0 ] ; then
  echo ""
  echo "Usage: oracc-build-client.sh {args}"
  echo ""
  exit 0
fi

## Check we are in proper project location


## User/Pass
. ~/.oracc-user

## Package
version=`cat .version`
#zip=$project
#zip=`/bin/echo -n $project | tr / -`.zip
zip=request.zip
zip -q $zip 00atf/*.atf 00lib/config.xml 00lib/*.glo

## Send
echo oracc-client -m build -u $user -P $pass -p $project -v $version -Mfile:zip=$zip
oracc-client -m build -u $user -P $pass -p $project -v $version -Mfile:zip=$zip

## Report
cat build.log
