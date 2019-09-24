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
	my $key = "$_";
	my $m2 = $m2{$key};
	if ($m2) {
	    $s .= $m2;
	    $s .= '.';
	} else {
	    warn "m2: no equivalency for $key\n";
	}
    }
    $s =~ s/\.$//;
    $s;
}

sub m2_init {
    my $prefix = '';
    while (<DATA>) {
	next if /^\s*$/;
	chomp;
	s/\t+/\t/g;
	my($m,$m2) = split(/\t/,$_);
	$m2{$m} = $m2;
    }
    # use Data::Dumper; print Dumper \%m2;
}

1;

########################################################

__DATA__

vpr₀₁=0		IND
vpr₀₁=nu	NEG
vpr₀₁=bara	M1
vpr₀₁=ga	M2
vpr₀₁=ha	M3
vpr₀₁=na	M4
vpr₀₁=nuš	M5
vpr₀₁=u		M6
vpr₀₁=ša	M7

vpr₀₂=inga	CNJ

vpr₀₃=mu	C1
vpr₀₃=ba	C2
vpr₀₃=V		C3
vpr₀₃=Vmma	C4
vpr₀₃=al	C5

vpr₀₄=a		D1s
vpr₀₄=ra	D2s
vpr₀₄=na	D3s
vpr₀₄=me	D1p
vpr₀₄=ne	D3p

vpr₀₅=m		Pm
vpr₀₅=e		P2
vpr₀₅=n		P3a
vpr₀₅=b		P3i

vpr₀₆=da	COM

vpr₀₇=m		Pm
vpr₀₇=e		P2
vpr₀₇=n		P3a
vpr₀₇=b		P3i

vpr₀₈=ši	ALL
vpr₀₈=ta	ABL

vpr₀₉=ni	L0C0
vpr₀₉=a		L0C1
vpr₀₉=i		L0C2

vpr₁₀=m		Pm
vpr₁₀=e		P2
vpr₁₀=en	P0	for 1p use P0-1 for 2p use P0-2
vpr₁₀=n		P3a
vpr₁₀=b		P3i

nsf₀₁=ak	GEN
nsf₀₂=ŋu	POSS1s
nsf₀₂=zu	POSS2s
nsf₀₂=ani	POSS3sa
nsf₀₂=bi	POSS3si
nsf₀₂=me	POSS1p
nsf₀₂=zunene	POSS2p
nsf₀₂=anene	POSS3p
nsf₀₂=bida	POSS3d

nsf₀₃=ak	GEN

nsf₀₄=ene	PLUR

nsf₀₅=ak	GEN

nsf₀₆=ene	PLUR

nsf₀₇=e		ERG
nsf₀₇=0		ABS
nsf₀₇=ra	DAT
nsf₀₇=ta	ABL
nsf₀₇=da	COM
nsf₀₇=a		LC1
nsf₀₇=l		LC2
nsf₀₇=v		VOC
nsf₀₇=eše	ALL
nsf₀₇=gin	EQU

nsf₀₈=men1	COP1s
nsf₀₈=men2	COP2s
nsf₀₈=am	COP3s
nsf₀₈=menden	COP1p
nsf₀₈=menzen	COP2p
nsf₀₈=meš	COP3p
