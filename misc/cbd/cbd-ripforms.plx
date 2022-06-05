#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;

GetOptions(
    );

my %f = ();

while (<>) {
    if (s/^\@bases\s+//) {
	chomp;
	my @b = split(/;\s+/,$_);
	foreach my $b (@b) {
	    $b =~ s/\s+\(.*$//;
	    ++$f{$b};
	}
    } elsif (/^\@form/) {
	/\s(\S+)\s/;
	++$f{$1};
    }
}

print join("\n", map { tr/·//d; $_; } sort keys %f), "\n";

1;
