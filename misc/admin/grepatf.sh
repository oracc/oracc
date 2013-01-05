#!/bin/sh
allprojatf.sh | tr '\n' '\0' | xargs -0 grep $* 2>/dev/null
