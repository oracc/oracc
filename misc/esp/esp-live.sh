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

if [ "$2" = "" ]; then
    echo esp-live.sh: must give webdir name as second argument
    exit 1
else
    echo esp-live.sh: forcing webdir to 01bld/www
    webdir=01bld/www
fi

if [ ! -d 00web/esp ]; then
    echo esp-live.sh: must run in a project with 00web/esp
    exit 1
fi

if [ "$3" != "" ]; then
    if [ "$3" = "force" ]; then
	force=yes
    else
	echo esp-live.sh: third argument may only be 'force'
	exit 1
    fi
fi

project=$1
webdir=$2
mkdir -p $webdir
if [ ! -d $webdir ]; then
    echo esp-live.sh: $webdir does not exist and cannot be created
    exit 1
fi

esp=/usr/local/oracc/$project/00web/esp

if [ "$force" != "yes" ]; then
    echo "Make development directory live -- are you sure?"
    echo "(type 'yes' or 'no' then press [Enter])"
    read CONFIRM
    if ! test $CONFIRM = yes
    then exit 1
    fi
fi

HDEV="$esp/httproot_dev"
HLIVE="$esp/httproot_live"
HCHANGED="$esp/httproot_changed"
HGONE="$esp/httproot_gone"
BACKUPS="$esp/backups"

HLIVENEW="${HLIVE}_new"
HCHANGEDNEW="${HCHANGED}_new"
HGONENEW="${HGONE}_new"
BACKUPDIR="$BACKUPS/`date +"%Y-%m-%d_%H.%M.%S"`"

cd $HDEV
HDEVFILES="$(find . -type f)"
# -type f => files only

echo Creating new live and changed directories...

for HDEVFILE in $HDEVFILES
do 
	echo
	echo File: $HDEVFILE
	echo Directory: `dirname "$HLIVENEW/$HDEVFILE"`

	mkdir -p `dirname "$HLIVENEW/$HDEVFILE"`
	
	# If file is present and identical in HLIVE

	if find "$HLIVE/$HDEVFILE" && diff --brief "$HDEV/$HDEVFILE" "$HLIVE/$HDEVFILE"
	then	
		cp -p "$HLIVE/$HDEVFILE" "$HLIVENEW/$HDEVFILE"

	# Otherwise (ie. if file is different or not present in HLIVE)
	else
		mkdir -p `dirname "$HCHANGEDNEW/$HDEVFILE"`
		cp -p "$HDEV/$HDEVFILE" "$HLIVENEW/$HDEVFILE"
		cp -p "$HDEV/$HDEVFILE" "$HCHANGEDNEW/$HDEVFILE"
	fi
done

echo Creating gone list...

cd $HLIVE
HLIVEFILES="$(find . -type f)"

for HLIVEFILE in $HLIVEFILES
do
    if ! find "$HLIVENEW/$HLIVEFILE"; then
	echo $HLIVEFILE >> $HGONENEW
	if [ -r 02www/$HLIVEFILE ]; then
	    rm -fr 02www/$HLIVEFILE
	fi
    fi
done

echo Backing up old directories and replacing with new ones...

mkdir -p $BACKUPDIR

mv $HLIVE $BACKUPDIR
mv $HLIVENEW $HLIVE

if [ -d $HCHANGEDNEW ]; then
  mv $HCHANGED $BACKUPDIR
  mv $HCHANGEDNEW $HCHANGED
fi

if [ -d $HGONENEW ]; then
  mv $HGONE $BACKUPDIR
  mv $HGONENEW $HGONE
fi

cd $ORACC/$project/$webdir

## This needs to be done more carefully: if you do this
## you hose all subprojects  ...
#rm -fr * .[a-zA-Z0-9_]*

#n.b.: if you use mv to update live then the
#testing of changed/unchanged files above fails
cp -a $esp/httproot_live/* $esp/httproot_live/.[a-zA-Z0-9_]* .
cd $ORACC/$project
chmod -R o+r $webdir
echo esp content version now live at oracc/$project
exit 0
