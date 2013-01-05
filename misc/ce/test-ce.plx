#!/usr/bin/perl
use warnings; use strict;
use lib "$ENV{'ORACC'}/lib";
use ORACC::CE;

ORACC::CE::load();
print ORACC::CE::line(shift @ARGV);
ORACC::CE::unload();

1;

