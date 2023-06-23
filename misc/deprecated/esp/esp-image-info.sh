#!/bin/sh
find site-content '(' -name '*.jpg' -or -name '*.png' -or -name '*.gif' ')' -print0 \
    | xargs -0 java -classpath $ORACC/lib/java ImageInfo
