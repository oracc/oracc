package ORACC::SMA2::M2;

require Exporter;
@ISA=qw/Exporter/;
@EXPORT = qw/m2/;

use warnings; use strict; use utf8;

my $m2_inited = 0;
my %m2 = ();

sub m2 {
    m2_init() unless $m2_inited;
    my $m = shift;
    my ($vpr,$vsf,$isf,$nsf) = ('', '', '', '');
    my $m2 = '';
    if ($m =~ s/^(.*?)://) {
	$vpr = $1;
    }
    if ($m =~ s/,(.*)$//) {
	$nsf = $1;
    }
    if ($m =~ s/\!(.*)$//) {
	$isf = $1;
    } elsif ($m =~ s/;(.*?)$//) {
	$vsf = $1;
    }
    if ($vpr) {
	$m2 .= m2m2('vpr',$vpr);
	$m2 .= ':';
    }
    $m2 .= $m;
    if ($isf) {
	$m2 .= '!';
	$m2 .= m2m2('isf',$isf);
    } elsif ($vsf) {
	$m2 .= ';';
	$m2 .= m2m2('vsf',$vsf);
    }
    if ($nsf) {
	$m2 .= ',';
	$m2 .= m2m2('nsf',$nsf);
    }	
    $m2;
}

sub m2m2 {
    my($prefix,$chain) = @_;
    my @c = split(/\./,$chain);
    my $s = '';
    foreach (@c) {
	my $m2 = $m2{$prefix,$_};
	if ($m2) {
	    $s .= $m2;
	    $s .= '.';
	} else {
	    warn "m2: no equivalency for $prefix , $_\n";
	}
    }
    $s =~ s/\.$//;
    $s;
}

sub m2_init {
    my $prefix = '';
    while (<DATA>) {
	next if /^\s*$/;
	if (/^(vpr|vsf|isf|nsf)\s*$/) {
	    $prefix = $1;
	} else {
	    chomp;
	    my($m,$m2) = split(/\t/,$_);
	    $m2{$prefix,$m} = $m2;
	}
    }
#    use Data::Dumper; print Dumper \%m2;
}

1;

########################################################

__DATA__

vpr

₁0	IND
₁nu	NEG
₁bara	M1
₁ga	M2
₁ha	M3
₁na	M4
₁nuš	M5
₁u	M6
₁ša	M7

₂inga	CNJ

₃mu	C1
₃ba	C2
₃V	C3
₃Vmma	C4
₃al	C5

₄a	D1s
₄ra	D2s
₄na	D3s
₄me	D1p
₄ne	D3p

₅m	Pm
₅e	P2
₅n	P3a
₅b	P3i

₆da	COM

₇m	Pm
₇e	P2
₇n	P3a
₇b	P3i

₈ši	ALL
₈ta	ABL

₉ni	L0C0
₉a	L0C1
₉i	L0C2

₁₀m	Pm
₁₀e	P2
₁₀en	P0	for 1p use P0-1 for 2p use P0-2
₁₀n	P3a
₁₀b	P3i


1;
