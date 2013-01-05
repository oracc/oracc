#!/bin/sh
perl -ne '/(?:^\@sense\s+|\])([A-Z]N)(?![A-Za-z])/&&++$x{$1};END{print join(" ", sort keys %x)}' $1;
