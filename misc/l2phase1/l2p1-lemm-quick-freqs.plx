#!/usr/bin/perl
use warnings; use strict;

my $project = shift @ARGV;

my %words = ();
my %freqs = ();

my @freq_sigs = ('01bld/from-xtf-glo.sig', '01bld/from-prx-glo.sig');

foreach my $f (@freq_sigs) {
    unless (open(F, $f)) { 
	warn "l2p1-lemm-quick_freqs.plx: no signatures file $f\n"
	    unless $f =~ /prx/ && !-s '00lib/proxy.lst';
	next;
    }
    while (<F>) {
	next if /^\@(?:project|name|lang)/ || /^\s*$/;
	chomp;
	my @fields = split(/\t/,$_);
	my $sig;
	my $refs;
	if ($#fields == 2) {
	    ($sig,$refs) = ($fields[0],$fields[2]);
	} else {
	    ($sig,$refs) = ($fields[0],$fields[1]);
	}
	if ($refs) {
	    my $freq = ($refs =~ tr/ / /);

	    $sig =~ s/^\@(.*?)%/\@$project%/ if $project;
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

#use Data::Dumper; print Dumper \%freqs;

foreach my $l (<02pub/lemm-*.sig>) {
    if (open(L, $l)) {
	open(O, ">$l.freq");
	while (<L>) {
	    chomp;
	    s/\t.*$//;
#	    if (exists $freqs{$_}) {
#		warn "OK: $_\n";
#	    } else {
#		warn "NO: $_\n";
#	    }
	    my $f = ($freqs{$_} ? $freqs{$_} : 0);
	    print O "$_\t$f\n"
	}
	close(O);
    }
}

1;
