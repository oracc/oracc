package ORACC::SSA3::CoreArgs;
use ORACC::SSA3::Phrasal;
use open ':utf8';
use utf8;
use warnings; use strict;

binmode STDIN, ':utf8';
binmode STDERR, ':utf8';

my $loaded = 0;
my $logging = 1;
my %data = ();

my @augment_order = qw/ERG DAT COM ABL LOC TER LT ABS/;

my %augment_index = ();
for (my $i = 0; $i <= $#augment_order; ++$i) {
    $augment_index{$augment_order[$i]} = $i;
}

# Caller passes an S-node (which may be a REL or SUB, this
# routine doesn't care).  We scan for the V; if it matches
# we check that the preceding N matches if the noun field
# is non-empty.  If so, we look at the args to see if the
# minimum required core args are present; any that are found
# to be missing are added in a conventional sequence.
#
# The format of coreargs.txt is:
#
#  ERG LT|DAT ki aj[to love]
#
# Meaning that aj[to love] when it has an ABS ki has a core
# ERG and either an LT or a DAT.  If no member of a
# choice sequence is present, we look at the verb prefix; if any
# member of the choice sequence is present in VPR, we supply
# a corresponding empty category.  If no member of the choice 
# sequence is present in the VPR, we supply an instance of the first
# in the sequence.
#
# If a core arg is bracketed with hyphens, e.g., -DAT-, then if it is
# absent from the sentence the VPR is checked; if it is present in the
# VPR we supply it in the sentence.
#
my %seen = ();
sub
add_core_args {
    load_core_args() unless $loaded;
    my $s = shift;
    return if $seen{$s}++;
#    warn "add_core_args: $s\n";
    if ($$s{'label'} && ($$s{'label'} eq 'S' || $$s{'label'} =~ /^S-/)) {
	augment_node($s);
    }
    my @c = ();
    for (my $i = 0; $i <= $#{$$s{'children'}}; ++$i) {
#	${$$s{'children'}}[$i] = add_core_args(${$$s{'children'}}[$i]);
	push @c, add_core_args(${$$s{'children'}}[$i]);
    }
    @{$$s{'children'}} = @c;
    $s;
}

sub
augment_node {
    my $s = shift;
    foreach my $n (@{$$s{'children'}}) {
	if (ORACC::SSA3::Is::is_v_finite($n)) {
	    my $v = verb_of($n);
#	    print CLOG "processing verb `$v'\n" if $logging;
	    if (!$data{$v}) {
		$v =~ s/\\t\]/]/ unless $data{$v};
		if (!$data{$v}) {
		    $v =~ tr/ /_/;
		}
	    }
	    my %dim;
	    @dim{ dim_infixes($n) } = ();
	    if (defined $data{$v}) {
		# FIXME: add treatment of $data{'noun'}		
#		print CLOG "augmenting sentence with verb '$v'\n" if $logging;
		my %core = %{$data{$v}};
		my %expected_args = ();
		foreach my $a (@{$core{'args'}}) {
		    if ($a =~ /\|/) {
			foreach my $aa (split(/\|/,$a)) {
			    $expected_args{$aa} = $a;
			}
		    } else {
			$expected_args{$a} = $a;
		    }
		}
		$expected_args{'ABS'} = 'ABS' unless $expected_args{'ABS'};
		if (exists $dim{'DAT'}) {
		    $expected_args{'DAT'} = 'DAT' unless $expected_args{'DAT'};
		}
		my $aug = join(', ', keys %expected_args);
#		print CLOG "CoreArgs: expecting = $aug\n" if $logging && length $aug;
		# FIXME: add treatment of $core{'dims'}
		foreach my $n (@{$$s{'children'}}) {
		    if ($$n{'label'} && $$n{'label'} =~ /^NP-/) {
			my $case = case_of($n);
			if ($expected_args{$case}) {
			    if ($expected_args{$case} =~ /\|/) {
				foreach my $c (split(/\|/, $expected_args{$case})) {
				    delete $expected_args{$c};
				}
			    } else {
				delete $expected_args{$case};
			    }
			}
		    }
		}
		$aug = join(', ', keys %expected_args);
#		print CLOG "CoreArgs: to add = $aug\n" if $logging && length $aug;
		my @newc = ();
		my @oldc = @{$$s{'children'}};
		if (ORACC::SSA3::Is::is_trace($oldc[0])) {
		    push @newc, shift @oldc;
		} elsif (!exp_case($oldc[0],$core{'nouncases'})
			 && ORACC::SSA3::Is::is_lt($oldc[0]) 
			 && ORACC::SSA3::Is::is_animate($oldc[0])
			 && defined($expected_args{'ERG'})) {
		    my $erg = shift @oldc;
		    $$erg{'label'} =~ s/-LT/-ERG/;
		    for (my $i = 0; $i <= $#{$$erg{'children'}}; ++$i) {
			my $elt = ${$$erg{'children'}}[$i];
			if ($$elt{'type'} eq 'leaf' && $$elt{'pos'}  eq 'LT') {
			    ${${$$erg{'children'}}[$i]}{'pos'} = 'ERG';
			    last;
			}
		    }
		    delete $expected_args{'ERG'};
#		    print CLOG "CoreArgs: coercing LT to ERG\n" if $logging;
		    push @newc, $erg;
		}
		my @exp = sort { $augment_index{$a} <=> $augment_index{$b} } keys %expected_args;
		while ($#oldc >= 0 || $#exp >= 0) {
		    my $oc_case = case_of($oldc[0]);
		    if (ORACC::SSA3::Is::is_conjp($oldc[0]) || ($oc_case && length $oc_case)) {
			warn("CoreArgs: oldc case '$oc_case' not in augment_index\n")
			    unless !$oc_case || defined $augment_index{$oc_case};
			warn("CoreArgs: exp case '$exp[0]' not in augment_index\n")
			    unless $#exp < 0 || defined $augment_index{$exp[0]};
			if ($#exp < 0 
			    || !$oc_case 
			    || ($augment_index{$oc_case} <= $augment_index{$exp[0]})) {
			    push @newc, shift @oldc;
			} else {
#			    warn("coreargs+\n");
			    my $e = shift @exp;
			    push @newc, 
			    ORACC::OSS2::Node::make_node
				('phrase', 
				 [ ORACC::SSA3::MakeLeaves::make_leaf(undef,'0','#N','*',undef,1,undef,undef,'CoreArgs',148) ], 
				 "NP-$e",'CoreArgs',149);
			}
		    } else {
			# flush all the expected items at the first non-case NP
			# FIXME: what about sentence-initial pronouns and function words?
			#        should really flush at verb
			foreach my $e (@exp) {
#			    warn("coreargs++\n");
			    push @newc, 
			    ORACC::OSS2::Node::make_node
				('phrase', 
				 [ ORACC::SSA3::MakeLeaves::make_leaf(undef,'0','#N','*',undef,1,undef,undef,'CoreArgs',160) ], 
				 "NP-$e",'CoreArgs',161); ### changed *' to * here
			    ### also fixed this leaf to match the one above
			    delete $expected_args{$e};
			}
			last;
		    }
		}
		push @newc, @oldc;

		$$s{'children'} = [ @newc ];
		last;
	    } else {
		print STDERR "CoreArgs: no core arg data for verb '$v'\n" unless $v =~ /^V/;
		last;
	    }
	}
    }
    $s;
}

