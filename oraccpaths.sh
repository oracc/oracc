#!/bin/bash
PATH=$PATH:$ORACC_BUILDS/bin
if [[ $ORACC_MODE == "single" ]]; then
    PATH=$PATH:$ORACC_BUILDS/sbin
fi
export PATH
