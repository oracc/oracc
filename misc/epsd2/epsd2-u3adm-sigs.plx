#!/usr/bin/perl
use warnings; use strict; use open ':utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

my @subs = ();

foreach (`cat u3adm/00lib/umbrella.lst`) {
    chomp;
    push @subs, split(/\s+/, $_);
}

my %x = ();

foreach my $s (@subs) {
    my $fxg = "u3adm/$s/01bld/from-xtf-glo.sig";
    if (-r $fxg) {
	open(F,$fxg);
	while (<F>) {
	    chomp;
	    my($sig,$insts) = split(/\t/,$_);
	    foreach my $i (split(/\s+/, $insts)) {
		++${$x{$sig}}{$i};
	    }
	}
	close(F);
    } else {
	warn "epsd2-u3adm-sigs.plx: no sigs file `$fxg'\n";
    }
}

my $output = 'u3adm/01bld/from-xtf-glo.sig';
open(F,">$output") 
    || die "epsd2-u3adm-sigs.plx: can't open $output for write\n";
foreach my $s (sort keys %x) {
    print F
	$s, 
	"\t",
	join(' ', sort keys %{$x{$s}}),
	"\n";
}
close(F);
1;
