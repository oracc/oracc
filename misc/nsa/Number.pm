package ORACC::ATF::Number;
use warnings; use strict;
use ORACC::ATF::Parser;
use ORACC::ATF::NumberData;

my @tok_refs = ();

my %met_system = ();
my %num_system = ();
my %any_system = ();
my %mark_for_delete = ();
my $n_sys_poss;

my %sys_cue = (
    sig4  => 'brick-met',
    sahar => 'volume-met'
);

my $num_debug = 0;

ORACC::ATF::NumberData::dump_data() if $num_debug;

sub
find_systems {
    my $index = shift;
    my $offset = $index;
    my $ret_index;
    
    @tok_refs = ();
    my @toks2 = ();
    my @toks3 = ();
    my @toks4 = ();

    # collect all the numeric tokens
    while (1) {
	my $tok_ref = ORACC::ATF::Parser::get_line_tok($index++);
	last unless defined($tok_ref) && $$tok_ref{'type'} eq 'number';
	push @tok_refs, $tok_ref;
    }

    return $index unless $#tok_refs >= 0; # this shouldn't happen

    $ret_index = $index;

    show_toks(@tok_refs) if $num_debug;

    # rewrite list to be a sequence of numbers and non-numbers
    # numbers consume successive elements of diminishing size
    # but numbers of the same size create separate nodes in this
    # list
    @toks2 = num_non_num(@tok_refs);
    show_toks2(@toks2) if $num_debug;

    # merge count+unit pairs
    @toks3 = merge_num_unit(@toks2);
    show_toks3(@toks3) if $num_debug;

    # merge sequences that share a common system
    @toks4 = merge_systems(@toks3);
    show_toks4(@toks4) if $num_debug;

    # if the initial system is ambiguous, try to disambiguate using preceding word
#FIXME: 63, 75, 93, 97
    if (defined ${$toks4[0]}{'system'}) {
	if (${$toks4[0]}{'system'} =~ /\|/) {
	    my $prev_w = ORACC::ATF::Parser::get_prev_word($offset);
	    print STDERR "Number.pm: prev_w = $prev_w\n" if $num_debug;
	    if (length $prev_w) {
		$prev_w =~ s/-bi$//;
		if (defined $sys_cue{$prev_w}) {
		    ${$toks4[0]}{'system'} = $sys_cue{$prev_w};
		}
	    }
	}

	# if the final system is ambiguous, try to disambiguate using following word
	if (${$toks4[$#toks4]}{'system'} =~ /\|/) {
	    my $next_w = ORACC::ATF::Parser::get_next_word($index-1);
	    print STDERR "Number.pm: next_w = $next_w\n" if $num_debug;
	    if ($next_w) {
		$next_w =~ s/-bi$//;
		if ($sys_cue{$next_w}) {
		    ${$toks4[0]}{'system'} = $sys_cue{$next_w};
		}
	    }
	}
	
	my @q = @toks4;
	
	# Now we have a list of quantities in either determinate or
	# undeterminable systems.  We can poke the system values back
	# into the initial token of each quantity in the parser's token
	# array.
	foreach my $q (@q) {
	    if ($$q{'system'} 
		=~ /^sexagesimal-num-(?:asz|disz)-\*-\*\|sexagesimal-num-(?:asz|disz)-\*-\*$/) {
		$$q{'system'} = 'sexagesimal-num';
	    }
	    ORACC::ATF::Parser::put_n_system($offset+$$q{'index'}, $$q{'system'});
	    
	    # FIXME: adjust boundaries which precede system-initial 
	    # numbers so they are not numeric
	    # FIXME second-guessing: is that really necessary?
	}
    }

    # return the index of the line number token which ended the number sequence
    return $ret_index;
}

sub
get_unit {
    my $tok_ref = shift;
    my $paren;
    my $u = $$tok_ref{'unit'} || $$tok_ref{'tok'};
    if ($u =~ /^.*?\((.*?)\)$/) {
	$u = $1;
	$paren = 1;
    } else {
	$paren = 0;
    }
    ($u,$paren);
}

sub
init_system {
    my $u = shift;
    undef %any_system;
    foreach my $ns (ORACC::ATF::NumberData::get_num_poss($u), ORACC::ATF::NumberData::get_met_poss($u)) {
	my($sys,$n) = ($ns =~ /^(.*)\/(.*)$/);
	$any_system{$sys} = $n;
    }
}

sub
common_system {
    my $u = shift;
    my %u_sys = ();
    my $n_sys_poss = keys %any_system;

    # FIXME: get_num_poss should just return a hash for u_sys
    # to use directly
    foreach my $us (ORACC::ATF::NumberData::get_num_poss($u), ORACC::ATF::NumberData::get_met_poss($u)) {
	my($sys,$n) = ($us =~ /^(.*)\/(.*)$/);
	$u_sys{$sys} = $n;
    }
    foreach my $ns (keys %any_system) {
	if (defined $u_sys{$ns}
	    && (is_sds($u) 
		|| $u_sys{$ns} <= $any_system{$ns}
		|| ($u_sys{$ns} == -1 || $any_system{$ns} == -1))) {
	    $any_system{$ns} = $u_sys{$ns};
	} else {
	    ++$mark_for_delete{$ns};
	    --$n_sys_poss;
	}
    }
    $n_sys_poss;
}

sub
get_common_system {
    my $ret = join('|', sort keys %any_system);
    if ($ret eq 'sexagesimal-num-asz|sexagesimal-num-disz') {
	$ret = 'sexagesimal-num-disz';
    }
    undef %any_system;
    $ret;
}

sub
delete_marked_any {
    foreach my $k (%mark_for_delete) {
	delete $any_system{$k};
    }
    undef %mark_for_delete;
}

sub
init_num_system {
    my $u = shift;
    undef %num_system;
    foreach my $ns (ORACC::ATF::NumberData::get_num_poss($u)) {
	my($sys,$n) = ($ns =~ /^(.*)\/(.*)$/);
	$num_system{$sys} = $n;
    }
}

sub
common_num_system {
    my $u = shift;
    my %u_sys = ();
    my $n_sys_poss = keys %num_system;

    # FIXME: get_num_poss should just return a hash for u_sys
    # to use directly
    foreach my $us (ORACC::ATF::NumberData::get_num_poss($u)) {
	my($sys,$n) = ($us =~ /^(.*)\/(.*)$/);
	$u_sys{$sys} = $n;
    }
    foreach my $ns (keys %num_system) {
	if (defined $u_sys{$ns}
	    && ($u_sys{$ns} < $num_system{$ns}
		|| $u_sys{$ns} == -1 || $num_system{$ns} == -1)) {
	    $num_system{$ns} = $u_sys{$ns};
	} else {
	    ++$mark_for_delete{$ns};
	    --$n_sys_poss;
	}
    }
    $n_sys_poss;
}

sub
delete_marked_num {
    foreach my $k (%mark_for_delete) {
	delete $num_system{$k};
    }
    undef %mark_for_delete;
}

sub
get_common_num_system {
    my $ret = join('|', sort keys %num_system);
    if ($ret eq 'sexagesimal-num-asz|sexagesimal-num-disz') {
	$ret = 'sexagesimal-num-disz';
    }
    undef %num_system;
    $ret;
}

sub
init_met_system {
    my $u = shift;
    unless (%met_system) {
	$n_sys_poss = 0;
	foreach my $ns (ORACC::ATF::NumberData::get_met_poss($u)) {
	    my($sys,$n) = ($ns =~ /^(.*)\/(.*)$/);
	    $met_system{$sys} = $n;
	    ++$n_sys_poss;
	}
    }
}

sub
common_met_system {
    my $u = shift;
    my %u_sys = ();
    my $n_sys_poss = keys %met_system;

    my %u = ();
    #
    # FIXME: get_met_poss should just return a hash for u_sys
    # to use directly
    foreach my $us (ORACC::ATF::NumberData::get_met_poss($u)) {
	my($sys,$n) = ($us =~ /^(.*)\/(.*)$/);
	$u_sys{$sys} = $n;
    }
    foreach my $ns (keys %met_system) {
	# FIXME?: the <= here should probably be == if we have a frac
	# but at present getting the token for the unit and thus the value
	# is a pain; it is possible that this test is adequate for the range
	# of occurrences which are actually found in the corpus
	if (defined $u_sys{$ns}
	    && (is_sds($u)
		|| ($u_sys{$ns} == -1 || $met_system{$ns} == -1)
	        || ($u_sys{$ns} <= $met_system{$ns}))) {
	    $met_system{$ns} = $u_sys{$ns};
	} else {
	    ++$mark_for_delete{$ns};
	    --$n_sys_poss;
	}
    }
    $n_sys_poss;
}

sub
delete_marked_met {
    foreach my $k (%mark_for_delete) {
	delete $met_system{$k};
    }
    undef %mark_for_delete;
}

sub
get_common_met_system {
    my $ret = join('|', sort keys %met_system);
    undef %met_system;
    $ret;
}

sub
num_non_num {
    my @tok_refs = @_;
    my @toks2 = ();
    my $met_best = 0;

    # rewrite them so that adjacent numeric tokens from the same 
    # number system are single items
    for (my $i = 0; $i <= $#tok_refs; ) {

	my ($init_u,$u_paren) = get_unit($tok_refs[$i]);

	if (is_met($init_u) && ($met_best || !is_num($init_u))) {
	    $met_best = 1;
	    if ($u_paren) {
		push @toks2, { type=>'num',
			       units=>['#'],
			       tok_ref_index=>$i,
			     };
	    }
	    push @toks2, { type=>'unit',
			   units=>[ $init_u ],
			   tok_ref_index=>$i,
			 };
	    ++$i;

	} elsif (is_num($init_u)) {

	    my @u = ($init_u);

	    my %t2 = ();
	    $t2{'tok_ref_index'} = $i;

	    init_num_system($init_u);
	    if ($init_u =~ /^asz|disz$/) {
		$t2{'axis'} = $init_u;
	    } elsif ($init_u eq 'igijal') {
		$i += 2; # skip igi and delimiter
		push @u, 'disz'; # cheap hack; should check this
		++$i;
		if (${$tok_refs[$i]}{'tok'} eq 'gal2') {
		    ++$i;
		}
	    }

	    while (++$i <= $#tok_refs) {
		my ($u,$u_paren) = get_unit($tok_refs[$i]);
		if ($u =~ /^$ORACC::ATF::Parser::delim_pat$/o) {
#		    push @u, $u; # drop punctuation
		} elsif (is_met($u) && ($met_best || !is_num($u))) {
		    last;
		} elsif (is_num($u)) {
		    if ($u =~ /^asz|disz$/) {
			$t2{'axis'} = $u;
		    }
		    if (common_num_system($u)) {
			delete_marked_num();
			push @u, $u;
		    } elsif ($u eq $u[$#u]
			     && ${$tok_refs[$i]}{'tok'} =~ m,/,
		             && ${$tok_refs[$i]}{'tok'} !~ /NINDA2/) {
			push @u, $u;
		    } else {
			# pop off any final delimiter and decrement
			# past it
			if (!is_num($u[$#u])) {
			    pop @u;
			    --$i;
			}
			last;
		    }
		} elsif (${$tok_refs[$i]}{'tok'} eq 'la2') {
		    push @u, 'la2';
		    ++$i;
		} else {
		    # drop '#' and '|' tokens
		}
	    }

	    $t2{'type'} = 'num';
	    $t2{'units'} = \@u;
#	    $t2{'system'} = get_common_num_system();
	    push @toks2, \%t2;

	} elsif (is_sds($init_u)) {

            push @toks2, { type=>'sds', units=>[ $init_u ], system=>get_sds($init_u) };
    
	} else {
	    ++$i;
	}
    }
    return @toks2;
}

sub
merge_num_unit {
    my @in = @_;
    my @ret = ();
    for (my $i = 0; $i <= $#in; ++$i) {
	if (${$in[$i]}{'type'} eq 'num') {
	    if ($i < $#in) {
		if (${$in[$i+1]}{'type'} && ${$in[$i+1]}{'type'} eq 'unit') {
		    push @ret, make_cu_node($in[$i],$in[$i+1]);
		    ++$i;
	        } elsif (unit_final($in[$i])) {
		    push @ret, make_cu_node($in[$i],make_u_ref(trim_final_unit($in[$i]),0));
	        } elsif (${$in[$i]}{'axis'} && ${$in[$i]}{'axis'} eq 'asz') {
		    push @ret, make_cu_node($in[$i],make_u_ref('gur',1));
	        } else {
		    push @ret, make_cu_node($in[$i],undef);
		}
	    } else {
	        if (unit_final($in[$i])) {
		    push @ret, make_cu_node($in[$i],make_u_ref(trim_final_unit($in[$i]),0));
		} else {
		    push @ret, make_cu_node($in[$i],undef);
		}
	    }
	} else {
	    push @ret, make_cu_node(undef,$in[$i]);
	}
    }
    @ret;
}

sub
make_u_ref {
    { type=>'unit', units=>[ $_[0] ], implicit=>$_[1], tok_ref_index=>-1 };
}

sub
unit_final {
    my $nref = shift;
    my @units = @{$$nref{'units'}};
    if ($#units > 0) {
	return is_met($units[$#units]);
    }
    0;
}

sub
trim_final_unit {
    my $nref = shift;
    my @units = @{$$nref{'units'}};
    my $ret = '';
    if ($#units > 0) {
	$ret = pop (@units);
	$$nref{'units'} = [ @units ];
    }
    $ret;
}

sub
make_cu_node {
    my($c_ref,$u_ref) = @_;
    my $axis = '';
    my $unit = '';
    if (defined $c_ref) {
	$axis = $$c_ref{'axis'};
    }
    if (defined $u_ref) {
	$unit = ${$$u_ref{'units'}}[0];
    }

    { type=>'cu',
      axis=>$axis,
      unit=>$unit,
      cref=>$c_ref,
      uref=>$u_ref };

}

sub
merge_systems {
    my @cu = @_;
    my @q = ();
    foreach (my $i = 0; $i <= $#cu; ) {
	# if the current c-u has an undefined unit make it an 
	# independent quantity for now 
	if (length($cu[$i]{'unit'}) && is_num($cu[$i]{'unit'})) {
	    my $init_u = ${$cu[$i]}{'unit'};
	    my @qcu = ($cu[$i]);
	# redundant because this block is only entered if $cu[$i]{'unit'} is non-empty
#	    if (!defined($init_u) || !length($init_u)) {
#		my $cref = ${$cu[0]}{'cref'};
#	        $init_u = ${$$cref{'units'}}[0];
#	    }
	    init_system($init_u);
	    while (++$i <= $#cu) {
	        my $a = ${$cu[$i]}{'axis'};
	        my $u = ${$cu[$i]}{'unit'};
	        if (length($u)) {
    		    last unless common_system($u);
		} else {
    		    last unless common_system($a);
                }
                push @qcu, $cu[$i];
		delete_marked_any();
	    }

	    # stash them as a single quantity
	    push @q, { system=>get_common_system(),
		       index =>get_index($qcu[0]),
		       cu=>\@qcu };
	} elsif (!length($cu[$i]{'unit'})) {
	    # we are dealing with a count 
	    # suck up subsequent units that fit the num (e.g., gin2 and sze)
	    my $init_u = ${$cu[$i]}{'axis'};
	    my @qcu = ($cu[$i]);
	    if (!defined($init_u) || !length($init_u)) {
		my $cref = ${$cu[0]}{'cref'};
	        $init_u = ${$$cref{'units'}}[0];
	    }
	    init_num_system($init_u);
	    while (++$i <= $#cu) {
	        my $a = ${$cu[$i]}{'axis'};
	        my $u = ${$cu[$i]}{'unit'};
	        if (length($u)) {
    		    last unless common_num_system($u);
		} else {
    		    last unless common_num_system($a);
                }
                push @qcu, $cu[$i];
		delete_marked_num();
	    }

	    # stash them as a single quantity
	    push @q, { system=>get_common_num_system(),
		       index =>get_index($qcu[0]),
		       cu=>\@qcu };
	} else {
	    # we are dealing with a count-unit pair
	    my $init_u = ${$cu[$i]}{'unit'};
	    init_met_system($init_u);
	    my @qcu = ($cu[$i]);
	    while (++$i <= $#cu) {
		last unless length ${$cu[$i]}{'unit'};
	        my $u = ${$cu[$i]}{'unit'};
		last unless common_met_system($u);
                push @qcu, $cu[$i];
		delete_marked_met();
	    }

	    # stash them as a single quantity
	    push @q, { system=>get_common_met_system(),
		       index =>get_index($qcu[0]),
		       cu=>\@qcu }
	}
        if ($q[$#q]{'system'} =~ /\|/) {
	    if ($q[$#q]{'system'} =~ /sexagesimal-num-disz/) {
		$q[$#q]{'system'} = 'sexagesimal-num-disz';
	    }
	}
    }
    @q;
}

sub
get_index {
    my $cu = shift;
    my $tref = $$cu{'cref'} || $$cu{'uref'};
    $$tref{'tok_ref_index'};
}

sub
show_toks {
    print STDERR "Number::show_toks(): ";
    foreach my $t (@_) {
	print STDERR $$t{'tok'}, ' ';
    }
    print STDERR "\n";
}

sub
show_toks2 {
    print STDERR "Number::show_toks2():\n";
    foreach my $t (@_) {
	my($type,$axis) = ($$t{'type'},$$t{'axis'});
	$type = '' unless $type;
	$axis = '' unless $axis;
	print STDERR "type = $type; axis = $axis; ";
	print STDERR "[ ";
	foreach my $u (@{$$t{'units'}}) {
	    print STDERR $u, ' ';
	}
	print STDERR "]\n";
    }
    print STDERR "===\n";
}

sub
show_toks3 {
    print STDERR "Number::show_toks3():\n";
    foreach my $t (@_) {
	my($axis,$unit) = ($$t{'axis'},$$t{'unit'});
	$axis = '' unless $axis;
	$unit = '' unless $unit;
	print STDERR "  [ axis = $axis; unit = $unit ]\n";
    }
    print STDERR "\n";
    print STDERR "===\n";
}

sub
show_toks4 {
    print STDERR "Number::show_toks4():\n";
    foreach my $q (@_) {
	print STDERR "  [ system = $$q{'system'}; index = $$q{'index'} ";
	my @cu = @{$$q{'cu'}};
	if ($#cu >= 0) {
	    print STDERR "[ ";
	    foreach my $c (@cu) {
		print STDERR $$c{'unit'}, ' ';
	    }
	    print STDERR "] ";
	}
	print STDERR "]\n";
    }
    print STDERR "===\n";
}

1;
