#!/usr/bin/perl
use warnings; use strict;

my $project = shift @ARGV;

my %words = ();
my %freqs = ();
my %f = ();

sub
set_f {
    my $fields = shift;
    %f = ();
    my @f = split(/\s/, $fields);
    shift @f; # drop '@field';
    for (my $i = 0; $i <= $#f; ++$i) {
	$f{$f[$i]} = $i;
    }
}

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
	if (/^\@fields/) {
	    set_f($_);
	    next;
	}
	my @fields = split(/\t/,$_);
	my $sig = $fields[0];
	my $refs = $fields[$f{'inst'}];
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
	print O "\@fields sig rank freq\n";
	while (<L>) {
	    next if /^\@fields/;
	    chomp;
	    my @t = split(/\t/,$_);
	    $t[1] = '0' unless (defined $t[1] && $t[1]);
	    my $f = ($freqs{$_} ? $freqs{$_} : '0');
	    print O "$t[0]\t$t[1]\t$f\n"
	}
	close(O);
    }
}

1;
