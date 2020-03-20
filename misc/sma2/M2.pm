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
	    my $slot = $key;
	    $slot =~ s/=.*$//;
	    $s .= "$slot=$m2";
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

vp₀₁=0		IND
vp₀₁=nu	NEG
vp₀₁=bara	M1
vp₀₁=ga	M2
vp₀₁=ha	M3
vp₀₁=na	M4
vp₀₁=nuš	M5
vp₀₁=u		M6
vp₀₁=ša	M7

vp₀₂=inga	CNJ

vp₀₃=mu	C1
vp₀₃=ba	C2
vp₀₃=V		C3
vp₀₃=Vmma	C4
vp₀₃=al	C5

vp₀₄=a		D1s
vp₀₄=ra	D2s
vp₀₄=na	D3s
vp₀₄=me	D1p
vp₀₄=ne	D3p

vp₀₅=m		Pm
vp₀₅=e		P2
vp₀₅=n		P3a
vp₀₅=b		P3i

vp₀₆=da	COM

vp₀₇=m		Pm
vp₀₇=e		P2
vp₀₇=n		P3a
vp₀₇=b		P3i

vp₀₈=ši	ALL
vp₀₈=ta	ABL

vp₀₉=ni	L0C0
vp₀₉=a		L0C1
vp₀₉=i		L0C2

vp₁₀=m		Pm
vp₁₀=e		P2
vp₁₀=en	P0	for 1p use P0-1 for 2p use P0-2
vp₁₀=n		P3a
vp₁₀=b		P3i

vs₀₁=e		AF

vs₀₂=de       	FO

vs₀₃=en	P0s	for 1p use P0s-1 for 2p use P0s-2
vs₀₃=e		P3s
vs₀₃=enden	P1p
vs₀₃=enzen	P2p	
vs₀₃=ene	P3p
vs₀₃=eš	P3p

vs₀₄=a		NOM
vs₀₄=am       	COP

ns₀₁=ak	GEN
ns₀₂=ŋu	POSS1s
ns₀₂=zu	POSS2s
ns₀₂=ani	POSS3sa
ns₀₂=bi	POSS3si
ns₀₂=me	POSS1p
ns₀₂=zunene	POSS2p
ns₀₂=anene	POSS3p
ns₀₂=bida	POSS3d

ns₀₃=ak	GEN

ns₀₄=ene	PLUR

ns₀₅=ak	GEN

ns₀₆=ene	PLUR

ns₀₇=e		ERG
ns₀₇=0		ABS
ns₀₇=ra	DAT
ns₀₇=ta	ABL
ns₀₇=da	COM
ns₀₇=a		LC1
ns₀₇=l		LC2
ns₀₇=v		VOC
ns₀₇=eše	ALL
ns₀₇=gin	EQU

ns₀₈=men1	COP1s
ns₀₈=men2	COP2s
ns₀₈=am	COP3s
ns₀₈=menden	COP1p
ns₀₈=menzen	COP2p
ns₀₈=meš	COP3p
