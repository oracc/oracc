#!/bin/sh
perl -pi -e 's/ATF mode version ....-..-../ATF mode version '`date --iso`'/' atf-mode.el
zip -u atf-mode
