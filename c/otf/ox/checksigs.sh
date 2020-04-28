#!/bin/sh
grep '%' $1 | cut -f1 | sigx $1 | tr -d °· | gdlme2 -scng >/dev/null
