#!/bin/sh
cat 01bld/from-{xtf,prx}-glo.sig | cut -f2 | tr ' ' '\n' | wc -l

