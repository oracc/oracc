#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;

GetOptions(
    );

my %seen = ();
while (<>) {
    if (/^\@(sign|form)/) {
	%seen = ();
	print;
    } elsif (/^\@list\s+(\S+)\s*$/) {
	print unless $seen{$1}++;
    } else {
	print;
    }    
}

1;
