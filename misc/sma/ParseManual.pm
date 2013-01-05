package ORACC::SMA::ParseManual;
use warnings; use strict; use utf8;

# Parser for Manual parses entered in vpr:root;vsf,nsf or root!isf notations
# ==========================================================================
#

#FIXME: DOESN'T HANDLE ISF YET

my %mslots = (
    vpr_x => 1,
	vpr_nu => 1,
	vpr_na => 1,
	vpr_ga => 1,
	vpr_u => 1,
	vpr_ha => 1,
	vpr_bara => 1,
	vpr_ca => 1,
	vpr_ša => 1,
	vpr_nuc => 1,
	vpr_nuš => 1,
	vpr_inga => 2,
	vpr_mu => 3,
	vpr_ba => 3,
	vpr_V => 3,
	vpr_Vmma => 3,
	vpr_al => 3,
	vpr_a => 4,
	vpr_ra => 4,
	vpr_na => 4,
	vpr_me => 4,
	vpr_ne => 4,
	vpr_m => 5,
	vpr_e => 5,
	vpr_n => 5,
	vpr_b => 5,
	vpr_da => 6,
	vpr_m => 7,
	vpr_e => 7,
	vpr_n => 7,
	vpr_b => 7,
	vpr_ta => 8,
	vpr_ci => 8,
	vpr_ši => 8,
	vpr_ni => 9,
	vpr_l => 9,
	vpr_I => 9,
	vpr_i => 9,
	vpr_m => 10,
	vpr_e => 10,
	vpr_n => 10,
	vpr_b => 10,
	vsf_e => 1,
	vsf_de => 2,
	vsf_en => 3,
	vsf_enden => 3,
	vsf_enzen => 3,
	vsf_ene => 3,
	vsf_ec => 3,
	vsf_eš => 3,
	vsf_a => 4,
	vsf_am => 4,
	vsf_ma => 4,
	vsf_ri => 5,
	vsf_ece => 5,
	vsf_eše => 5,
	nsf_ak => 1,
	nsf_ju => 2,
	nsf_ŋu => 2,
	nsf_zu => 2,
	nsf_ani => 2,
	nsf_bi => 2,
	nsf_me => 2,
	nsf_zunene => 2,
	nsf_anene => 2,
	nsf_bi => 2,
	nsf_ene => 4,
	nsf_e => 7,
	nsf_eE => 7,
	nsf_eL => 7,
	nsf_STOP => 7,
	nsf_0 => 7,
	nsf_0V => 7,
	nsf_0I => 7,
	nsf_ra => 7,
	nsf_da => 7,
	nsf_ta => 7,
	nsf_ece => 7,
	nsf_eše => 7,
	nsf_a => 7,
	nsf_e => 7,
	nsf_eV => 7,
	nsf_gin => 7,
	nsf_men => 8,
	nsf_men => 8,
	nsf_am => 8,
	nsf_menden => 8,
	nsf_mezen => 8,
	nsf_mec => 8,
	nsf_meš => 8,
    );

my %last_slots = (
		  vpr=>10,
		  vsf=>5,
		  isf=>0,
		  nsf=>8,
		  );

sub
mk_refs_parse {
    return {
	parses => [ mk_parse_from_pos(shift) ],
	w      => { wid=>'x00' , lid=>'xFF' , lang=>'sux' , form=>'' }
    };
}

sub
mk_parse_from_morph {
    my $pos_string = shift;
    my ($vpr,$vsf,$isf,$nsf) 
	= ($pos_string =~ /(.*?:)?~(;.*?)?(!.*?)?(,.*)?$/);

    my $pvpr = $vpr || '';
    my $pvsf = $vsf || '';
    my $pnsf = $nsf || '';
#    print STDERR "ParseManual: vpr='$pvpr'; vsf='$pvsf'; nsf='$pnsf'\n";

    $vpr =~ s/:$// if defined $vpr;
    $vsf =~ s/^;// if defined $vsf;
    $nsf =~ s/^,// if defined $nsf;
    my($rvpr,$rroot,$rvsf,$risf,$rnsf);
    $rvpr = mk_parse_ref('vpr', $vpr || '');
    $rvsf = mk_parse_ref('vsf', $vsf || '');
    $risf = mk_parse_ref('isf', $isf || '');
    $rnsf = mk_parse_ref('nsf', defined($nsf) ? $nsf : '');

    my %flat = ();
    flatten('VPR',$last_slots{'vpr'}, $rvpr,\%flat);
    flatten('VSF',$last_slots{'vsf'}, $rvsf,\%flat);
    flatten('NSF',$last_slots{'nsf'}, $rnsf,\%flat);
    \%flat;
}

