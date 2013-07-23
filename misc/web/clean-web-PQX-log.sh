#!/bin/sh
perl -n -e '/failed to load external entity.*(uxh|xst|xtr)/ || print' 01tmp/web-PQX.log
