#!/bin/sh
find 00web -name '*.xml' | xargs grep -l esp:page | xargs wc -w
