#!/bin/sh
if [ -r 00lib/config.xml ]; then
    mkdir -p 00web/css
    if [ -d 00web/css ]; then
	echo '@import url(/css/colour_nn.css)' >00web/css/projcss.css
	if [ -r 00web/css/projcss.css ]; then
	    echo "$0: 00web/css/projcss.css successfully created."
	    echo "\tColourized transliterations will work after next rebuild--"
	    echo "\tusers will probably need to clear caches before seeing them."
	    exit 0
	else
	    echo "$0: failed to create 00web/css/projcss.css. Stop."
	    exit 1
	fi
    else
	echo "$0: failed to create 00web/css. Stop."
	exit 1
    fi    
else
    echo "$0: this program must be run in a project directory. Stop."
    exit 1
fi
