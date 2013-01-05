#!/bin/sh
java -cp ~/lib/java/saxon.jar net.sf.saxon.Query -t -q:$1
