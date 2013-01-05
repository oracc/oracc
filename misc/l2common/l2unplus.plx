#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::XMD::Fields;

binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';

my @files = ();

if ($#ARGV >= 0) {
    @files = @ARGV;
} else {
    @files = <00atf/*.atf>;
}

foreach my $f (@files) {
    open(F,$f);
    my @lines = (<F>);
    close(F);
    if (open(F,">$f")) {
	select F;
	foreach (@lines) {
	    if (/^\#lem:/) {
		s/([:;])(\s+)\+(?!\.)/$1$2/g;
		s/\+\.(\s+)\+/+.$1/g;
		s/\&\+/\&/g;
	    }
	    print;
	}
	close(F);
    } else {
	warn "l2unplus.plx: can't save $f\n";
    }
}

1;
