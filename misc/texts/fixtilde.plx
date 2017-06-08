#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
undef $/;
my @files = ();
my $argf = shift @ARGV;
if ($argf) {
    push @files, $argf;
} else {
    @files = (<00atf/*.atf>, <00lib/*.glo>);
}
foreach my $f (@files) {
    open(F,$f); $_ = <F>; close(F);
    my $olen = length;
    if (tr/∼/~/) {
	my $nlen = length;
	warn "fixtilde.plx: fixed tildes in $f\n";
	if ($argf) {
	    print $_;
	} else {
	    open(O,">$f"); print O; close(O);
	}
    }
}

1;
