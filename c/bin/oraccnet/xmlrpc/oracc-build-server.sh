#!/bin/sh
/bin/echo -n completed > /usr/local/oracc/tmp/rpc/$1/status
/bin/echo 'Hello world!' > /usr/local/oracc/tmp/rpc/$1/method.log
