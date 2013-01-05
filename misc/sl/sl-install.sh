#!/bin/sh
#
# This script caches the current version of the OGSL
# main data file so that it's easy to build an ATF
# runtime distribution that includes ogsl.xml.
#
cp ${ORACC}/xml/ogsl/ogsl.xml ${ORACC}/lib/data
chmod o+r ${ORACC}/lib/data/ogsl.xml
