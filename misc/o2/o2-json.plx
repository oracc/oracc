#!/usr/bin/perl
use warnings; use strict;
my $project = shift @ARGV;

my @x=(<>); chomp(@x); @x = map{"\"$_\""} @x;
print "{\n\t\"type\": \"manifest\",\n";
print "\t\"project\": \"$project\",\n";
print "\t\"files\": [\n\t\t";
print join(",\n\t\t",@x);
print "\n\t],\n";
print "\t\"everything\": \"json.zip\"\n}\n";

1;
