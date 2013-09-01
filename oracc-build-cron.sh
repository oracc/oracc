#!/bin/sh
ds=`dropbox status`
if [ "$ds" = "Dropbox isn't running!" ]; then
    dropbox start 
fi
