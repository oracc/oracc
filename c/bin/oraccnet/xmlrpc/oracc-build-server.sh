#!/bin/sh
/bin/echo -n completed > /usr/local/oracc/tmp/rpc/$1/status
unzip -v /usr/local/oracc/tmp/rpc/$1/request.zip >/usr/local/oracc/tmp/rpc/$1/method.log