sub
flatten {
    my($n,$l,$r,$f) = @_;
    my @slots = @{$$r[1]};
    for (my $i = 1; $i <= $l; ++$i) {
	$$f{"$n$i"} = $slots[$i] if $slots[$i];
    }
}

sub
mk_parse_from_pos {
    my $pos_string = shift;
    my ($pos,$vpr,$cfgw,$vsf,$isf,$nsf) 
	= ($pos_string =~ /^(.*?)\#(.*?:)?(.+?)(;.*?)?(!.*?)?(,.*)?$/);
    my $root = $cfgw;
    $root =~ s/\[.*$//;

    # There are two (!) forms of manual notation in use at present.
    # In tests, we write, e.g., N#an[heaven],0.  In XPH we write, e.g.,
    # an[heaven]N#an,0.  This code handles the discrepancy
    if ($pos !~ /^.*?\[.*?\]/) {
	$pos = "$cfgw$pos";
    }

    my ($pvpr,$ppos,$pvsf,$pnsf);
    $pvpr = $vpr || '';
    $ppos = $pos || '';
    $pvsf = $vsf || '';
    $pnsf = $nsf || '';
#    print STDERR "ParseManual: vpr='$pvpr'; pos='$ppos'; vsf='$pvsf'; nsf='$pnsf'\n";

    $vpr =~ s/:$// if defined $vpr;
    $vsf =~ s/^;// if defined $vsf;
    $nsf =~ s/^,// if defined $nsf;
    my($rvpr,$rroot,$rvsf,$risf,$rnsf);
    $rvpr = mk_parse_ref('vpr', $vpr || '');
    $rvsf = mk_parse_ref('vsf', $vsf || '');
    $risf = mk_parse_ref('isf', $isf || '');
    $rnsf = mk_parse_ref('nsf', defined($nsf) ? $nsf : '');
    my $jroot = $root;
    $rroot = [-1 , $root , $jroot ];
    return mk_parse($rvpr,$rroot,$rvsf,$risf,$rnsf,$pos);
}

sub
mk_parse_ref {
    my ($chain,$morph) = @_;
#    print STDERR "ParseManual: mk_parseref: chain=$chain; morph='$morph'\n";
    my $a = set_slots($chain,split(/\./, $morph));
    [ 10000 , $a ];
}

sub
set_slots {
    my ($chain,@m) = @_;
    my @ret = ();
    my $virtual = '';
    my $next_slot = 0;
    foreach my $m (@m) {
	$virtual = '';
	$m =~ s/\+$//;
	$m =~ s/^\*// && ($virtual = '*');
	my $key = "${chain}_$m";
	my $slot = $mslots{$key};
	if (defined $slot) {
#	    print STDERR "ParseManual: assigning $key to slot $mslots{$key}\n";
	    if ($m eq 'ak') {
		if ($next_slot == 2) { # only happens with ,ak.ak
		    $ret[$slot] .= ".$virtual$m";
		} else {
		    $slot = next_ak_slot($next_slot);
		    $ret[$slot] = "$virtual$m";
		}
	    } elsif ($chain eq 'vpr' && $m =~ /^[menb]$/) {
		$slot = next_menb_slot($next_slot);
		$ret[$slot] = "$virtual$m";
	    } elsif ($key eq 'nsf_ene') {
		$slot = next_ene_slot($next_slot);
		$ret[$slot] = "$virtual$m";
	    } elsif ($key eq 'vpr_na') {
		unless ($next_slot) {
		    $slot = 1;
		    $m = 'na+';
		}
		$ret[$slot] = "$virtual$m";
	    } else {
		$ret[$slot] = "$virtual$m";
	    }
	    $next_slot = $slot+1;
	} else {
	    print STDERR "ParseManual: unknown key $key\n";
	}
    }
    \@ret;
}

sub
next_ak_slot {
    my $slot = shift;
    if ($slot <= 1) {
	1;
    } elsif ($slot <= 3) {
	3;
    } else {
	5;
    }
}

sub
next_ene_slot {
    my $slot = shift;
    if ($slot <= 4) {
	4;
    } else {
	6;
    }
}

sub
next_menb_slot {
    my $slot = shift;
    if ($slot <= 5) {
	5;
    } elsif ($slot <= 7) {
	7;
    } else {
	10;
    }
}

sub
mk_parse {
    my %tmp;
    @tmp{qw/vpr root vsf isf nsf pos/} = @_;
    \%tmp;
}

1;
