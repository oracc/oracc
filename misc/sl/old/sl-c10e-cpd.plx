#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;

GetOptions(
    );

while (<>) {
    if (/\@aka/) {
	my ($f,$l,$s,$c,$n) = (/^(.*?):(.*?): (\S+) in (\S+) should be (\S+?);/);
	if ($f) {
	    my $cc = $c; $cc =~ tr/\|//d;
	    my @c = split(/([+×\.])/, $cc);
	    my @n = ();
	    my $subbed = 0;
	    foreach my $c (@c) {
		# warn "s=$s; c=$c\n";
		if ($s eq $c) {
		    push @n, $n;
		    ++$subbed;
		} else {
		    push @n, $c;
		}
	    }
	    my $nc = '';
	    if ($subbed) {
		$nc = join('', @n);
		$nc =~ tr/\|//d;
		$nc = "|$nc|";
	    } else {
		$nc = $c;
		warn "no subs made in $s should be $n in $c\n";
	    }
	    # print "$f:$l: $c => $nc\n";
	    print "$c\t$nc\n";
	} else {
	    warn "failed match\n";
	}
    }
}

1;
