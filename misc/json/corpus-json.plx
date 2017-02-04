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
    foreach my $t (@texts) {
	my ($project,$PQX) = split(/:/, $t);
	my ($four) = ($PQX =~ /^(....)/);
	my $xtf = "$ENV{'ORACC'}/bld/$project/$four/$PQX/$PQX.xtf";
	if (-r $xtf) {
	    print "," if $texts++;
	    system "$ENV{'ORACC'}/bin/xcl-json.plx", $t;
	} else {
	    warn "corpus-json.plx: no such file $xtf\n"
	}

    }
}

print "\n]\n}\n";

1;
