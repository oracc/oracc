#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC_BUILDS'}/lib";

my $cbd = shift @ARGV;
my $dropping = 0;
my %seen = ();
die "$0: must give .glo file on command line\n" unless $cbd;
die "$0: can't read .glo file $cbd\n" unless -r $cbd;
open(G,$cbd) || die "$0: error opening .glo file $cbd\n";
open(D,'>dups.glo');
open(N,'>new.glo');
while (<G>) {
    if (/^\@entry\S*\s+(.*?)\s*$/) {
	my $cgp = $1;
	if ($seen{$cgp}) {
	    $dropping = 1;
	} else {
	    $dropping = 0;
	    ++$seen{$cgp};
	}
    } elsif (/^\@(?:lang|name|project)/) {
	print D;
	print N;
	next;
    }
    if ($dropping) {
	print D;
    } else {
	print N;
    }
}
close(D);
close(N);

warn "===\n$0: dups left in dups.glo, remainder of glossary in new.glo\n===\n";

1;
