#!/bin/sh
echo Running tests
for a in t/*.t ; do ./runfile.sh $a ; done
