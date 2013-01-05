package ORACC::SSA3::Is;

require Exporter;
@ISA=qw/Exporter/;
@EXPORT = qw/is_forced_cnj is_initial is_msr_np is_aux is_conjp is_unit is_num 
	is_sysdet is_prn is_prof is_proper is_persname is_cnj is_regens is_pp 
	is_erg is_abs is_stop is_leaf is_leaf_abs is_cvn is_hacked_cvn is_dim 
	is_v is_v_finite is_v_nonfinite is_vnf is_vnf_marked is_x is_cop 
	is_vsf_e is_clause_begin is_clause_end is_de is_nom is_nom_leaf 
	is_poss is_plur is_gen is_P2 is_np is_np_gen is_lfd is_np_mod 
	is_np_pos is_np_prn is_n is_person is_n_or_s is_s is_adj is_adja 
	is_modp is_rel is_rrc is_rel_pron is_erg_pron is_finite is_animate 
	is_animate_node is_lt is_trace is_phrase/;

use lib '@@ORACC@@/lib';

use warnings; use strict;
use ORACC::SSA3::Common;
use ORACC::SSA3::Intrans;
use Data::Dumper;

my %pre_syn = ();
my %post_syn = ();

my %pp;
my @pp = qw/ABS ERG DAT COM ABL TER LOC LT VOC EQU INSERTP STOP/;
@pp{@pp} = ();

my %pron_forms;
my @pron = qw/lu[man] lu[person] niŋ[thing] aba[who] ana[what] ud[day] ud[sun] nin[lady]/;
@pron_forms{@pron} = ();

my %erg_forms;
my @erg = qw/lu[man] lu[person] aba[who] ama[mother]/;
@erg_forms{@erg} = ();

my %prof;
my @prof = qw/lugal[king] lu[man] lu[person] ensik[ruler] damgar[merchant] dam[spouse] 
    ugula[overseer] arad[slave] diŋir[deity] sipad[shepherd] nin[lady]
    dumu[child]
/;
push @prof, 'sukkalmah[an official]';
@prof{@prof} = ();

my %anim;
my @anim = qw/ama[mother] dumu[child] saŋ[head] munus[woman] ursaŋ[hero]
    sisa[just]/;
@anim{@anim} = ();

