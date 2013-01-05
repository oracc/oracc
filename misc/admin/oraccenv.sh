#!/bin/sh
umask 026
ORACC=/usr/local/oracc
ORACC_VAR=/var/local/oracc
ORACC_HOME=/home
ORACC_HOST=oracc.museum.upenn.edu
export ORACC ORACC_VAR ORACC_HOME ORACC_HOST
PATH=$PATH:$HOME/00bin:$ORACC/bin
export PATH
