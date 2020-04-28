#!/bin/sh
grep -v ^@fields $1 | cut -f1 | sigx $1 | tr -d °· | gdlme2 -scng >/dev/null
