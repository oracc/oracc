#!/usr/bin/perl
use warnings; use strict; use open 'utf8';

my %l = ();
my %langs = ();
my %sig = ();

open(L,'00lib/neo.lst');
my @l = (<L>); chomp @l; @l{map { s/^\s*(\S+)\s*$/$1/; $_} @l} = ();
close(L);

open(O, '00lib/order.lst')
    || die "neo-sigs.plx: can't open neo/00lib/order.lst\n";
while (<O>) {
    chomp;
    my $prj = $_;
    my $sig = "../$prj/01bld/from-xtf-glo.sig";
    if (-r $sig) {
	if (open(S,$sig)) {
	    while (<S>) {
		next unless /%(.*?):/;
		my $lang = $1;
		if ($lang =~ /-/) {
		    s/%(...).*?:/\%$1:/;
		    $lang =~ s/-.*$//;
		}
		++$langs{$lang};
		chomp;
		s/^\@(.*?)\%/\@neo\%/;
		my($s,$rr) = (split(/\t/,$_));
		foreach my $r (split(/\s+/,$rr)) {
		    my $t = $r; $t =~ s/\..*$//;
		    if (exists($l{$t})) {
			push @{$sig{$s}}, $r;
		    }
		}
	    }
	}
    } else {
	warn "neo-sigs.plx: can't open $sig, skipping it.\n";
    }
}
close(O);

open(S,'>01bld/mega.sig');
print S '@fields sig freq inst', "\n";
foreach my $s (keys %sig) {
    my @r = @{$sig{$s}};
    my $f = 1+$#r;
    print S "$s\t$f\t";
    print S join(' ',@r);
    print S "\n";
}
close(S);

open(L, '>01bld/megalangs');
print L join(' ', sort keys %langs, 'qpn'), "\n";
close(L);

1;
