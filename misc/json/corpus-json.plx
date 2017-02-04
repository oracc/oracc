#!/usr/bin/perl
use warnings; use strict;

my $project = shift;
die "corpus-json.plx: must give project on command line\n"
    unless $project;

my $list = "$ENV{'ORACC'}/bld/$project/lists/lemindex.lst";
my $texts = 0;

print "{\n\"type\": \"corpus\",\n\"project\": \"$project\",\n\"members\": [\n";

if (-r $list) {
    my @texts = `cat $list`; chomp @texts;
    foreach (@texts) {
	print "," if $texts++;
	system "$ENV{'ORACC'}/bin/xcl-json.plx", $_;
    }
}

print "\n]\n}\n";

1;
