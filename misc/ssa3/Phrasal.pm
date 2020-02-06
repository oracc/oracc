package ORACC::SSA3::Phrasal;
use warnings; use strict; use open 'utf8'; use utf8;
use lib '@@ORACC@@/lib';
use Data::Dumper;
use ORACC::OSS2::Node;
use ORACC::OSS2::Log;
use ORACC::SSA3::Common;
use ORACC::SSA3::Number;
use ORACC::SSA3::Intrans;
use ORACC::SSA3::NNCnj;
use ORACC::SSA3::NNMod;
use ORACC::SSA3::CoreArgs;
use ORACC::SSA3::MakeLeaves;
use ORACC::SSA3::Is;

use Clone::PP qw/clone/;

my %known_prn = (
    'e[house]' => 1,
    'gu[neck]' => 1,
    );

sub make_leaf ($$$$$$$$$$);
sub make_node ($$$$$);

my $error_id = '';
my $error_label = '';

my $n_sentence_exit = 0;

# Phrasal Parsing Layer
# =====================
#
# We get passed a list of parsed-word structures as created by the RefMaker; the
# normal case is that this represents a sentence terminated by a finite verb.
# 
#  (1) Rewrite the sentence as a sequence of leaf nodes, splitting off NSF morphemes
#      as separate leaf nodes
#  (2) Split the sentence into segments which end with a postposition terminating with
#      a VF preceded by optional ADV
#  (3) Rewrite N+ADJ as (NP (N )(ADJ ))
#  (4) Rewrite N|NP PRO$ as (NP ...)
#  (5) Rewrite N|NP GEN as (NP-GEN ...)
#

my $node_index = 0;

my $empty_conj = ORACC::SSA3::MakeLeaves::make_leaf(undef,'CONJ','','*','',0,'DUMMY','','Phrasal',46);
my $empty_node = ORACC::OSS2::Node::make_empty_node();

sub
phrasal_init {
#    ORACC::SSA3::Intrans::load_intrans();
}

sub
phrasal_reinitialize {
    $node_index = 0;
}

sub
phrasal_merges {
    my $sent = shift;
    merge_adj($sent);
    1 while merge_cnj($sent);
    merge_poss($sent);
    merge_plur($sent);
    merge_gen($sent);
    merge_poss($sent);
    fixup_forced_conjp($sent);
    $sent;
}

sub
map_sentence {
    my $sent = shift;
    my @node_map = ();

    ## map phrases and subordinate clauses
    #
    # To do this, we put codes in the slots of a mapping array
    # which correspond to features in the sentence:
    #
    #   S = Sentence first node
    #   P = Postposition node marking end of phrase 
    #   0 = NP identified as ABS
    #   R = Relative clause first node
    #   H = Head of subordinate clause which is not an R
    #   N = Nonfinite verb, i.e., end of subordinate clause
    #   G = unresolved GEN (occurs with double GEN or non-simple GEN)
    my @nodes = @{$$sent{'children'}};
    my $m = undef;
    foreach (my $i = 0; $i <= $#nodes; ++$i) {
	if (is_v_finite($nodes[$i])
	    || (is_v_nonfinite($nodes[$i]) && has_cop($nodes[$i]))) {
	    my $node_map = undef;
	    ($i, $node_map, @nodes) = map_verb_finite(\@node_map, \@nodes, $i);
	    @node_map = @$node_map;
	} elsif (is_vnf($nodes[$i])) {
	    my $node_map = undef;
	    ($i, $node_map, @nodes) = map_verb_nonfinite(\@node_map, \@nodes, $i);
	    @node_map = @$node_map;
	} elsif (is_phrase($nodes[$i])
		 && ${$nodes[$i]}{'label'} 
		 && ($m = map_phrase(${$nodes[$i]}{'label'}))) {
	    if (${$nodes[$i]}{'done'} == 1) {
		$node_map[$i] = '<';
	    } else {
		$node_map[$i] = $m;
	    }
	} else {
	    if (is_pp($nodes[$i])) {
		$node_map[$i] = 'P';
	    } elsif ($i && is_abs($nodes[$i])) {
		if (is_msr_np($nodes[$i-1])) {
		    $node_map[$i] = 'P';
		} elsif (is_nom($nodes[$i-1])) {
		    ;
		}
	    } elsif (is_gen($nodes[$i])) {
		$node_map[$i] = 'G';
	    } elsif (is_clause_begin($nodes[$i])) {
		if (${$nodes[$i]}{'form'} eq 'HR') {
		    $node_map[$i] = 'H';
		} else {
		    $node_map[$i] = '#';
		}
	    } else {
		$node_map[$i] = undef unless $node_map[$i];
	    }
	}
    }

    if (!defined $node_map[0]) {
	if ($$sent{'label'}) {
	    $node_map[0] = map_phrase($$sent{'label'});
	} else {
	    $node_map[0] = map_default($$sent{'type'});
	}
    }
   (\@nodes,\@node_map);
}

