#!/bin/sh
grep '%' $1 | grep -v '^{' | cut -f1 | sigx $1 | tr -d °· | gdlme2 -scng >/dev/null