sub
no_pp {
    my $node = shift;
    return 1 if !$$node{'children'} || ref($$node{'children'}) ne 'ARRAY';
    my @c = @{$$node{'children'}};
    if ($c[$#c]{'type'} eq 'leaf') {
	for (my $i = $#c; $i >= 0; --$i) {
	    return 0 if $c[$i]{'type'} eq 'leaf' && exists($pp{$c[$i]{'pos'}});
	}
	return 1;
    } else {
	last_pos($c[$#c]);
    }
    return 1;
}

sub
is_forced_cnj {
    my $n = shift;
#    para_syntax($n,'ante','and');
#    my $w = $$n{'wid'};
#    return $w && $pre_syn{$w} && $pre_syn{$w} =~ /\+&/;
    0;
}

sub
is_initial {
    my $n = shift;
    $$n{'position'} && $$n{'position'} eq 'initial';
}

sub
is_phrase {
    my $n = shift;
    $$n{'type'} && $$n{'type'} eq 'phrase';
}

sub
is_msr_np {
    my $n = shift;
    if ($$n{'type'} eq 'phrase' && $$n{'label'} eq 'CONJP') {
	$n = ${$$n{'children'}}[0];
    }
    if ($$n{'type'} eq 'phrase' && $$n{'label'} eq 'NP') {
	my $c = ${$$n{'children'}}[0];
        return $$c{'type'} eq 'phrase' && $$c{'label'} eq 'NP-MSR';
    }
    0;
}

sub
is_aux {
    my $n = shift;
    my $c = v_cfgw($n);
    $c =~ /^(di|dug|e)\[(?:to )?(?:do|speak)\]|ak\[(?:to )?do\]$/;
}

sub
is_conjp {
    my $n = shift;
    $$n{'type'} eq 'phrase' && $$n{'label'} eq 'CONJP';
}

sub
is_unit {
    my $n = shift;
    $$n{'type'} eq 'leaf' && $$n{'form'} =~ /measure|unit|harvest/;
}

sub
is_num {
    my $n = shift;
    $$n{'type'} eq 'leaf' && $$n{'pos'} eq 'NU';
}

sub
is_sysdet {
    my $n = shift;
    $$n{'type'} eq 'leaf' && $$n{'form'} =~ /gur\[unit\]|GANA₂/;
}

sub
is_prn {
    my ($left,$right,$strict) = @_;
    if ($left && $right) {
	my $lh = head($left);
	my $rh = head($right);

	return 0 
	    if $$rh{'wid'} && $pre_syn{$$rh{'wid'}} && $pre_syn{$$rh{'wid'}} =~ /-,/;

#	print STDERR "is_prn: lh='$$lh{'form'}', rh='$$rh{'form'}'\n";
	if ($$lh{'pos'} =~ /^[DROPST]N$/) {
	    return exists $prof{$$rh{'form'}};
	} elsif ((!$strict || $$rh{'pos'} =~ /^[DROPST]N$/)
		 && (exists($prof{$$lh{'form'}}) && exists($prof{$$rh{'form'}}))) {
	    return 1;
	}
    }
    0
}

sub
is_prof {
    my $n = shift;
    exists $prof{$$n{'form'}};
}

sub
is_proper {
    my $n = shift;
    $$n{'pos'} && $$n{'pos'} =~ /^[DGOPRSTW]N$/;
}

sub
is_persname {
    my $n = shift;
    $n = head($n) unless $$n{'pos'};
    $$n{'pos'} =~ /^[DPR]N$/;
}

sub
is_cnj {
    0;
}

sub
is_regens {
    my $n = shift;
    my @c = @{$$n{'children'}} if $$n{'children'};
    $#c > 0 && is_np_gen($c[1]);
}

sub
is_pp {
    my $node = shift;
    $$node{'type'} eq 'leaf' && exists($pp{$$node{'pos'}});
}

sub
is_erg {
    my $node = shift;
    ($$node{'type'} eq 'leaf' && $$node{'pos'} eq 'ERG');
}

sub
is_abs {
    my $node = shift;
    if ($$node{'type'} eq 'phrase') {
	return $$node{'label'} eq 'NP-ABS' || no_pp($node);
    } elsif ($$node{'type'} eq 'leaf') {
        return $$node{'pos'} =~ /^.?N$|^NC$/;
    } else {
	return 0;
    }
}

sub
is_stop {
    my $node = shift;
    if ($$node{'type'} && $$node{'type'} eq 'leaf') {
        return $$node{'pos'} =~ /^STOP$/;
    } else {
	return 0;
    }
}

sub
is_leaf {
    my $node = shift;
    $$node{'type'} eq 'leaf';
}
sub
is_leaf_abs {
    my $node = shift;
    $$node{'type'} eq 'leaf' && $$node{'pos'} eq 'ABS';
}
sub
is_cvn {
    my $node = shift;
    $$node{'type'} eq 'leaf' 
	&& ($$node{'form'} =~ /\#cvn/
	    || $$node{'userdata'} eq 'CVN');
}
sub
is_hacked_cvn {
    my $node = shift;
    $$node{'type'} eq 'leaf' && $$node{'pos'} =~ /\#CVN/;
}
sub
is_dim {
    my $node = shift;
    return 0 unless $$node{'type'};
    if ($$node{'type'} eq 'phrase') {
	return $$node{'label'} && $$node{'label'} =~ /NP-(?:ERG|ABS|ABL|COM|LOC|TER|LT|DAT|EQU|VOC)/;
    } elsif ($$node{'type'} eq 'leaf') {
        return $$node{'pos'} =~ /^ERG|ABS|ABL|COM|LOC|TER|LT|DAT|EQU|VOC$/;
    } else {
	return 0;
    }
}
sub
is_v {
    my $node = shift;
    $$node{'type'} eq 'phrase' && ($$node{'label'} =~ /^VC/);
}

sub
is_v_finite {
    my $node = shift;
    $$node{'type'} && $$node{'type'} eq 'phrase' && ($$node{'label'} eq 'VC-F');
}

sub
is_v_nonfinite {
    my $node = shift;
    ($$node{'type'} eq 'phrase' 
     && ($$node{'label'} eq 'VNF' || $$node{'label'} eq 'VC-N'))
	|| ($$node{'type'} eq 'leaf' && $$node{'pos'} eq 'VC-N');
}

sub
is_vnf {
    my $node = shift;
    $$node{'type'} eq 'phrase' && ($$node{'label'} eq 'VC-N');
}

sub
is_vnf_marked {
    my $node = shift;
    if ($$node{'type'} eq 'phrase' && ($$node{'label'} eq 'VC-N')) {
	my $i;
	for ($i = $#{$$node{'children'}}; $i > 0 ; --$i) {
	    my $n = ${$$node{'children'}}[$i];
	    return 1 if is_nom($n) || is_de($n) || is_vsf_e($n);
	}
    }
    0;
}

sub
is_x {
    my $node = shift;
    $$node{'type'} eq 'leaf' && $$node{'pos'} eq 'X';
}
sub
is_cop {
    my $node = shift;
    $$node{'type'} && $$node{'type'} eq 'leaf' && $$node{'pos'} eq 'COP';
}
sub
is_vsf_e {
    my $node = shift;
    $$node{'type'} eq 'leaf' && $$node{'pos'} eq 'E';
}
sub
is_clause_begin {
    my $node = shift;
    $$node{'type'} eq 'leaf' && $$node{'pos'} eq '#INS';
}
sub
is_clause_end {
    my $node = shift;
    my $last_child = ${$$node{'children'}}[$#{$$node{'children'}}];
    is_de($last_child);
}
sub
is_de {
    my $node = shift;
    $$node{'type'} eq 'leaf' && $$node{'pos'} eq 'DE';
}
sub
is_nom {
    my $node = shift;
    $$node{'type'} eq 'leaf' && $$node{'pos'} eq 'NOM';
#    if ($$node{'type'} eq 'phrase') {
#	my @children = @{$$node{'children'}};
#	is_nom_leaf($children[$#children]);
#    }
}

sub
is_nom_leaf {
    my $node = shift;
    $$node{'type'} eq 'leaf' && $$node{'pos'} eq 'NOM';
}
sub
is_poss {
    my $node = shift;
    $$node{'type'} && $$node{'type'} eq 'leaf' && $$node{'pos'} eq 'POS';
}

sub
is_plur {
    my $node = shift;
    $$node{'type'} eq 'leaf' && $$node{'pos'} eq 'PL';
}

sub
is_gen {
    my $node = shift;
    $$node{'type'} eq 'leaf' && $$node{'pos'} eq 'GEN';
}

sub
is_P2 {
    my $node = shift;
    $$node{'type'} eq 'leaf' && $$node{'pos'} eq 'P2';
}

sub
is_np {
    my $node = shift;
    $$node{'type'} eq 'phrase' && $$node{'label'} =~ /^NP/;
}

sub
is_np_gen {
    my $node = shift;
    $$node{'type'} && $$node{'type'} eq 'phrase' && $$node{'label'} eq 'NP-GEN';
}

sub
is_lfd {
    my $node = shift;
    $$node{'type'} && $$node{'type'} eq 'phrase' && $$node{'label'} =~ /LFD/;
}

sub
is_np_mod {
    my $node = shift;
    $$node{'type'} && $$node{'type'} eq 'phrase' && $$node{'label'} =~ /^NP-MOD/;
}

sub
is_np_pos {
    my $node = shift;
    $$node{'type'} && $$node{'type'} eq 'phrase' && $$node{'label'} =~ /^NP.*?-POS/;
}

sub
is_np_prn {
    my $node = shift;
    $$node{'type'} eq 'phrase' && $$node{'label'} =~ /^NP-PRN/;
}

sub
is_n {
    my $node = shift;
    ($$node{'type'} eq 'phrase' && $$node{'label'} =~ /^N[CP]/)
	|| ($$node{'type'} eq 'leaf' && $$node{'pos'} =~ /^[A-Z]?N$/);
}

sub
is_person {
    my $node = shift;
    my $head = head($node);
    my $pos = $$head{'pos'} || '';
    ($pos eq 'PN' || $pos eq 'RN' || $pos eq 'DN');
}

sub
is_n_or_s {
    my $node = shift;
    is_n($node) || ($$node{'type'} ne 'leaf' && $$node{'label'} =~ /^S-?/);
}

sub
is_s {
    my $node = shift;
    if ($$node{'type'} ne 'leaf' && $$node{'label'} =~ /^S-?/) {
	return $$node{'label'};
    } else {
	return undef;
    }
}

# CAUTION: this routine assumes V is a bare root--this means that caller has to
# know what is before/after V to ensure it's not a more complex form
sub
is_adj {
    my ($node,$next) = @_;
    ($$node{'type'} eq 'phrase' && $$node{'label'} eq 'MODP')
	|| ($$node{'type'} eq 'leaf' && $$node{'pos'} eq 'A')
	|| ((is_vnf($node) 
	     || ($$node{'type'} eq 'leaf' && $$node{'pos'} =~ /^V/))
	    && (ORACC::SSA3::Intrans::is_intrans(Vparse($node))
		|| ($next && is_gen($next))));
}
# CAUTION: this routine assumes V is an ADJ--this means that caller has to
# know what is before/after V to ensure it's not a more complex form
sub
is_adja {
    my ($node,$nom) = @_;
    is_adj($node) && is_nom($nom);
}

# CHECKME: what does kug inanak generate?
# this test is used in detecting premods for determining nncnj sequences
sub
is_modp {
    my $node = shift;
    (defined($node) && $$node{'type'}
     && (($$node{'type'} eq 'phrase' && $$node{'label'} eq 'MODP')
	 || ($$node{'type'} eq 'leaf' && $$node{'pos'} eq 'ADJ')));
}

sub
is_rel {
    my $node = shift;
    $$node{'type'} && $$node{'type'} eq 'clause' && $$node{'label'} =~ /^S-REL/;
}
sub
is_rrc {
    my $node = shift;
    ($$node{'type'} eq 'phrase' && $$node{'label'} eq 'RRC');
}

sub
is_rel_pron {
    my $node = shift;
    ($$node{'type'} eq 'leaf' && exists($pron_forms{$$node{'form'}}));
}

sub
is_erg_pron {
    my $node = shift;
    ($$node{'type'} eq 'leaf' && exists($erg_forms{$$node{'form'}}));
}

sub
is_finite {
    my $parse = shift;
    my @vpr = grep(defined, @{$$parse{'vpr'}[1]});
    $#vpr >= 0;
}

###DEPRECATED: FIX CODE TO USE is_animate_node INSTEAD
sub
is_animate {
    my $cfgw = shift;
    exists($anim{$cfgw});
}

sub
is_animate_node {
    my $node = shift;
    if (is_persname($node)) {
	return 1;
    } else {
	my $cfgw = head_cfgw($node);
	return exists($prof{$cfgw}) || exists($anim{$cfgw});
    }
}

sub
is_lt {
    my $n = shift;
    $$n{'label'} && $$n{'label'} eq 'NP-LT';
}

sub
is_trace {
    my $n = shift;
    if ($#{$$n{'children'}} == 0) {
	my $c = ${$$n{'children'}}[0];
	if ($$c{'type'} eq 'leaf' && $$c{'pos'} =~ /\*T\*/) {
	    return 1;
	}
    }
    0;
}

1;