sub
exp_case {
    my($n,$expref) = @_;
    my $cf = cf_of($n);
    if ($$expref{$cf}) {
	my $case = case_of($n);
	return $$expref{$cf} =~ /$case/;
    }
    0;
}

sub
cf_of {
    my $n = shift;
    my $cfgw = ORACC::SSA3::Phrasal::head_cfgw($n);
    if ($cfgw) {
	$cfgw =~ s/\[.*$//;
    }
    $cfgw;
}

sub
case_of {
    my $node = shift;
    if ($$node{'type'} && $$node{'type'} eq 'phrase') {
#	print STDERR "case_of: node{label} = $$node{'label'}\n";
	$$node{'label'} =~ /-(ERG|ABS|ABL|COM|LOC|TER|LT|DAT)/ && return $1;
    }
    '';
}


sub
verb_of {
    my $vc = shift;
    foreach my $n (@{$$vc{'children'}}) {
	if ($$n{'dsu'}) {
	    my $ret = $$n{'dsu'};
	    $ret =~ s/\].*$/]/;
	    $ret = ORACC::SSA3::MakeLeaves::edit_sem($ret);
	    return $ret;
	} elsif ($$n{'pos'} =~ /^V/) {
	    my $ret = $$n{'form'};
	    $ret =~ s/\#[Rr]+$//;
#	    use Data::Dumper; print STDERR Dumper($n);
	    $ret =~ s/\]/\\t]/ if ${$$n{'parse'}}{'pos'} =~ /t$/;
	    return $ret;
	}
    }
}

sub
dim_infixes {
    my $n = shift;
    my @c = @{$$n{'children'}};
    my @dim = ();
    foreach my $c (@c) {
	push(@dim, $$c{'pos'}) if $$c{'pos'} =~ /^ABL|COM|DAT|LOC|LT$/;
    }
    @dim;
}

sub
load_core_args {
    $loaded = 1;
    open(IN,'@@ORACC@@/lib/ORACC/SSA3/data/coreargs.txt') 
	|| die("CoreArgs: can't find coreargs.txt");
    while (<IN>) {
	next if /^\s*\#/ || /^\s*$/;
	s/\s*$//;
	my @data = split;
	my @args = ();
	my @dims = ();
	my $verb = '';
	my @nouns = ();
	my %nouncases = ();

	for (my $i = 0; $i <= $#data; ++$i) {
	    if ($data[$i] =~ /^[A-Z]+$/) {
		push @args, $data[$i];
	    } elsif ($data[$i] =~ /^[A-Z\|]+$/) {
		push @args, $data[$i];
	    } elsif ($data[$i] =~ /^-[A-Z]+-$/) {
		$data[$i] =~ s/^-//;
		$data[$i] =~ s/-$//;
		push @dims, $data[$i];
	    } elsif ($data[$i] =~ /\[/) {
		$verb = join(' ', @data[$i..$#data]);
		$verb =~ s/\**$//;
#		$verb =~ s/ŋ/j/g;
#		$verb =~ s/š/c/g;
		last;
	    } else {
		my $noun = $data[$i];
#		$noun =~ s/ŋ/j/g;
#		$noun =~ s/š/c/g;
		if ($noun =~ s/\.([A-Z].*?)$//) {
		    $nouncases{$noun} = $1;
		}
		push @nouns, $noun;
	    }
	}

	$verb = ORACC::SSA3::MakeLeaves::edit_sem($verb);
	$verb = join(' ',@nouns).' '.$verb if $#nouns >= 0;
	my $nn = join('_',@nouns);
#	print CLOG "loading $verb (noun=$nn)\n" if $logloading;
	$data{$verb} = {
	    args=>[ @args ],
	    nouns=>[ @nouns],
	    nouncases=>\%nouncases,
	    dims=>[ @dims ]
	};
    }
    close(IN);

    if ($logging) {
#	open CLOG, ">01tmp/coreargs.log";
#	use Data::Dumper;
#	print CLOG Dumper \%data;
    }
}

1;
