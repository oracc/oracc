#!/usr/bin/perl
use warnings; use strict;
use Math::Units qw(convert);

print "6000.00mm = ", convert(6000.00, 'mm', 'm'), "m\n";
