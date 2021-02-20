#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;



GetOptions(
    );


# coerce senses to match entries only where POS =~ /[A-Z]N/

my $possense = '';
my $printed = 0;

while (<>) {
    if (/\@entry\S*\s+(.*)\s*$/) {
	my $cgp = $1;
	$cgp = /\[(.*?)\]\s+(\S+)/;
	$possense = "$2 $1";
	$printed = 0;
    } elsif (/end\s+entry/) {
	$possense = '';
    }
    if (/\@sense/) {
	print "\@sense $possense\n" unless $printed++;
    } else {
	print;
    }
}

1;
