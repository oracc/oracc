#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;

GetOptions(
    );
my $currv = '';
while (<>) {
    if (/\@v\S*\s+(\S+)/) {
	$currv = $1;
    } elsif (/\@inote\s+Attinger/) {
	if ($currv) {
	    if (/Attinger.*?(\S)/) {
		my $x = $1;
		if ($x eq '~') {
		    s/~/$currv =>/;
		} else {
		    # warn "$.: missing '~'\n";
		    s/ger\s+/ger $currv => /;
		}
	    } else {
		chomp;
		$_ .= " $currv\n";
	    }
	    s/inote/sys/;
	} else {
	    warn "$.: inote Attinger must immediately follow \@v\n";
	}
    } else {
	$currv = '';
    }
    print;
}

1;
