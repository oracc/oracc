#!/bin/sh
use warnings; use strict;

my @V = <V[0-9][0-9][0-9]>;

print join("\n", @V), "\n";

1;