sub
map_verb_finite {
    my($node_map, $nodes, $i) = @_;
    my @nodes = @$nodes;
    my $n = $nodes[$i];
    if ($i == $#nodes || !is_nom($nodes[$i+1])) {
	$$node_map[$i] = 'V';
	if ($i > 0 && $$node_map[$i-1] && $$node_map[$i-1] eq 'P' 
	    && no_abs($i, @nodes)) {
	    my $case = case_of($nodes[$i-1]);
	    # FIXME: the idea is good here but the implementation sucks
	    # todo: scan sentence for phrases with inanimate double head
	    # and take the first head as the ABS if there is no other ABS in
	    # the sentence
#		    print STDERR "candidate ABS split\n";
	    # ABS is either implicit or fronted
	    # find the first preceding phrase that has two leading
	    # inanimate nouns, and split off the first of them for the ABS
	    for (my $a = $i-1; $a >= 0; --$a) {
		last if is_person($nodes[$a]);
#			last if is_leaf($nodes[$a]) && is_abs($nodes[$a]);
		if (!$a || ($$node_map[$a-1] && $$node_map[$a-1] eq 'P')) {
		    # we are at the head of a phrase; does it start
		    # with NP NP?
#			    print STDERR "looking for splittable ABS\n";
#			    print STDERR show_nodes('','','',@nodes[$a..$i]);
		    if (is_n($nodes[$a]) && is_abs($nodes[$a])
			&& ($case ne 'ERG' || !is_person($nodes[$a]))) {
			my $double_abs = 0;
			my $b = $a+1;
			while ($b < $i) {
			    last if is_pp($nodes[$b]);
			    if (is_n($nodes[$b]) && is_abs($nodes[$b]) 
				&& !is_person($nodes[$b])) {
				$double_abs = 1;
				last;
			    }
			    ++$b;
			}
			if ($double_abs) {
			    # if so, mark the first one as ABS
#				    print STDERR "found splittable ABS a=$a b=$b i=$i\n";
##			    my @new_node_map = @$node_map;
##			    splice(@new_node_map, $b,0,'0');
##			    $node_map = [ @new_node_map ];
##			    splice(@nodes, $b,0,$empty_node);
##			    ++$i;
			    last;
			}
		    }
		}
	    }
	}
    } else {
	# Determine the start of the subordinate clause and process it as 
	# a sentence in its own right.  
	#
	# Determine the start of a subordinate clause. We look backwards for
	# lu[man], nij[thing], aba[who] or aba[what].  If we don't find
	# any of those, we work back from the verb, taking any preverbal
	# absolute as internal to the SUB, then taking all preceding 
	# case-marked NPs until the next unmarked NP; this is taken as
	# the external head of the R.  If the R has a finite verb, 
	# we make it an RC; if the verb is nonfinite, it's an RRC
	$$node_map[$i] = 'V';
	$$node_map[$i+1] = 'N' if ($i < $#nodes);
	my $found_R = 0;
	for (my $j = $i-1; $j >= 0; --$j) {
	    if ($$node_map[$j] && $$node_map[$j] eq 'V' 
		&& !is_vnf($nodes[$j])) {
		last;
	    }
	    if (is_rel_pron($nodes[$j])) {
		$$node_map[++$j] = 'R';
		$found_R = 1;
#			if (is_erg_pron($nodes[$j])) {
#			    splice(@nodes,$j+1,0,make_leaf(undef,'ERG','','*'));
#			    splice(@node_map,$j+1,0,'P');
#			    ++$i;
#			}
		last;
	    }
	}
	if (!$found_R) {
	    my $found_H = -1;
	    my $abs_count = 0;
	    for (my $j = $i-1; $j >= 0; --$j) {
		last if (defined($$node_map[$j]) 
			 && ($$node_map[$j] eq 'N'
			     || ($$node_map[$j] eq 'P'
				 && $j
				 && defined($$node_map[$j-1]) 
				 && $$node_map[$j-1] eq 'N')));
		if (defined($$node_map[$j])) {
		    if ($$node_map[$j] eq 'P') {
			while ($j && !is_abs($nodes[$j-1])) {
			    --$j;
			}
			--$j;
			if ($j) {
			    # check for PRN in the dimensional
			    if (para_syntax($nodes[$j],'ante','prn')
				|| (is_prof(head($nodes[$j]))
				    && is_proper(head($nodes[$j-1]))
				    && !para_syntax($nodes[$j],'ante','no_prn'))) {
				--$j;
			    }
			}
			# now the loop decrement will take $j past
			# the head of this NP
			$found_H = $j;
		    }
		} elsif (is_abs($$nodes[$j])) {
		    if ($abs_count) {
			my $rc = 'R';
			if (is_abs($nodes[$j]) && is_abs($nodes[$j+1])) {
			    ++$j; # leave first abs as external head of REL
			} elsif (is_animate(head_cfgw($nodes[$j]))) {
			    ++$j;
			} else {
			    $rc = 'H';
			}
			$$node_map[$j] = $rc;
			$found_H = $j;
			last;
		    } else {
			++$abs_count;
		    }
		}
	    }
	    if ($found_H < 0) {
		if ($i > 1  && is_abs($nodes[$i-1]) 
		    && !is_animate(head_cfgw($nodes[$i-1]))) {
		    $$node_map[$i-1] = 'H';
		} else {
		    $$node_map[$i] = 'I';
		}
	    } else {
		$$node_map[$found_H] = 'H';
	    }
	}
#		ORACC::OSS2::Log::log_node_map(@node_map);
    }
    ($i, $node_map, @nodes);
}

sub
map_verb_nonfinite {
    my($node_map, $nodes, $i) = @_;
    my @nodes = @$nodes;
    my $marked = is_vnf_marked($nodes[$i]);
    if (is_clause_end($nodes[$i])) {
	$$node_map[$i] = 'Z';
    } else {
	$$node_map[$i] = 'V';
	my $is_intrans = ORACC::SSA3::Intrans::is_intrans(Vparse($nodes[$i]));
	my $j = $i;
	if ($j) {
	    my $found_cvn = 0;
	    my $found_dim = 0;

	    # is the preceding node a #cvn?
	    if (!$is_intrans
		&& is_abs($nodes[$j-1]) 
		&& !is_animate(head_cfgw($nodes[$j-1]))) {
		--$j;
		if ($marked && is_poss($nodes[$i+1])
		    && (!$j || !is_abs($nodes[$j-1]))) {
		    ++$j;
		    # if this is a singleton animate
		    # ABS before a marked verb and there is
		    # a POSS after the verb, don't include it after all
		} elsif ($marked
			 && is_poss($nodes[$i+1]) 
			 && ($j == 0 || !is_abs($nodes[$j-1]))) {
		    ++$j if is_animate($nodes[$j]);
		} else {
		    $found_cvn = 1;
		}
	    }
	    
	    # is the preceding node -a, -ta, -ece or -da?
	    while ($marked
		   && is_dim($nodes[$j-1]) 
		   && $j > 1 
		   && (is_n($nodes[$j-2]) && !is_np_gen($nodes[$j-2]))
		   || (is_leaf_abs($nodes[$j-2]) 
		       && (!is_gen($nodes[$j-3]) 
			   && !is_np_gen($nodes[$j-3])))) {
		--$j;
		# take the preceding NP along with the dimensional
		if ($j) {
		    --$j;
		    if ($j) {
			# check for PRN in the dimensional
			if (para_syntax($nodes[$j],'ante','prn')
			    || (is_prof(head($nodes[$j]))
				&& is_proper(head($nodes[$j-1]))
				&& !para_syntax($nodes[$j],'ante','no_prn'))) {
			    --$j;
			}
		    }
		}
		$found_dim = 1;
	    }
	}
	if ($j < $i) {
	    # in lu2 e2 du3-a-ka we treat e2 du3 as a compound noun;
	    # with no following GEN we treat as RRC
	    if ($i < $#nodes && !$marked 
		&& 
		(is_gen($nodes[$i+1]) 
		 || is_poss($nodes[$i+1])
		 || is_aux($nodes[$i+1]))) { # (!$nom) {
		$$node_map[$j] = 'D';
	    } else {
		$$node_map[$j] = 'A';
	    }
	} else {
	    if ($is_intrans) {
		$$node_map[$i] = 'M';
	    } else {
		$$node_map[$i] = 'C';
	    }
	}
    }
    ($i, $node_map, @nodes);
}

sub
parse_sentence {
    my ($sent,$map) = @_;

    my $nodes = $$sent{'children'};
    
    if ($ORACC::OSS2::verbose > 1) {
	log_args("===parse_sentence nodes with node_index=$node_index ====\n", Dumper($sent), "=======\n");
    }
    log_node_map("===parse_sentence map with node_index=$node_index ====\n", $nodes, $map);

    #N.B.: '#'-map entries are not handled in these hashes
    my %res_nodes = ('S'=>'S-MAT','R'=>'S-REL','H'=>'S-REL','I'=>'S-REL','P'=>'NP', 
		     'C'=>'RRC',
		     'D'=>'NC','A'=>'RRC','M'=>'MODP','V'=>'S','0'=>'NP','G'=>'NP');
    my %end_nodes = ('S'=>'', 'R'=>'N','H'=>'N','P'=>'P', 'C'=>'','D'=>'V',
		     'A'=>'V', 'M'=>'', 'V'=>'', '0'=>'', 'G'=>'', 'I'=>'N',
	);

    my @sentence_nodes = ();
    my @phrase_nodes = ();
    my @post_phrasal_queue = ();
    my $result_node = undef;
    my $end_node = ''; 

    if ($$map[$node_index] && $$map[$node_index] ne '#') {
	$result_node = $res_nodes{$$map[$node_index]};
	$end_node = $end_nodes{$$map[$node_index]};
    } elsif ($node_index) {
	if ($$map[$node_index-1] eq '#') {
	    $result_node = ${$$nodes[$node_index-1]}{'form'};
	    $end_node = 'Z';
	} else {
#	    warn("parse_sentence: called with null map[node_index]\n");
	    $result_node = 'S-MAT';
	}
    } else {
	$result_node = 'S-MAT';
    }

    my $node_type = ($result_node eq 'MODP' ? 'phrase' : 'clause');

    if (!$result_node) {
	warn("parse_sentence: $$map[$node_index] yields no result type\n");
    } else {
	$$sent{'label'} = $result_node unless $result_node eq 'S-MAT';
    }

    my $i = $node_index;
    for (; $i <= $#$nodes; ++$i) {
	if ($$map[$i] && $$map[$i] eq '<') {
	    if ($#phrase_nodes >= 0) {
		if ($#phrase_nodes >= 0) {
		    push @sentence_nodes, parse_phrases(@phrase_nodes);
		    @phrase_nodes = ();
		}		
	    }
	    push @sentence_nodes, $$nodes[$i];
	} elsif (!defined $$map[$i] || !length $$map[$i]) { # $i == $node_index ||
	    if (is_np_gen($$nodes[$i])) {
		# we should have a head by now; if not create an
		# empty one
		my $head;
		if ($#phrase_nodes >= 0) {
		    if (is_n($phrase_nodes[$#phrase_nodes]) 
			&& is_abs($phrase_nodes[$#phrase_nodes])
			&& !is_regens($phrase_nodes[$#phrase_nodes])) {
			$head = pop @phrase_nodes;
		    } else {
			$head = empty_n();
		    }
		} else {
		    if (is_n($sentence_nodes[$#sentence_nodes])
			&& is_abs($sentence_nodes[$#sentence_nodes])) {
			$head = pop @sentence_nodes;
		    } else {
			$head = empty_n();
		    }
		}
		push @phrase_nodes, ORACC::OSS2::Node::make_node('phrase', [ $head, $$nodes[$i] ], 'NP','Phrasal',701);
	    } else {
		push @phrase_nodes, $$nodes[$i];
	    }
	} else {
	    if ($$map[$i] eq 'S' || $$map[$i] eq 'R' || $$map[$i] eq 'H') {

		if ($$map[$i] eq 'S' && $#phrase_nodes >= 0) {
		    # this is S after an S which doesn't end in a V;
		    # do the same clean up as for 'V' map nodes and return
		    # similarly also
		    push @sentence_nodes, parse_phrases(@phrase_nodes);
		    @phrase_nodes = ();
		    last;
		}

		if ($node_index == $i) {
		    push @phrase_nodes, $$nodes[$i];
		} else {
		    my $rc_head = undef;
		    my $s;
		    my $start = $i;
		    $node_index = $i;
		    $s = parse_sentence(clone($sent),$map);
		    warn "S LABEL == $$s{'label'}\n";
		    my $end = $i;
		    $i = $node_index;
		    if ($$map[$start] eq 'R') {
			$rc_head = pop @phrase_nodes;
		    } elsif ($$map[$start] eq 'H') {
			$rc_head = make_rc_head($start,$end,$map,$nodes);
		    }
		    if ($$map[$start] eq 'R' || $$map[$start] eq 'H') {
			if ($rc_head) {
			    push @phrase_nodes, ORACC::OSS2::Node::make_node('phrase', 
									     [$rc_head, $s],
									     'NP', 'Phrasal',735);
			} else {
			    $$s{'label'} .= '-PRN';
			    if ($#phrase_nodes >= 0) {
				my @c = @{$phrase_nodes[$#phrase_nodes]{'children'}};
				$phrase_nodes[$#phrase_nodes]{'children'} = [ @c, $s ];
			    } else {
				push @phrase_nodes, $s;
			    }
			}
		    } else {
			push @phrase_nodes, $s;
		    }
		}
	    } elsif ($$map[$i] eq 'I') {
		# this is a doubleton node with the [V N] of an RC
		my $rc = ORACC::OSS2::Node::make_node('clause', [ $$nodes[$i], $$nodes[$i+1] ] , 'S-REL','Phrasal',751);
		my $rc_head = make_rc_head($i,$i+1,$map,$nodes);
		if ($rc_head) {
		    push @phrase_nodes, ORACC::OSS2::Node::make_node('phrase', [$rc_head, $rc], 'NP','Phrasal',754);
		} else {
		    $$rc{'label'} .= '-PRN';
		    my @c = @{$phrase_nodes[$#phrase_nodes]{'children'}};
#		    debnod(@c);
		    $phrase_nodes[$#phrase_nodes]{'children'} = [ @c, $rc ];
#		    debnod(@{$phrase_nodes[$#phrase_nodes]{'children'}});
		}
		++$i;
	    } elsif ($$map[$i] eq 'A') {
		if ($node_index == $i) {
		    push @phrase_nodes, $$nodes[$i];
		} else {
		    $node_index = $i;
		    my $s = parse_sentence(clone($sent),$map);
		    $i = $node_index;
		    my $rrc;
		    if ($#phrase_nodes >= 0 
			&& is_abs($phrase_nodes[$#phrase_nodes])
			&& !is_np_gen($phrase_nodes[$#phrase_nodes])
			&& !is_regens($phrase_nodes[$#phrase_nodes])) {
			my @s = ($s);
			# in, e.g., di kud.a.ani, include the POS with the RRC
			if ($i < $#$nodes && is_poss($$nodes[$i+1])) {
			    push @s, $$nodes[$i+1];
			    ++$i;
			}
			$rrc = ORACC::OSS2::Node::make_node('phrase', [ pop(@phrase_nodes), @s ], 'NP','Phrasal',781);
		    } else {
			$rrc = ORACC::OSS2::Node::make_node('phrase', 
					 [ empty_n(), $s ], 
					 'NP','Phrasal',785);
		    }
		    push @phrase_nodes, $rrc;
		}
	    } elsif ($$map[$i] eq 'V') {
		if ($#phrase_nodes >= 0) {
		    push @sentence_nodes, parse_phrases(@phrase_nodes);
		    @phrase_nodes = ();
		}
		push @sentence_nodes, $$nodes[$i];
		if ($i < $#$map && $$map[$i+1] && $$map[$i+1] eq 'N') {
		    push @sentence_nodes, $$nodes[$i+1];
		    ++$i;
		}
		last;
	    } elsif ($$map[$i] eq 'D') {
		if ($i == 0) {
		    $result_node = 'S';
		}
		# ONLY 'P' SHOULD ADD TO @sentence_nodes, EXCEPT FOR LFD and ... ?
#		if ($#phrase_nodes >= 0) {
#		    push @sentence_nodes, parse_phrases(@phrase_nodes);
#		    @phrase_nodes = ();
#		}
		my @cn;
		while ($$map[$i] ne 'V') {
		    push @cn, $$nodes[$i];
		    ++$i;
		}
		push @cn, $$nodes[$i]; # this adds the 'V' node; don't move $i now
		push @phrase_nodes, ORACC::OSS2::Node::make_node('phrase', [ @cn ], 'NC','Phrasal',815);
	    } elsif ($$map[$i] eq 'P') {
		# a P can lose its children if they are an NP-GEN-LFD because that
		# gets put right onto the sentence list; in that case, suppress the
		# phrase generation
		#
		# BUT (2010-03-11): keep a STOP node, and never attach
		# that to preceding children anyway--it stays a top-level
		# constituent.
		if (is_stop($$nodes[$i])) {
		    push @sentence_nodes, $$nodes[$i];
		} elsif ($#phrase_nodes >= 0) {
		    push @phrase_nodes, $$nodes[$i];
		    if ($i < $#$nodes && is_cop($$nodes[$i+1])) {
			push @phrase_nodes, $$nodes[$i+1];
			++$i;
		    }
		    if (head_cfgw($phrase_nodes[0]) eq 'u[and]') {
			my $c = shift @phrase_nodes;
			my $p = parse_phrases(@phrase_nodes);
			push @sentence_nodes, ORACC::OSS2::Node::make_node('phrase',[ $c, $p ], 'CONJP','Phrasal',835);
		    } else {
			push @sentence_nodes, parse_phrases(@phrase_nodes);
		    }
		    @phrase_nodes = ();
		} else {
		    warn "orphan P in map\n";
		    push @phrase_nodes, $$nodes[$i];
		}
	    } elsif ($$map[$i] eq 'G') {
		if ($#phrase_nodes < 0) {
		    push @phrase_nodes, pop @sentence_nodes;
		    if ($#sentence_nodes >= 0 &&
			is_abs($sentence_nodes[$#sentence_nodes])) {
			my $n = pop @sentence_nodes;
			if ($$n{'label'}) {
			    $$n{'label'} =~ s/-ABS//;
			}
			unshift @phrase_nodes, $n;
		    }
		}
		push @phrase_nodes, $$nodes[$i];

		@phrase_nodes = phrases_merge_gen(@phrase_nodes);
		
		if (is_gen($phrase_nodes[$#phrase_nodes])) {
		    # try harder to merge the genitive
		    my $j;
		    for ($j = $#phrase_nodes-1; $j >= 0; --$j) {
			last if is_n($phrase_nodes[$j]);
		    }
		    if ($j >= 0) {
			my $gen = ORACC::OSS2::Node::make_node('phrase', 
					    [ splice(@phrase_nodes, $j, 
						     1 + $#phrase_nodes - $j) ],
					    'NP-GEN','Phrasal',867);
			push @phrase_nodes, $gen;
		    }
		}

		if (is_np_gen($phrase_nodes[0])) {
		    # look ahead for NP-POS; if there is one, assume it's
		    # an anticipatory genitive
		    # FIXME: well, watch me anyway: this algorithm is quite 
		    # simple-minded...
		    my $found_pos = 0;
		    for (my $j = $i+1; $j <= $#$nodes; ++$j) {
			if (is_np_pos($$nodes[$j])) {
			    $found_pos = 1;
#			    ${$$nodes[$j]}{'label'} .= '-RSP-1';
			    my $cref = ${$$nodes[$j]}{'children'};
			    my $posref = $$cref[$#$cref];
			    $$posref{'rsp'} = '1';
			}
		    }
		    if ($found_pos) {
			${$phrase_nodes[0]}{'label'} .= '-LFD-1';
			# copy the LFD right onto the sentence nodes
			# so it doesn't get labeled or phrase-processed
			# in any way
			push @sentence_nodes, relative_traces(shift @phrase_nodes);
		    } else {
			@phrase_nodes = phrases_merge_regens(@phrase_nodes);
		    }
		} else {
		    @phrase_nodes = phrases_merge_regens(@phrase_nodes);
		}
		
#		show_nodes('',1,'',@phrase_nodes);
#		show_nodes('',1,'',@$nodes[$i..$#$nodes]);
	    } elsif ($$map[$i] eq '0') {
                # nodes[$i] is empty, so don't push it onto the list
		push @sentence_nodes, parse_phrases(@phrase_nodes);
		@phrase_nodes = ();
	    } elsif ($end_node && $$map[$i] eq $end_node) {
		if ($end_node eq 'N') {
		    push @post_phrasal_queue, $$nodes[$i];
		} elsif ($end_node eq 'Z') {
		    push @sentence_nodes, parse_phrases(@phrase_nodes), $$nodes[$i];
		    @phrase_nodes = ();
		} else {
		    push @phrase_nodes, $$nodes[$i];
		}
		last;
	    } elsif ($$map[$i] eq 'N') {
		push @sentence_nodes, parse_phrases(@phrase_nodes);
		push @sentence_nodes, $$nodes[$i];
		@phrase_nodes = ();
	    } elsif ($$map[$i] eq 'C') {
		my $np = undef;
		my $modp = undef;
		$modp = maybe_make_node('phrase', [ $$nodes[$i] ], 'RRC');
		my $head;
		if ($#phrase_nodes >= 0 && is_abs($phrase_nodes[$#phrase_nodes])) {
		    $head = pop @phrase_nodes;
		} else {
		    $head = empty_n();
		}
		push @phrase_nodes, maybe_make_node('phrase', [ $head, $modp ], 'NP');
	    } elsif ($$map[$i] eq 'M') {
		# this is a MODP consisting of a nominalized nonfinite verb
		# with no associated NPs
		my $np = undef;
		my $modp = maybe_make_node('phrase', [ $$nodes[$i] ], 'MODP');
		if ($#phrase_nodes >= 0
			 && !is_dim($phrase_nodes[$#phrase_nodes])) {
		    $np = ORACC::OSS2::Node::make_node('phrase', [ pop @phrase_nodes, $modp ], 'NP-MOD','Phrasal',938);
		    push(@phrase_nodes, $np);
		} elsif ($#sentence_nodes >= 0 
			 && !is_dim($sentence_nodes[$#sentence_nodes])) {
		    $np = ORACC::OSS2::Node::make_node('phrase', [ pop @sentence_nodes, $modp ], 'NP-MOD','Phrasal',942);
		    push(@phrase_nodes, $np);
		} else {
		    # WATCHME: is this a safe place to map headless MODPs 
		    # to empty-headed ones?
		    my $head = empty_n();
		    push @phrase_nodes, ORACC::OSS2::Node::make_node('phrase', [ $head, $modp ], 'NP','Phrasal',948);
#		    push @sentence_nodes, $modp;
		}
	    } elsif ($$map[$i] eq '#') {
		# this is an insertion, currently only used to mark the start of 
		# subordinate clauses marked by de.
		my $start = $i+1;
		$node_index = $i+1;
		my $s = parse_sentence(clone($sent),$map);
		my $end = $i;
		$i = $node_index;
		if ($$s{'label'} eq 'RRC') {
		    my $abs_node = undef;
		    if ($#phrase_nodes == -1) {
			if (is_abs($sentence_nodes[$#sentence_nodes])) {
			    $abs_node = pop(@sentence_nodes);
			}
		    } elsif (is_abs($phrase_nodes[$#phrase_nodes])) {
			$abs_node = pop(@phrase_nodes);
		    }
		    unless ($abs_node) {
			$abs_node = empty_n();
		    }
		    push @sentence_nodes, parse_phrases(@phrase_nodes)
			if $#phrase_nodes >= 0;
		    push @sentence_nodes, parse_phrases($abs_node, $s);
		} else {
		    push @sentence_nodes, parse_phrases(@phrase_nodes), $s;		    
		}
		@phrase_nodes = ();
#		push @sentence_nodes, $s;
	    } elsif ($$map[$i] eq 'Z') { ### WATCHME: This fix may be a hack!
		push @sentence_nodes, parse_phrases(@phrase_nodes), $$nodes[$i];
		@phrase_nodes = ();
	    } else {
		warn("Phrasal: untrapped node_map type '$$map[$i]'\n");
	    }
	}
    }
    if ($#phrase_nodes >= 0) {
	push @sentence_nodes, parse_phrases(@phrase_nodes);
#	push @sentence_nodes, @phrase_nodes;
    }
    push @sentence_nodes, @post_phrasal_queue;
    $node_index = $i;

#    return ORACC::OSS2::Node::make_node($node_type, [ @sentence_nodes ], $result_node, 'Phrasal',994);

    ++$n_sentence_exit;
    log_nodes("==== sentence nodes on parse_sentence exit $n_sentence_exit =====\n", 1, '', @sentence_nodes);
    
    $$sent{'children'} = clone(\@sentence_nodes);

    warn "==== sentence dump $n_sentence_exit ===\n", Dumper $sent, "================";

    $sent;
}

sub
post_sentence_fixups {
    my $s = shift;
    $s = fixup_phrasal_conjp($s);
    $s = fixup_copula_sentence($s);
    $s = fixup_copulas($s);
    $s = fixup_singleton_np($s);
    $s = fixup_ABS_X_prn($s);
    $s = case_double_prn($s);
    $s = ORACC::SSA3::CoreArgs::add_core_args($s);
    $s = drop_stop($s);
    foreach my $c (@{$$s{'children'}}) {
	adjust_prn_poss($c);
    }
    $s;
}

sub
case_double_prn {
    my $s = shift;
    my @c = @{$$s{'children'}};
    my $last_case = '';
    for (my $i = 0; $i < $#c; ++$i) {
	if (is_dim($c[$i])) {
	    my $case = case_of($c[$i]);
	    if ($case && $last_case && $case eq $last_case) {
		${$c[$i]}{'label'} .= '-PRN'
		    unless distinct_names($c[$i-1],$c[$i]);
	    } else {
		$last_case = $case;
	    }
	} else {
	    $last_case = '';
	}
    }
    $s;
}

sub
distinct_names {
    my ($l,$r) = @_;
    my ($lh,$rh) = (head($l),head($r));
    if ($$lh{'pos'} =~ /^[DPRTGSW]N$/
	&& $$rh{'pos'} =~ /^[DPRTGSW]N$/
	&& $$lh{'pos'} ne $$rh{'pos'}
	) {
	1;
    } else {
	0;
    }
}

sub
drop_stop {
    my $s = shift;
    return unless $s && $$s{'children'};
    my @c = @{$$s{'children'}};
    for (my $i = 0; $i < $#c; ++$i) {
	if (is_stop($c[$i])) {
	    delete($c[$i]);
	    --$i;
	}
    }
    $$s{'children'} = [ @c ];
    $s;
}

# This is a deliberately conservative routine that is
# primarily intended to catch fronting of the 
# nin .... {d}nin-lil2 kind.
sub
fixup_ABS_X_prn {
    my $s = shift;
    my @c = @{$$s{'children'}};
    for (my $i = 0; $i < $#c; ++$i) {
	if (is_abs($c[$i])
	    && (is_dim($c[$i+1]) && !is_abs($c[$i+1]))
	    && is_animate_node($c[$i]) && is_animate_node($c[$i+1])) {
	    my $case = case_of($c[$i+1]);
	    ${$c[$i]}{'label'} =~ s/-ABS//;	    
	    ${$c[$i+1]}{'label'} = 'NP-PRN';
	    ${$$s{'children'}}[$i] = ORACC::OSS2::Node::make_node('phrase', [$c[$i],$c[$i+1]], "NP-$case",'Phrasal',482);
	    delete(${$$s{'children'}}[$i+1]);
	    --$i;
	    # FIXME?: can this leave (NP-ERG (NP (N nin))(NP-PRN ...))
	    # where the first NP should be unwrapped?
	}
    }
    $s;
}

sub
fixup_copula_sentence {
    my $s = shift;
    my @c = @{$$s{'children'}};
    if ($#c >= 0 && is_np($c[$#c]) && has_cop($c[$#c])) {
	push @{$$s{'children'}}, pop @{$c[$#c]{'children'}};
	unless (has_erg($s)) {
	    unshift @{$$s{'children'}}, empty_cat('ERG');
	}
    }
    $s;
}

sub
fixup_copulas {
    my $s = shift;
    my @c = @{$$s{'children'}};
    my @d = ();
    my $last = pop @c; # protect possible final COP from processing
    foreach my $c (@c) {
	if (is_cop($c)) {
	    if ($#d >= 0) {
		push @{$d[$#d]{'children'}}, $c;
	    } else {
		push @d, $c; # bizarre but possible in a broken text
	    }
	} else {
	    push @d, $c;
	}
    }
    push @d, $last;
    $$s{'children'} = [ @d ];
    $s;
}

sub
fixup_singleton_np {
    my $s = shift;
    my @c = @{$$s{'children'}};
    for (my $i = 0; $i < $#c; ++$i) {
	if (is_np($c[$i]) && $#{${$c[$i]}{'children'}} == 0) {
	    my $currc = $c[$i];
	    my $cnode = ${$$currc{'children'}}[0];
	    if (is_np($cnode)) {
		### WATCHME: do I need to propagate sublabel parts of
		### this upwards?
		${${$$s{'children'}}[$i]}{'children'} = $$cnode{'children'};
	    }
	}
    }
    $s;
}

sub
empty_cat {
    my $e = shift;
    ORACC::OSS2::Node::make_node
	('phrase', 
	 [ ORACC::SSA3::MakeLeaves::make_leaf(undef,'0','','*','',0,'DUMMY','','Phrasal',550) ], 
	 "NP-$e", 'Phrasal', 551);
}

sub
has_erg {
    my $n = shift;
    foreach my $c (@{$$n{'children'}}) {
	return 1 if is_np($c) && ${$c}{'label'} =~ /-ERG/;
    }
    0;
}

sub
has_cop {
    my $n = shift;
    my @c = @{$$n{'children'}};
    is_leaf($c[$#c]) && ${$c[$#c]}{'pos'} eq 'COP';
}

sub
relative_traces {
    my ($s,$case) = @_;
    return $s unless $$s{'children'} && ref($$s{'children'}) eq 'ARRAY';
    my @newc = ();
    foreach my $c (@{$$s{'children'}}) {
	if (is_rel($c)) {
	    push @newc, expand_relative($c,$case);
	} else {
	    my $hcg = head_cfgw($c);
	    if ($hcg && ($hcg eq 'lu[man]' || $hcg eq 'lu[person]')) {
		$case = 'ERG';
	    } else {
		$case = case_of($c) if is_dim($c);
	    }
	    push @newc, relative_traces($c,$case);
	}
    }
    $$s{'children'} = [ @newc ];

    merge_poss($s);

#    show_nodes("==relative_traces before merge_rel==\n",1,'',$s);

    merge_rel($s); # this will bind the S-REL to the head if there was no NP-POS

#    show_nodes("==relative_traces after merge_rel==\n",1,'',$s);

#    merge_gen($s);
#    merge_regens($s);

    add_prn($s);
    $s;
}

sub
expand_relative {
    my($r,$case) = @_;
    my @r = ();
    push @r, ORACC::SSA3::MakeLeaves::make_leaf(undef,'RPRO-1', '', '*','',0,'DUMMY','','Phrasal',609); # '0'
    my $pcase = '';
    $pcase = "-$case" if length $case;
    my @rchildren = @{$$r{'children'}};
    if (!$pcase) {
	foreach my $n (@rchildren) {
	    if ($$n{'type'} eq 'phrase' && $$n{'label'} =~ /ERG/) {
		$pcase = '-ABS';
		last;
	    }
	}
    }
    my $nom = pop @rchildren;
    push @r, (ORACC::OSS2::Node::make_node('clause',
			[
			 ORACC::OSS2::Node::make_node('phrase', [ ORACC::SSA3::MakeLeaves::make_leaf(undef,'*T*-1','','*','',0,'DUMMY',
												    undef, 'Phrasal',624) ], # '0'
				   "NP$pcase",'Phrasal',625),
			 @rchildren
			 ],
			'S-SUB','Phrasal',628),
	      $nom
	      );
    $$r{'children'} = [ @r ];
    $r;
}

sub
empty_n {
    ORACC::SSA3::MakeLeaves::make_leaf(undef,'N','','*','',0,'DUMMY','','Phrasal',637);
}

# On entry, $i is the index of the marked RC node which is an H or I
# map entry, meaning in either case that it is an RC with no explicit
# external head.  $n is the index of the NOM that terminates the RC.
#
# If there is no preceding RC create an empty head.
#
# If there is a preceding RC, determine the state of this RC and the 
# preceding one.  States are:
#
#   unmarked
#   GEN-marked
#   P-marked
#
# If both RCs are unmarked, return undef so this one is appended to the
# preceding NP as an S-REL-PRN.
#
# If the preceding RC is unmarked and this one is marked, return undef
# so that the marking gets applied to the entire preceding sequence of
# S-REL-PRNs.
#
# If the preceding RC is marked, create an empty head.
sub
make_rc_head {
    my($i,$n,$map,$nodes) = @_;
    my $prec_rel = 0;
    my $prec_marked = 0;
    my $ret_head = undef;
    if ($i > 1 && $$map[$i-1] && $$map[$i-1] eq 'N' && is_v_finite($$nodes[$i-2])) {
	$prec_rel = 1;
    } elsif ($i > 2 && $$map[$i-1] && $$map[$i-2]
	     && ($$map[$i-1] eq 'P' || $$map[$i-1] eq 'G') 
	     && $$map[$i-2] eq 'N' && is_v_finite($$nodes[$i-3])) {
	$prec_rel = $prec_marked = 1;
    }
    if ($prec_rel) {
	if ($prec_marked) {
	    $ret_head = empty_n();
	}
    } else {
	$ret_head = empty_n();
    }
    $ret_head;
}

sub
fixup_forced_conjp {
    my $s = shift;
    my @c = @{$$s{'children'}};
    my @nc = ();
    for (my $i = 0; $i <= $#c; ++$i) {
	my $h = head($c[$i]);
	if (is_forced_cnj($h)) {
#	    print STDERR "found +&\n";
	    my $prev = pop @nc;
	    push @nc, ORACC::OSS2::Node::make_node('phrase',[$prev,$empty_conj,$c[$i]],'CONJP','Phrasal',1051);
	} else {
	    push @nc, $c[$i];
	}
    }
    $$s{'children'} = \@nc;
}

sub
fixup_phrasal_conjp {
    my $s = shift;
    my @c = @{$$s{'children'}};
    my @case_stack = '';
    my @case_indexes = '';
    my $last_dim_index = -1;
    for (my $i = 0; $i < $#c; ++$i) {
	if (is_dim($c[$i])) {
	    $last_dim_index = $i;
	    push @case_indexes, $i;
	    push @case_stack, case_of($c[$i]);
	}
	if (is_conjp($c[$i])) {
	    my @conjc = @{$c[$i]{'children'}};
	    # ignore CONJP if they don't match phrasal CONJP pattern
	    if ($#conjc > 0) {
		my $conj_case = case_of($conjc[1]);
		if ($conj_case) {
		    if ($last_dim_index == ($i - 1)
			&& $conj_case eq $case_stack[$#case_stack]) {
			# simple phrasal conj
			$s = make_phrasal_conjp($s,$i-1,$i,0,$conj_case);
		    } elsif ($last_dim_index >= 0) {
			# look for a matching dim in the case stack
			for (my $j = $#case_stack-1; $j > 0; --$j) {
			    if ($conj_case eq $case_stack[$j]) {
				$s = make_phrasal_conjp($s,$case_indexes[$j],
							$i,1,$conj_case);
			    }
			}
		    }
		}
	    }
	}
    }
    $s;
}

sub
make_phrasal_conjp {
    my ($s,$head,$conj,$ich,$case) = @_;
    my @c = @{$$s{'children'}};
    my @r = ();

    # copy the pre-conjp nodes 
    push @r, @c[0..$head-1];

    my $head_node = $c[$head];
    my $conj_node;
    if ($ich) {
	$conj_node = ORACC::SSA3::MakeLeaves::make_leaf(undef, 'CONJP-1',0,'*ICH*','',0,'DUMMY','','Phrasal',1110);
    } else {
	$conj_node = $c[$conj];
    }
    push @r, ORACC::OSS2::Node::make_node('phrase', [$head_node, $conj_node], "NP-$case",'Phrasal',1114);

    if ($ich) {
	push @r, @c[$head+1..$conj-1];
	my $rsp = $c[$conj];
	$$rsp{'label'} .= '-1';
	push @r, $rsp;
    }

    # copy remaining nodes
    push @r, @c[$conj+1..$#c];

    # replace sentence children
    $$s{'children'} = \@r;
    $s;
}

sub
phrases_merge_gen {
    my @nodes = @_;
    my $phrase = undef;
    if ($#nodes != 0 || !is_np($nodes[0])) {
	$phrase = ORACC::OSS2::Node::make_node('phrase', [ @nodes ], 'NP','Phrasal',1136);
    } else {
	$phrase = $nodes[0];
    }
    merge_gen($phrase, 1);
    @{$$phrase{'children'}};
}

sub
phrases_merge_regens {
    my @nodes = @_;
    my $phrase = undef;
    if ($#nodes != 0 || !is_np($nodes[0])) {
	$phrase = ORACC::OSS2::Node::make_node('phrase', [ @nodes ], 'NP','Phrasal',1149);
    } else {
	$phrase = $nodes[0];
    }
    merge_regens($phrase);
    @{$$phrase{'children'}};
}

sub
parse_phrases {
    my @nodes = @_;

    if ($#nodes == 0 && (is_vnf($nodes[0]) || $nodes[0]{'type'} eq 'clause')) {
	return $nodes[0];
    }

    my $phrase = ORACC::OSS2::Node::make_node('phrase', [ @nodes ], 'NP','Phrasal',1222);
    
    merge_adj($phrase,' parse_phrases');

    # now get any POS that had a NOM immediately before them on pass 1
    merge_poss($phrase);

    $phrase = label_phrase($phrase);

    ORACC::OSS2::Log::log_nodes("==parse_phrases after label_phrase==\n",1,'',$phrase);

    $phrase = relative_traces($phrase, '');

    $phrase = fixup_copulas($phrase);

    add_prn($phrase);

    $phrase;
}

sub
remove_redundant_np {
    my $p = shift;
    if (is_np($p)) {
	if ($#{$$p{'children'}} == 0 && is_np(${$$p{'children'}}[0])) {
	    my $ret = $$p{'children'}[0];
	    my($x) = ($$p{'label'} =~ /(-.*)$/);
	    if ($x) {
		$$ret{'label'} .= $x unless $$ret{'label'} =~ s/-/$x-/;
	    }
	    return $ret;
	}
    }
    $p;
}

sub
label_phrase {
    my $phrase = shift;
    my @nodes = @{$$phrase{'children'}};
    my $node_label = '';

    my $first_node = $nodes[0];
    my $last_node = $nodes[$#nodes];

    my $head = head($first_node);

    return $phrase unless $head && $$head{'pos'};

    # coerce obvious animate heads+e into ERG rather than LT;
    # this could be better with real animate marking in the lexicon
    if (!$$head{'ro'} && ($$head{'pos'} =~ /^[DRP]N/ || is_animate($$head{'form'}))) {
	if ($$last_node{'pos'} && $$last_node{'pos'} eq 'LT') {
	    $$last_node{'pos'} = 'ERG';
	}
    }

    if (is_cop($last_node) && $#nodes > 0) {
	$last_node = $nodes[$#nodes-1];
    }

    if (is_pp($last_node)) {
	if (is_gen($nodes[$#nodes-1])) {
	    return ORACC::OSS2::Node::make_node('phrase', [ @nodes[0..$#nodes-1] ], 
			     "NP-GEN-$$last_node{'pos'}",'Phrasal',1248);
	} else {
	    $$phrase{'type'} = 'phrase';
	    if ($$last_node{'pos'} eq 'INSERTP') {
		$$phrase{'type'} = 'clause';
		$$phrase{'label'} = 'INSERTP';
		pop @{$$phrase{'children'}};
	    } else {
		if ($node_label) {
		    $$phrase{'label'} = $node_label;
		} else {
		    $$phrase{'label'} = 'NP-'.$$last_node{'pos'};
		}
	    }
	}
    } elsif (is_n(head($first_node)) 
	     || (is_x($first_node) && $#nodes > 0 && is_n($nodes[1]))) {
	$$phrase{'type'} = 'phrase';
	if ($$phrase{'label'} =~ /NP-/) {
	    $$phrase{'label'} .= '-ABS' unless $$phrase{'label'} =~ /MSR/;
	} else {
	    $$phrase{'label'} = 'NP-ABS';
	}
    } elsif (is_v_finite($first_node)) {
	if ($#nodes == 0) {
	    $phrase = $first_node;
	} else {
	    $$phrase{'type'} = 'phrase';
	    $$phrase{'label'} = 'VP';
	}
    }
#    if ($$last_node{'pos'} eq 'TER') {
#	debnod(@{$$phrase{'children'}});
#    }
    $phrase;
}

# This routine is actually a driver for rewrapping of several different
# N N adjacencies: parenthetic, conjunctive and modifying.  In the PPCS
# syntax tree bare N N adjacency is reserved for the components of compound
# nouns.
sub
add_prn {
    my $phrase = shift;
#    print LOG Dumper($phrase) if $logging;
    return if $$phrase{'label'} && $$phrase{'label'} eq 'NC';
    my @nodes = @{$$phrase{'children'}};
    if ($#nodes > 0 && is_n($nodes[0])) {
	my @newn = (shift @nodes);
	my $PRN = 1;
	my $conj_mode = 0;
	my $xn;
	my $exit_flag = 0;
	foreach my $n (@nodes) {
	    undef $xn;
	    if ($PRN) {
		if (is_n($n)) {

		    if (para_syntax($n,'ante','no_prn')) {
			$exit_flag = 1;
			last;
		    }

		    my ($new_n,$type) = make_prn($n, $newn[$#newn]);
		    if ($type eq 'STOP') {
#			$PRN = 0;
		    } elsif ($type eq 'MODP' || $type eq 'CONJP') {
			# we've used the last item on @newn in the MODP, so pop it off
			pop @newn;
			$n = $new_n;
			# now carry on to see if next NP is PRN or whatever
		    } 
		    if (!$conj_mode || $type eq 'CONJP') {
			# we've used the last item on @newn in the NP-CNJ, so pop it off
			if ($type eq 'CONJP') {
			    $conj_mode = 1;
			    # if the last item in the second NP in the NP-CNJ
			    # is a POSS, reattach it above the NP-CNJ
			    # FIXME: this a horrid hack with no genericity!

			    my @children = @{$$n{'children'}};
			    my $last_node = $children[$#children];
			    if (is_np($last_node)) {
				my @sub_children = @{$$last_node{'children'}};
				my $last_subnode = $sub_children[$#sub_children];
				if (is_poss($last_subnode)) {
				    if ($#sub_children > 1) {
					pop @sub_children;
					${$last_subnode}{'children'} = [ @sub_children ];
				    } else {
					${$$n{'children'}}[$#{$$n{'children'}}] 
					    = $sub_children[0];
				    }
				    $xn = $sub_children[$#sub_children];
				}
			    }
			} else {
			    $n = $new_n;
			}
		    } else {
			$PRN = 0;
		    }
		} else {
		    $PRN = 0;
		}
	    }
	    push (@newn, $n);
	    push (@newn, $xn) if $xn;
	}
	unless ($exit_flag) {
	    if ($#newn == 0 && is_np($newn[0]) && is_np($phrase)) {
		$$phrase{'children'} = ${$newn[0]}{'children'};
	    } else {
		$$phrase{'children'} = \@newn;
	    }
	}
    }
}

sub
make_prn {
    my ($n,$prev_n) = @_;
    my $type = 'PRN';
    if (is_leaf($n)) {
	$n = leaf_to_phrase($n);
	$$n{'label'} .= '-PRN';
#    } elsif ($$n{'label'} eq 'S-NOM') {
#	$n = ORACC::OSS2::Node::make_node('phrase', [ $n ], 'NP-PRN');
    } elsif ($$n{'label'} eq 'NP-GEN') { # || $$n{'label'} eq 'NP-MOD'
	$type = 'STOP';
    } else {
	if (is_forced_cnj(head($n))) {
#	my $c = ${$$n{'children'}}[0];
#	if (!is_leaf($c) && $$c{'label'} eq 'NP-MSR') {
	    $type = $$n{'label'} = 'CONJP';
	    print STDERR "found CONJP\n";
#	    $type = 'STOP';
	} else {
	    if ($$n{'label'} !~ /^S/) {
		my $prev_case = '';
		if ($prev_n && $$prev_n{'label'}) {
		    $prev_case = $1 if $$prev_n{'label'} =~ /^NP-([A-Z]+)/;
		}
		if ((!defined($prev_case) || !length($prev_case) 
		     || $prev_case =~ /^PRN|MOD$/)
		    && ($$n{'label'} eq 'NP' || $$n{'label'} =~ /NP-(POS|MOD)/)) {
		    unless ($$n{'label'} =~ s/-(POS|MOD)/-PRN-$1/) {
			$$n{'label'} .= '-PRN';
		    }
		    clean_first_child($n);
		} else {
		    if ($prev_case && $$n{'label'} =~ /^NP-$prev_case$/) {
			unless ($$n{'label'} =~ /-PRN/) {
			    $$n{'label'} .= '-PRN';
			    clean_first_child($n);
			}
		    }
		}
	    }
	}
    }
    ($n,$type);
}

sub
extract_final_poss {
    my $n = shift;
    my @children = @{$$n{'children'}};
    my $last_node = $children[$#children];
    my $ret = undef;
    my $tail = find_last_child_node($n);
    if (is_poss($tail)) {
	$ret = remove_last_child_node($n);
    }
    $ret;
}

sub
find_last_child_node {
    my $n = shift;
    while ($$n{'children'} && $#{$$n{'children'}}) {
	$n = ${$$n{'children'}}[$#{$$n{'children'}}];
    }
    $n;
}
sub
remove_last_child_node {
    my $n = shift;
    my $parent = undef;
    while ($$n{'children'} && $#{$$n{'children'}} >= 0) {
	$parent = $n;
	$n = ${$$n{'children'}}[$#{$$n{'children'}}];
    }
    pop @{$$parent{'children'}};
}

sub
clean_first_child {
    my $n = shift;
    my $nc = ${$$n{'children'}}[0];
    if ($$nc{'label'} && $$nc{'label'} =~ /-PRN$/) {
	$$nc{'label'} = 'NP';
    }
    if ($$nc{'label'} && $$nc{'label'} eq 'NP' && $#{$$nc{'children'}} == 0) {
	$nc = ${$$nc{'children'}}[0];
    }
    ${$$n{'children'}}[0] = $nc;
}

sub
leaf_to_phrase {
    my $n = shift;
    my %leaf = %{$n};
    $$n{'type'} = 'phrase';
    if ($$n{'pos'} =~ /^.N/) {
	$$n{'label'} = 'NP';
    } else {
	$$n{'label'} = "$$n{'pos'}P";
    }
    $$n{'children'} = [ \%leaf ];
    delete $$n{'pos'};
    $n;
}

sub
merge_adj {
    my ($phrase,$call) = @_;
    return $phrase unless $$phrase{'children'} && ref($$phrase{'children'}) eq 'ARRAY';
    my @nodes = @{$$phrase{'children'}};
    my @ret = ();
    my $i;
    $call = '' unless $call;
    ORACC::OSS2::Log::log_nodes("=========before merge_adj$call========\n",1,'',@nodes);
    for ($i=0; $i < $#nodes; ++$i) {
	if ($#nodes-$i >= 1 && is_n($nodes[$i]) 
	    && is_adj($nodes[$i+1],$nodes[$i+2]) && !is_modp($nodes[$i+1])) {
	    my $mod;
	    $mod = ORACC::OSS2::Node::make_node('phrase',
					       [ $nodes[$i+1] ],
					       'MODP','Phrasal',1487);
	    push @ret, ORACC::OSS2::Node::make_node('phrase',[ $nodes[$i], $mod ],'NP-MOD','Phrasal',1488);
	    ++$i; # skip over ADJ
	} else {
	    push @ret, $nodes[$i];
	}
    }
    push(@ret, $nodes[$#nodes]) unless $i > $#nodes;
    ORACC::OSS2::Log::log_nodes("=========after merge_adj========\n",1,'',@ret);
    $$phrase{'children'} = \@ret;
}

sub
merge_rel {
    my $phrase = shift;
    my @nodes = @{$$phrase{'children'}};
    my @ret = ();
    my $i;
    for ($i=0; $i < $#nodes; ++$i) {
#	if (is_n($nodes[$i]) && is_rel($nodes[$i+1])
#	    && (!$nodes[$i+2] || !is_poss($nodes[$i+2]))) {
#	    push @ret, ORACC::OSS2::Node::make_node('phrase',[ $nodes[$i], $nodes[$i+1] ],'NP');
#	    ++$i; # skip over REL
	if (is_rel($nodes[$i]) || (is_n($nodes[$i]) && is_rel($nodes[$i+1]))) {
	    my $j = $i;
	    while ($j < $#nodes) {
		if (is_rel($nodes[$j+1])) {
		    ++$j;
		} else {
		    last;
		}
	    }
	    if ($j+2 > $#nodes || !is_poss($nodes[$j+2])) {
		if (is_rel($nodes[$i])) {
		    push @ret, ORACC::OSS2::Node::make_node('phrase', [ empty_n(), @nodes[$i..$j] ], 'NP','Phrasal',1521);
		} else {
		    if ($i || $j < $#nodes) {
			push @ret, ORACC::OSS2::Node::make_node('phrase', [ @nodes[$i..$j] ], 'NP','Phrasal',1524);
		    } else {
			push @ret, @nodes;
		    }
		    $i = $j+1;
		}
	    } else {
		push @ret, $nodes[$i];
	    }
	} else {
	    push @ret, $nodes[$i];
	}
    }
    push(@ret, $nodes[$#nodes]) unless $i > $#nodes;

    $$phrase{'children'} = \@ret;
}

sub
merge_cnj {
    my $phrase = shift;
    my @nodes = @{$$phrase{'children'}};
    my @ret = ();
    my $i;
    my $merged = 0;
    # FIXME: is it enough to use $last_was_merge only with nnmod?
    my $last_was_merge = 0;
    for ($i=$#nodes; $i >= 0; --$i) {
	my $nnmod = 0;
	if ($i &&
	    ORACC::SSA3::NNCnj::is_nncnj(head_cfgw($nodes[$i-1]),
					 head_cfgw($nodes[$i]))) {
	    my $n = ORACC::OSS2::Node::make_node('phrase', [ $nodes[$i-1], $empty_conj, 
							    $nodes[$i] ], 'NP-CONJ','Phrasal',1557);
	    --$i;
	    ++$merged;
	    unshift @ret, $n;
	} elsif ($last_was_merge 
		 && ($nnmod = ORACC::SSA3::NNMod::is_nnmod(head_cfgw($nodes[$i]),
							   head_cfgw($ret[0])))
		 ## This is going to be hairy; sometimes we well want to take the
		 ## NNMod as have GEN after the merged NNMod, but this line
		 ## suppresses that behaviour and makes e Jirsu-ka-ni parse as
		 ## (e (Jirsu)).ak.ani
		 && ($i == $#nodes || !is_gen($nodes[$i+1])
		     || ($i > 1 && known_prn($nodes[$i-2], $nodes[$i-1])))
		 ) {
	    my $n1 = $nodes[$i];
	    my $n2 = shift @ret;
	    if ($nnmod < 0) { # it's a pre-MOD
		$n1 = ORACC::OSS2::Node::make_node('phrase', [ $n1 ], 'MODP','Phrasal',1574);
	    } else {
		$n2 = ORACC::OSS2::Node::make_node('phrase', [ $n2 ], 'MODP','Phrasal',1576);
	    }
	    unshift @ret, ORACC::OSS2::Node::make_node('phrase', [ $n1, $n2 ], 'NP-MOD','Phrasal',1578);
	} elsif ($i && 
		 ($nnmod = ORACC::SSA3::NNMod::is_nnmod(head_cfgw($nodes[$i-1]),
							head_cfgw($nodes[$i])))
		 ## See the comment above.
		 && ($i == $#nodes || !is_gen($nodes[$i+1])
		     || ($i > 1 && known_prn($nodes[$i-2], $nodes[$i-1])))
	    ) {
	    my $n1 = $nodes[$i-1];
	    my $n2 = $nodes[$i];
	    --$i;
	    ++$merged;
	    $last_was_merge = 1;
	    if ($nnmod < 0) { # it's a pre-MOD
		$n1 = ORACC::OSS2::Node::make_node('phrase', [ $n1 ], 'MODP','Phrasal',1592);
	    } else {
		$n2 = ORACC::OSS2::Node::make_node('phrase', [ $n2 ], 'MODP','Phrasal',1594);
	    }
	    unshift @ret, ORACC::OSS2::Node::make_node('phrase', [ $n1, $n2 ], 'NP-MOD','Phrasal',1596);
	} elsif ($i && is_num($nodes[$i]) && !is_initial($nodes[$i])) {
	    my $nummod = ORACC::OSS2::Node::make_node('phrase', [ $nodes[$i] ], 'MODP','Phrasal',1598);
	    unshift @ret, ORACC::OSS2::Node::make_node('phrase',[ $nodes[$i-1], $nummod ],'NP-MOD','Phrasal',1599);
	    --$i;
	    ++$merged;
	    $last_was_merge = 1;
	} else {
	    unshift @ret, $nodes[$i];
	    $last_was_merge = 0;
	}
    }
    $$phrase{'children'} = \@ret;
    $merged;
}

sub
merge_plur {
    my $phrase = shift;
    my @nodes = @{$$phrase{'children'}};
    my @ret = ();
    my $i;
    for ($i=0; $i < $#nodes; ++$i) {
	if (is_n($nodes[$i]) && is_plur($nodes[$i+1])) {
	    push @ret, ORACC::OSS2::Node::make_node('phrase',[ $nodes[$i], $nodes[$i+1] ],'NP-PL','Phrasal',1716);
	    ++$i; # skip over PLU
	} else {
	    push @ret, $nodes[$i];
	}
    }
    push(@ret, $nodes[$#nodes]) unless $i > $#nodes;
    $$phrase{'children'} = \@ret;
}

sub
nmerge_gen {
    my $phrase = shift;
    return if is_np_gen($phrase);
    my @nodes = @{$$phrase{'children'}};
    my @ret = ();
    while (1) {
	# find index of next gen in nodes
	my $gen = find_gen(@nodes);
	last unless $gen;
	# find head; note that headless genitives can occur
	# head is index into @nodes giving first post-head node
	# that must be wrapped into the NP that wraps the NP-GEN
	my $head = find_gen_head(@nodes[0..$gen]);
	my @todo = splice(@nodes,0,$gen);
	my @genc = splice(@todo,$head,$#todo);

	# move everything up to head to @ret
	push @ret, @todo;
	# create new node containing head..gen as children
	my $gnode = ORACC::OSS2::Node::make_node('phrase',[ @genc ],'NP-GEN','Phrasal',1746);
	# add it to ret
	push @ret, $gnode;
	# go again
    }
}

sub
find_gen {
    for (my $i = 0; $i <= $#_; ++$i) {
	return $i if is_gen($_[$i]);
    }
    -1;
}
sub
find_gen_head {
    # find preceding NP which is not NP-POS

    # if none or it has POST, it's headless GEN or anticipatory

    # if preceding is PN/RN and head is profession, take preceding
    # also
}

sub
merge_gen {
    my ($phrase,$force) = @_;
    return if is_np_gen($phrase);
    my @nodes = @{$$phrase{'children'}};
    
    my @ret = ();

    for (my $i=0; $i <= $#nodes; ++$i) {
	if (($force || $i) && $i < $#nodes && is_n($nodes[$i]) 
	    && !is_np_gen($nodes[$i]) && is_gen($nodes[$i+1])) {
	    my @gen = ();
	    my $last_node = $nodes[$i];
	    while ($#ret >= 0 && is_prn($ret[$#ret], $last_node, 1)) { # is_prn in strict mode requires proper noun
		my($n,$t) = make_prn($last_node, undef);
		unshift @gen, $n;
		$last_node = pop @ret;
	    }
	    unshift @gen, $last_node;
	    push @gen, $nodes[$i+1];
	    my $np_gen = ORACC::OSS2::Node::make_node('phrase',[ @gen ],'NP-GEN','Phrasal',1790);
	    # merge this with its regens if it's a simple one
	    if ($#ret >= 0 && is_n($ret[$#ret]) && !is_regens($ret[$#ret])) {
		push @ret, ORACC::OSS2::Node::make_node('phrase', [ pop @ret, $np_gen ], 'NP','Phrasal',1793);
	    } else {
		push @ret, $np_gen;
	    }
	    ++$i; # step to GEN so that next loop post-incr steps past it
	} else {
	    push @ret, $nodes[$i];
	}
    }
    $$phrase{'children'} = \@ret;
}

sub
merge_regens {
    my $phrase = shift;
    ORACC::OSS2::Log::log_nodes2("==pre merge-regens============================\n",1,'',$phrase);
    my @nodes = @{$$phrase{'children'}};

    return if is_lfd($nodes[0]) || ($#nodes >= 0 && is_np_gen($nodes[1]));

    my @ret = ();
    my $i;

    if (is_np_gen($nodes[0])) {
	push @ret, ORACC::OSS2::Node::make_node('phrase',[ empty_n(), $nodes[0] ],'NP','Phrasal',1817);
	push @ret, @nodes[1..$#nodes];
    } else {
	for ($i=0; $i < $#nodes; ++$i) {
	    # only wrap the regens+gen if it is non-initial or non-final; this is intended
	    # to fix a bug with the double merge_regens call (we assume for now that the 
	    # double merge_regens call is necessary)
	    if (is_n($nodes[$i]) && is_abs($nodes[$i])
		&& is_np_gen($nodes[$i+1])) { # && ($i > 0 || ($i+1) < $#nodes)) {
		if (is_regens($nodes[$i])) {
		    my @children = @{$nodes[$i]{'children'}};
		    ${$nodes[$i+1]}{'label'} = 'NP-GEN-PRN';
		    push @{$nodes[$i]{'children'}}, $nodes[$i+1];
		    push @ret, $nodes[$i];
		} else {
		    push @ret, ORACC::OSS2::Node::make_node('phrase',[ $nodes[$i], $nodes[$i+1] ],'NP','Phrasal',1832);
		}
		++$i; # skip over GEN
	    } else {
		push @ret, $nodes[$i];
	    }
	}
	push (@ret, $nodes[$#nodes])  unless $i > $#nodes;
    }
    $$phrase{'children'} = \@ret;
    ORACC::OSS2::Log::log_nodes2("==post merge-regens=========================\n",1,'',$phrase);
}

sub
merge_poss {
    my $phrase = shift;
    return if $$phrase{'label'} && $$phrase{'label'} =~ /-POS/;
    my @nodes = @{$$phrase{'children'}};
    my @ret = ();
    my $i;
    for ($i=0; $i <= $#nodes; ++$i) {

	if (is_poss($nodes[$i]) && $i 
	    && !is_gen($nodes[$i-1]) && !is_nom($nodes[$i-1])
	    && !is_v($nodes[$i-1])) {
	    my $j = $i;
	    while (--$j >= 0) {
		last if is_n($nodes[$j]);
	    }
	    if ($j >= 0 && $j < $i) {
		my @poss_children = ();
		while ($j++ < $i) {
		    unshift(@poss_children, pop(@ret));
		}
#		print Dumper $phrase; print "====\n";
		push @ret, ORACC::OSS2::Node::make_node('phrase',[ grep(defined,@poss_children), $nodes[$i] ],'NP-POS','Phrasal',1867);
#		print Dumper $ret[$#ret]; print "====\n";
	    } else {
		push @ret, $nodes[$i];
	    }
	} else {
	    push @ret, $nodes[$i];
	}

#	if (is_n($nodes[$i]) && is_poss($nodes[$i+1])) {
#	    push @ret, ORACC::OSS2::Node::make_node('phrase',[ $nodes[$i], $nodes[$i+1] ],'NP-POS');
#	    ++$i; # skip over POSS
#	} elsif ($i < ($#nodes-1) && is_n($nodes[$i]) 
#		 && is_s($nodes[$i+1]) && is_poss($nodes[$i+2])) {
#	    push @ret, ORACC::OSS2::Node::make_node('phrase',[ $nodes[$i], $nodes[$i+1], $nodes[$i+2] ],
#				 'NP-POS');
#	    $i += 2; # skip over S and POSS
#	} else {
#	    push @ret, $nodes[$i];
#	}

    }
    push (@ret, $nodes[$#nodes]) unless $i > $#nodes;
    if ($#ret == 0) {
	my $n = $ret[0];
	if ($$n{'label'} && $$n{'label'} =~ /POS/) {
	    $$phrase{'label'} .= '-POS';
	    @ret = @{$$n{'children'}};
	} 
    }
    $$phrase{'children'} = \@ret;
}

my $adjust_count = 0;
# Weak support for raising POSS on PRN phrases (i.e., his house, TN instead
# of his TN-house)
sub
adjust_prn_poss {
    my($phrase) = @_;
    my $pp = undef;
#    print Dumper $phrase;
    my $hcf = head_cfgw($phrase);
    my $cc = $#{$$phrase{'children'}} + 1;
#    warn "adjust_prn_poss: head = $hcf; childcount = $cc\n";
    if (!is_proper(head($phrase)) && $#{$$phrase{'children'}} > 0) {
	my @n = @{$$phrase{'children'}};
	my $first = $n[0];
	my $first_last = ${$$first{'children'}}[$#{$$first{'children'}}];
#	warn "adjust_prn_poss: is_poss first_last = ", is_poss($first_last), "\n"
#	    if $first_last;
	if (!is_np_pos($first)) {
#	    warn "adjust_prn_poss\n";
	    my $pos_child = $n[$#n];
	    if (is_pp($pos_child)) {
		$pp = $pos_child;
		$pos_child = $n[$#n-1];
	    }
	    if (is_np_pos($pos_child) && is_proper(head($pos_child))) {
		$$pos_child{'label'} =~ s/-POS//;
		my $rempp = remove_last_child_node($phrase) if $pp;
		my $pos = remove_last_child_node($pos_child);
		if (is_leaf($n[0])) {
		    $n[0] = ORACC::OSS2::Node::make_node('phrase',[ $n[0] ],'NP','Phrasal',1925);
		}
		my $new_n = ORACC::OSS2::Node::make_node('phrase',[ @n, $pos ],'NP-POS','Phrasal',1927);
		if ($pp) {
		    $$phrase{'children'} = [ $new_n , $pp ];
		} else {
		    $$phrase{'children'} = [ $new_n ];
		}
		ORACC::OSS2::Log::show_nodes("adjust_prn_pos\n", 1, '', $new_n);
	    }
	}
    }
}

sub
case_of {
    my $node = shift;
    if ($$node{'type'} eq 'phrase') {
	$$node{'label'} =~ /-(ERG|ABS|ABL|COM|LOC|TER|LT|DAT|VOC)/;
	return $1;
    } else {
        return $$node{'pos'};
    }
}

sub
brk_leaf {
    $_[0] =~ /HR|RC|RRC|PRP|INF|QUE|THT/;
}

sub
same_dsu {
    my($dsu1,$dsu2) = @_;
    my($dsu1a,$dsu1b,$dsu2a,$dsu2b);
    ($dsu1a,$dsu1b) = ($dsu1 =~ /^(.*?)\#(.*)$/);
    ($dsu2a,$dsu2b) = ($dsu2 =~ /^(.*?)\#(.*)$/);
    ($dsu1a eq $dsu2a && ($dsu2b - $dsu1b) == 1);
}

sub
no_abs {
    my ($i,@s) = @_;
    for (my $j = 0; $j < $i; ++$j) {
	my $n = $s[$j];
	return 0 if $$n{'type'} eq 'leaf' && $$n{'pos'} eq 'ABS';
    }
    1;
}

sub
para_syntax {
    my($argnode,$pos,$type) = @_;
    my $node = undef;
#    print STDERR Dumper($argnode);
    if (ref($argnode) eq 'XML::LibXML::Element') { # && $argnode->isa('XML::LibXML::Element')) {
	$node = $argnode;
    } else {
	my $head = head($argnode);
	$node = $$head{'lnode'};
    }
    if ($node) {
	foreach my $p ($node->childNodes()) {
	    if ($p->isa('XML::LibXML::Element')
		&& $p->localName() eq 'para'
		&& $p->getAttribute('pos') eq $pos) {
		foreach my $c ($p->childNodes()) {
		    if ($c->isa('XML::LibXML::Element')
			&& $c->getAttribute('type') eq $type) {
			return $c;
		    }
		}   
	    }
	}
    }
    undef;
}

sub
map_phrase {
    my $l = shift;
    my %label2map = (
	RRC=>'C',
	MODP=>'M',
	'NP-ERG' =>'P',
	'NP-ABS' =>'P',
	'NP-DAT' =>'P',
	'NP-LT'  =>'P',
	'NP-LOC' =>'P',
	'NP-TER' =>'P',
	'NP-ABL' =>'P',
	'NP-EQU' =>'P',
	'NP-COM' =>'P',
	'VC-N' =>'A',
	'VC-F' =>'V',
	);
    if ($l && $label2map{$l}) {
	$label2map{$l};
    } elsif ($l =~ /^S-/) {
	'S';
    } else {
#	warn "Phrasal:map_phrase: unhandled phrase label `$l'\n";
#	'P';
	''
    }
}

sub
map_default {
    my $n = shift;
    my %label2map = (
	sentence=>'S',
	clause=>'C',
	phrase=>'P',
	);
    if ($n && $label2map{$n}) {
	$label2map{$n};
    } else {
	warn "Phrasal:map_default: unknown default map type '$n'\n";
	'S';
    }
}

sub
maybe_make_node {
    my($type,$children,$label) = @_;
    my $c = $$children[0];
    if ($c && $$c{'label'} && $$c{'label'} eq $label) {
	$c;
    } else {
	ORACC::OSS2::Node::make_node($type, $children, $label,'Phrasal',2023);
    }
}

sub
known_prn {
    foreach (@_) {
	return 0 unless exists $known_prn{head_cfgw($_)};
    }
    1;
}

1;
