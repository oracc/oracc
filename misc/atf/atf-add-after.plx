#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;

GetOptions(
    );

# Add '#key: after' lines to 00atf/*.atf
#
# Reads a table of PQX<TAB>AFTER-TEXT.

my %a = ();
my $pending = '';

while (<>) {
    chomp;
    my($pqx,$txt) = split(/\t/,$_);
    if ($pqx && $txt) {
	$a{$pqx} = $txt;
    } else {
	warn "$0: syntax error in after-table line: $_\n";
    }
}

system 'rm', '-r', 'new';
system 'mkdir', 'new';
if (-d 'new') {
    foreach my $a (<00atf/*.atf>) {
	my $n = $a; $n =~ s/00atf/new/;
	open(N,">$n") || die "$0: can't write output $n\n";
	select N;
	open(A,$a) || die "$0: can't ready atf $a\n";
	while (<A>) {
	    if (/^\&([PQX]\d{6})/) {
		$pending = $a{$1};
		print;
	    } elsif (/^#/ || /^\s*$/) {
		print;
	    } else {
		if ($pending) {
		    print "\n#key: after $pending\n";
		    $pending = '';
		}
		print;
	    }
	}
	close(A);
    }
} else {
    die "$0: no new/ directory to write output. Stop.\n";
}

1;
