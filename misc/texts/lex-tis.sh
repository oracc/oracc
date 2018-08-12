#!/bin/sh
if [ -r 02pub/base.tis ]; then
    cat 02pub/base.tis >>02pub/cbd/sux/sux.tis
fi
if [ -r 02pub/word.tis ]; then
    cat 02pub/word.tis >>02pub/cbd/sux/sux.tis
fi
