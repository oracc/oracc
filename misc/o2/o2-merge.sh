#!/bin/sh
lang=$1
project=`oraccopt`
if [ "$project" == "" ]; then
    echo oracc merge must be run from a project directory
    exit 1
fi
if [ "$lang" == "" ]; then
    echo oracc merge must be called with a language code as its only argument
    exit 1
fi
if [ ! -r 01bld/new/$lang.new ]; then
    echo oracc merge: nothing to do for language $lang
    exit 1
fi
if [ ! -r 00lib/$lang.glo ]; then
    echo oracc merge: moving 01bld/new/$lang.new to 00lib/$lang.glo
    mv 01bld/new/$lang.new 00lib/$lang.glo
    exit 0
fi
if [ ! -s 00lib/$lang.glo ]; then
    echo oracc merge: replacing empty 00lib/$lang.glo with 01bld/new/$lang.new
    mv 01bld/new/$lang.new 00lib/$lang.glo
    exit 0
fi

# Now we know that 00lib/$lang.glo is non-zero
date=`isogmt`
date=`/bin/echo -n $date | tr -d -`
mkdir -p 00bak
echo oracc merge: saving 00lib/$lang.glo as 00bak/$lang-$date.glo
cp -a 00lib/$lang.glo 00bak/$lang-$date.glo

# Now do the actual merge
l2-glomanager.plx -merge -cbdlang $lang \
    -mlist 00lib/$lang.glo,01bld/new/$lang.new > 01tmp/$lang.mrg \
    && mv 01tmp/$lang.mrg 00lib/$lang.glo \
    && echo oracc merge $lang successful \
    && exit 0

echo oracc merge $lang failed: 00lib/$lang.glo unchanged
exit 1
