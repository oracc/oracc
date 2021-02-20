#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;

GetOptions(
    );

my $gw = '0000';
my %seen = ();

while (<>) {
    if (/\@entry\S*\s+(.*?)\s*$/) {
	my $cgp = $1;
	if ($seen{$cgp}++) {
	    s/\[.*?\]/[$gw]/;
	    ++$gw;
	    print;
	    print ">> $cgp\n";
	} else {
	    print;
	}
    } else {
	print;
    }
}

1;
