#!/bin/sh
atf2xtf -BCD $1.atf >$1.xtf
ssa.plx ./$1.xtf
