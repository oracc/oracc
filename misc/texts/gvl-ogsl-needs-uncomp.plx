#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;

GetOptions(
    );

my $fixc = '';

while (<>) {
    if (/unknown compound:\s+(\S+)\s*$/) {
	$fixc = $1;
	if (/\[(\S+?) <= (\S+?)\]\s*$/) {
	    $fixc = $1;
	} elsif (/also tried\s+(\S+?)\)$/) {
	    my $tmp = $1;
	    unless ($fixc =~ /[()]/) { # keep versions with parens
		$fixc .= "//$tmp";
	    }
	} else {
	}
	print "\@sign\t$fixc\n\@inote gvl unknown compound\n\@end sign\n\n";
    }
}

1;
