#!/bin/sh
if [ -r 02pub/base.tis ]; then
    cat 02pub/base.tis >>02pub/cbd/sux/sux.tis
    (cd 02pub ; mv base.tis base.tis.done)
    echo lex-tis.sh added base.tis to 02pub/cbd/sux/sux.tis and renamed it to base.tis.done
fi
if [ -r 02pub/word.tis ]; then
    cat 02pub/word.tis >>02pub/cbd/sux/sux.tis
    (cd 02pub ; mv word.tis word.tis.done)
    echo lex-tis.sh added word.tis to 02pub/cbd/sux/sux.tis and renamed it to word.tis.done
fi
