#!/usr/bin/perl
use warnings; use strict;

my %words = ();
my %freqs = ();
my @freq_sigs = ('01bld/from-xtf-glo.sig', '01bld/from-proxy-xtf.sig');

foreach my $f (@freq_sigs) {
    unless (open(F, $f)) { 
	warn "l2p1-lemm-quick_freqs.plx: no signatures file $f\n";
	next;
    }
    while (<F>) {
	my($sig,$refs) = (/^(.*?)\t(.*?)$/);
	if ($refs) {
	    my $freq = ($refs =~ tr/ / /);
	    $freqs{$sig} += $freq;

# pct calcs TODO
#	    my $w = $sig;
#	    $w =~ s/^.*?=//;
#	    $w =~ s/\'.*$//;
#	    $w =~ s#//.*?\]#]#;
#	    $words{$w} += $freq;
	}
    }
    close(F);
}

foreach my $l (<02pub/lemm-*.sig>) {
    if (open(L, $l)) {
	open(O, ">$l.freq");
	while (<L>) {
	    chomp;
	    s/\t.*$//;
	    my $f = ($freqs{$_} ? $freqs{$_} : 0);
	    print O "$_\t$f\n"
	}
	close(O);
    }
}

1;
