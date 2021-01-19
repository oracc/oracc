#!/bin/sh
## NOTE: this means the @field line is wrong in 01tmp/l2p1-simple.sig but
## it is presently ignored by the only program that uses this file.
if [ -r 01bld/project.sig ]; then
    grep -v '^{' 01bld/project.sig | cut -f1-2 >01tmp/l2p1-simple.sig
fi
