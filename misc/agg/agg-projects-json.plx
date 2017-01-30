#!/usr/bin/perl
use warnings; use strict;

open(O,">$ENV{'ORACC_BUILDS'}/www/projects.json"); select O;
my @x=`agg-list-public-projects.sh`; chomp(@x); @x = map{"\"$_\""} @x;
print "{\n\t\"type\": \"projects\",\n";
print "\t\"public\": [\n\t\t";
print join(",\n\t\t",@x);
print "\n\t]\n}\n";

1;
