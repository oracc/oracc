package ORACC::CBD::Compare;
require Exporter;
@ISA=qw/Exporter/;

@EXPORT = qw/compare/;

use warnings; use strict; use open 'utf8'; use utf8;

use Data::Dumper;

use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;
use ORACC::CBD::Hash;
use String::Similarity;
use String::Similarity::Group qw/groups groups_hard sort_by_similarity/;

my $base = undef;
my $incoming = undef;

my %b_bases = ();
my %i_bases = ();
my %b_entries = ();
my %i_entries = ();
my %b_matched = ();
my %b_nomatch = ();
my %i_nomatch = ();

my %base_maybes = ();
my %maybes = ();

sub compare {
    my ($args) = @_;
    my $bas_name = ${$ORACC::CBD::data{'cbds'}}[0];
    my $inc_name = ${$ORACC::CBD::data{'cbds'}}[1];

    warn "comparing incoming $inc_name against base $bas_name\n";
    
    $base = \%{$ORACC::CBD::data{$bas_name}};
    $incoming = \%{$ORACC::CBD::data{$inc_name}};

    %b_entries = entries_of($base);
    %i_entries = entries_of($incoming);

    %b_bases = index_bases($base);
    %i_bases = index_bases($incoming);

#    print Dumper \%b_entries;
    
    cmp_entries();
    set_b_nomatch();
    
    sim_entries();

    unset_i_nomatch_maybes(%maybes);

    try_nomatch_by_base();

    unset_i_nomatch_maybes(%base_maybes);

    foreach my $ok (sort keys %b_matched) {
#	warn "incoming $ok is in base\n";
    }

    pp_file($$incoming{'file'});
    
    foreach my $m (sort keys %maybes) {
	my $mref = $maybes{$m};
	my ($maybe,$base,$sim) = @{$mref};
	$base = '' unless $base;
	$sim = sprintf("%.2f",$sim);
	my $l = line_of($incoming, $m, 'entry');
	pp_line($l);
	if (defined($m) && defined($maybe)) {
	    pp_warn("incoming $base='$m' may be [$sim] '$maybe' in base");
	} else {
	    $m = '' unless $m;
	    $maybe = '' unless $m;
	    pp_warn("undefined element in m=$m, maybe=$maybe");
	}
    }

    foreach my $m (sort keys %base_maybes) {
	my $mref = $base_maybes{$m};
	my ($maybe,$base,$sim) = @{$mref};
	$sim = sprintf("%.2f",$sim);
	my $l = line_of($incoming, $m, 'entry');
	pp_line($l);
	#	pp_warn("incoming $base='$m' homograph [$sim] of '$maybe' in base");
	pp_warn("incoming '$m' not in base glossary ($base = homograph [$sim] of '$maybe'?)");
    }

    foreach my $n (sort keys %i_nomatch) {
	my $l = line_of($incoming, $n, 'entry');
	pp_line($l);
	pp_warn("incoming '$n' not in base");
    }
}

sub line_of {
    my($where,$what,$tag) = @_;
    my %e = %{$$where{'entries'}};
    my $id = $e{$what};
    my $l = ${$e{$id,'l'}}{$tag};
    $l + 1;
}

sub cmp_entries {
    foreach my $e (keys %i_entries) {
	if ($b_entries{$e}) {
	    ++$b_matched{$e}
#	    warn "\@entry $e is in base as $b_entries{$e}\n";
	} else {
#	    warn "\@entry $e not in base\n";
	    ++$i_nomatch{$e};
	}
    }
}

sub sim_entries {
    my @try = keys %b_nomatch;
    foreach my $i (keys %i_nomatch) {
	my @maybe = sort_by_similarity(\@try,$i,0.9);
	if ($#maybe >= 0) {
	    foreach my $m (@maybe) {
		$maybes{$i} = [ $maybe[0], $b, 0.9 ];
		last;
	    }
	}
    }
    foreach my $i (keys %i_nomatch) {
	my @maybe = sort_by_similarity(\@try,$i,0.7);
	if ($#maybe >= 0) {
	    foreach my $m (@maybe) {
		$b = base_check($m, $i);
		if ($b) {
		    $maybes{$i} = [ $maybe[0], $b, 0.7];
		    last;
		    # warn "incoming '$i' could be '$maybe[0]' in base\n";
		} else {
		    # warn "rejecting incoming '$i' as maybe for '$maybe[0]' in base\n";
		}
	    }
	}
    }
}

sub try_nomatch_by_base {
    my @try = keys %b_nomatch;
    foreach my $i (keys %i_nomatch) {
	foreach my $t (@try) {
	    $b = base_check($t, $i);
	    if ($b) {
		my $sim = sim_cfgw($t,$i);
#		warn "sim $t ~ $i = $sim\n";
		if ($sim > 0.5) {
		    $base_maybes{$i} = [ $t, $b, $sim ];
		    last;
		}
		# warn "incoming '$i' could be '$maybe[0]' in base\n";
	    } else {
		# warn "rejecting incoming '$i' as maybe for '$maybe[0]' in base\n";
	    }
	}
    }
}

sub sim_cfgw {
    my ($t,$i) = @_;
    return unless $t && $i;
    my ($t_cf,$t_gw) = ($t =~ /^(.*?)\s+\[(.*?)\]/);
    my ($i_cf,$i_gw) = ($i =~ /^(.*?)\s+\[(.*?)\]/);
    unless ($t_cf && $t_gw) {
	pp_warn("syntax error in arg $t of sim_cfgw");
	return 0;
    }
    unless ($i_cf && $i_gw) {
	pp_warn("syntax error in arg $i of sim_cfgw");
	return 0;
    }
    my $cf_sim = similarity($t_cf,$i_cf);
    my $gw_sim = similarity($t_gw,$i_gw);
    return ($cf_sim > $gw_sim) ? $cf_sim : $gw_sim;
}

sub base_check {
    my($b_entry,$i_entry) = @_;
    my $b_id = $b_entries{$b_entry};
    my $i_id = $i_entries{$i_entry};
    foreach my $ibase (keys %{${i_bases{$i_id}}}) {
	if (${$b_bases{$b_id}}{$ibase}) {
	    my $t = ${$b_bases{$b_id}{$ibase}};
#	    warn "found base $ibase=$t in $b_entry and $i_entry\n";
	    return $t;
	}
    }
    '';
}

sub entries_of {
    my $h = shift;
    my $ids = $$h{'ids'};
    my %e = ();
    foreach my $id (@$ids) {
	$e{${$$h{'entries'}}{$id}} = $id;
    }
    %e;
}

sub index_bases {
    my $h = shift;
    my %b = %{$$h{'basedata'}};
    my %i = ();
    foreach my $b (keys %b) {
	my @idtr = @{$b{$b}};
	foreach my $idtr (@idtr) {
	    my($id,$tr) = ($$idtr[0],$$idtr[1]);
#	    warn "register $id => $tr\n";
	    ${${$i{$id}}{$b}} = $tr;
	}
    }
#    print Dumper \%i;
    %i;
}

sub set_b_nomatch {
    foreach my $e (keys %b_entries) {
	++$b_nomatch{$e} unless $b_matched{$e};
    }
#    print Dumper \%b_nomatch;
}

sub unset_i_nomatch_maybes {
    my %m = @_;
    foreach my $m (keys %m) {
	delete $i_nomatch{$m} if $i_nomatch{$m};
    }
}

1;
