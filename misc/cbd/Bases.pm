package ORACC::CBD::Bases;
require Exporter;
@ISA=qw/Exporter/;

@EXPORT = qw/bases_hash bases_log bases_log_errors bases_fixes bases_process bases_stats/;

use warnings; use strict; use open 'utf8'; use utf8;

use ORACC::CBD::PPWarn;
use Data::Dumper;

my $bound = '(?:[-\|.{}()/ ]|$)';
my %fixes = ();
my %log_errors = ();
my %stats = ();

sub bases_fixes {
    my %tmp = %fixes;
    %fixes = ();
    %tmp;
}

sub bases_hash {
    my($arg,$is_compound) = @_;
    if ($arg =~ s/;\s*$//) {
	pp_warn("bases entry ends with semi-colon--please remove it");
    }
    my @bits = split(/;\s+/, $arg);

    my $alt = '';
    my $stem = '';
    my $pri = '';
    my %vbases = (); # this one is just for validation of the current @bases field
    my $pricode = 0;

    foreach my $b (@bits) {
	if ($b =~ s/^\*(\S+)\s+//) {
	    $stem = $1;
	} elsif ($b =~ /^\*/) {
	    $b =~ s/^\*\s*//;
	    pp_warn("misplaced '*' in \@bases");
	}
	if ($b =~ /\s+\(/) {
	    my $tmp = $b;
	    pp_warn("malformed alt-base in `$b'")
		if ($tmp =~ tr/()// % 2);
	    ($pri,$alt) = ($b =~ /^(.*?)\s+\((.*?)\)\s*$/);
	    if ($pri) {
		if ($pri =~ s/>.*$//) {
		    pp_warn("can't merge bases in a file with unresolved edits");
		}
		if ($pri =~ /\s/ && !$is_compound) {
		    pp_warn("space in base `$pri'");
		    $pri = $alt = '';
		} else {
#		    ++$bases{$pri};
#		    $bases{$pri,'*'} = $stem
#			if $stem;
		}
		if ($pri) {
		    if (defined $vbases{$pri}) {
			pp_warn("repeated base $pri");
		    } else {
			%{$vbases{$pri}} = ();
			$vbases{"$pri#code"} = ++$pricode;
		    }
		    foreach my $a (split(/,\s+/,$alt)) {
			if ($a =~ /\s/ && !$is_compound) {
			    pp_warn("space in alt-base `$a'");
			    $pri = $alt = '';
			} else {
			    if ($a) {
				if (${$vbases{$pri}}{$a}++) {
				    pp_warn("$pri has repeated alternate base $a");
				}
				# all alternates for this primary
				++${$vbases{"$pri#alt"}}{$a};
				# all alternates in this @bases
				if (defined ${${$vbases{'#alt'}}{$a}}) {
				    my $prevpri =  ${${$vbases{'#alt'}}{$a}};
				    pp_warn("alt $a already defined for primary $prevpri");
				} else {
				    ${${$vbases{'#alt'}}{$a}} = $pri;
				}
			    }
			}
		    }
		}
	    } else {
		pp_warn("syntax error in base with (...) [missing paren?]");
	    }
	} else {
	    if ($b =~ /\s/ && !$is_compound) {
		pp_warn("space in base `$b'");
		$pri = $alt = '';
	    } else {
#		++$bases{$b};
#		$bases{$b,'*'} = $stem
#		    if $stem;
		$pri = $b;
		$alt = '';
		if (defined $vbases{$pri}) {
		    pp_warn("repeated base $pri");
		} else {
		    %{$vbases{$pri}} = ();
		    $vbases{"$pri#code"} = ++$pricode;
		}
	    }
	}
    }
#    warn Dumper \%vbases;
    %vbases;
}

sub bases_log{
    my $args = shift;
    open(L,$$args{'log'});
    while (<L>) {
	chomp;
	next unless /\(bases\)/;
	my($file,$line,$err) = (/^(.*?):(.*?):\s+\(bases\)\s+(.*?)\s*$/);
	if ($file eq $$args{'cbd'}) {
	    push @{$log_errors{$line}}, $err;
	}
    }
    close(L);
}

sub bases_log_errors {
    my $n = shift;
    if (defined $log_errors{$n}) {
	@{$log_errors{$n}};
    } else {
	()
    }
}

sub bases_prefer {
    my($cfgw,$a,$b) = @_;
    # try stats first
    if (defined $stats{$cfgw}) {
	my %s = %{$stats{$cfgw}};
	if ($s{$a} && $s{$b}) {
	    if ($s{$a} > $s{$b}) {
		return $a;
	    } elsif ($s{$b} > $s{$a}) {
		return $b;
	    }
	} elsif ($s{$a}) {
	    return $a;
	} elsif ($s{$b}) {
	    return $b;
	}
    }

    # now try qualified vs. non-qualified
    unless ($a =~ /ₓ/ || $b =~ /ₓ/) {
	if ($a =~ /\(.*?\)$/ && $b !~ /\(.*?\)$/) {
	    return $b;
	} elsif ($b =~ /\(.*?\)/) {
	    return $b;
	}
    }

    # how about ŋ vs. g
    if ($a =~ /ŋ/ && $b !~ /ŋ/) {
	return $a;
    } elsif ($b =~ /ŋ/) {
	return $b;
    }

    # cdot or degree preferred
    if ($a =~ /[·°]/ && $b !~ /[·°]/) {
	return $a;
    } elsif ($b =~ /[·°]/) {
	return $b;
    }

    # + in det is prefered to non-+
    if ($a =~ /\{\+/ && $b !~ /\{\+/) {
	return $a;
    } elsif ($b =~ /\{\+/) {
	return $b;
    }

    my $cf = $cfgw; $cf =~ s/\s.*$//;
    if ($a =~ /$cf/ && $b !~ /$cf/) {
	return $a;
    } elsif ($b =~ /$cf/) {
	return $b;
    }

    my $acf = base_to_cf($a);
    my $bcf = base_to_cf($b);
    if ($acf eq $cf) {
	return $a;
    } elsif ($bcf eq $cf) {
	return $b;
    } elsif ($cf =~ /$acf/ && $cf !~ /$bcf/) {
	return $a;
    } elsif ($cf =~ /$bcf/) {
	return $b;
    }
    
    undef;
}

sub base_to_cf {
    my $x = shift;
    $x =~ s/\{.*?\}//g;
    $x =~ s/\(.*\)//g;
    $x =~ s/ₓ.*$//;
    $x =~ tr/-₀₁₂₃₄₅₆₇₈₉ₓ//d;
    $x;
}

sub bases_stats {
    my($cfgw,$base) = @_;
    ++${$stats{$cfgw}}{$base};
}

sub bases_process {
    my %bd = @_;
    my @log_errors = bases_log_errors($bd{'line'});
    my %b = bases_hash($bd{'data'}, $bd{'compound'});
#    open(D,'>bases.dump');
#    use Data::Dumper;
#    print D Dumper \%stats;
#    print D Dumper \%b;
#    close(D);
    bases_fix(\%bd,\%b,@log_errors);
    %stats = ();
    bases_serialize(%b);
}

sub bases_serialize {
    my %b = @_;
    my $res = '';
    foreach my $b (sort keys %b) {
	next if $b =~ /\#/;
	$res .= '; ' if $res;
	$res .= $b;
	if (defined $b{"$b#alt"}) {
	    next if $b =~ /\#/;
	    $res .= ' (';
	    $res .= join(', ', sort keys %{$b{"$b#alt"}});
	    $res .= ')';
	}
    }
    if (!length($res)) {
	warn "bases_serialize: empty result from ", Dumper \%b;
    }
    $res;
}

sub bases_fix {
    my($bdref,$bref,@e) = @_;
    foreach my $e (@e) {
	if ($e =~ /^primary bases '(.*?)' and '(.*?)' are the same$/) {
	    bases_same_primary($bdref,$bref,$1,$2);
	} elsif ($e =~ /^compound (\S+) should be (\S+)\s*$/) {
	    bases_sign_should($bdref,$bref,$1,$2);
	} elsif ($e =~ /^sign name '(\S+)' should be '(\S+)'\s*$/) {
	    bases_sign_should($bdref,$bref,$1,$2);
	}
    }
}

sub bases_same_primary {
    my($bdref,$bref,$a,$b) = @_;
    my $pref = bases_prefer($$bdref{'cfgw'},$a,$b);
    if ($pref) {
	my $fixme = ($a eq $pref) ? $b : $a;
	warn("fixing $fixme to belong to $pref\n");
	$fixes{$fixme} = $pref;
	# save alternates of $fixme
	my @alt = ($fixme);
	if (defined $$bref{"$fixme#alt"}) {
	    push @alt, keys %{$$bref{"$fixme#alt"}};
	}
	# delete vbases{$fixme}
#	warn "deleting bref $fixme\n";
	delete $$bref{$fixme};
	# add $fixme and all its alternates to $pref
	# use a hash to do the adds so they get uniq'd at the same time
	foreach my $a (@alt) {
	    ++${$$bref{"$pref#alt"}}{$a};
	}
    } else {
	my $ai = ($stats{$$bdref{'cfgw'}} ? ${$stats{$$bdref{'cfgw'}}}{$a} : 0);
	my $bi = ($stats{$$bdref{'cfgw'}} ? ${$stats{$$bdref{'cfgw'}}}{$b} : 0);
	my $cf = $$bdref{'cfgw'}; $cf =~ s/\s.*$//;
	pp_warn("$$bdref{'cfgw'}: can't fix 'same primary $a [$ai] and $b [$bi]'");
    }
}

sub bases_sign_should {
    my($bdref,$bref,$from,$to) = @_;
    my $fromQ = quotemeta($from);
    foreach my $k (keys %$bref) {
	if (defined $$bref{"$k#alt"}) {
	    my @a = keys %{$$bref{"$k#alt"}};
	    my %new_alt = ();
	    foreach my $a (@a) {
		my $p = ${$$bref{"$k#alt"}}{$a};
		my $orig_a = $a;
		my $orig_p = $p;
		if ($a =~ s/$fromQ($bound)/$to$1/) {
#		    warn "fixing $from to $to in alt $orig_a\n";
		    fix_form_bases($bdref,$from,$fromQ,$to);
		}
		if ($p =~ s/$fromQ($bound)/$to$1/) {
#		    warn "fixing $from to $to in pri $orig_p ref'd from alt $orig_a\n";
		    fix_form_bases($bdref,$from,$fromQ,$to);
		}
		$new_alt{$a} = $p;
	    }
	    %{$$bref{"$k#alt"}} = %new_alt;
	}
    }
    foreach my $k (keys %$bref) {
	next if $k =~ /#/;
	if ($k =~ m/$fromQ/) {
	    my $new_k = $k;
	    if ($new_k =~ s/$fromQ($bound)/$to$1/) {
		warn "fixing $from to $to in $k\n";
		fix_form_bases($bdref,$from,$fromQ,$to);
	    } else {
		warn "failed to fix $fromQ in $new_k\n";
	    }
#	    warn "deleting bref k $k\n";
	    delete $$bref{$k};
	    if (defined $$bref{"$k#alt"}) {
		$$bref{$new_k} = $$bref{"$k#alt"};
	    } else {
		++$$bref{$new_k};
	    }
	}
    }
}

sub fix_form_bases {
    my($bd,$from,$fromQ,$to) = @_;
    if ($$bd{'form_i'}) {
	my @from_i = @{$$bd{'form_i'}};
	foreach my $fi (@from_i) {
	    my $l = ${$$bd{'cbd'}}[$fi];
#	    warn "trying to fix $fromQ in '$l'\n";
	    my ($prebase,$base,$postbase) = ($l =~ m#^(\@form\s+\S+\s+.*?)/(\S+)\s+(.*)\s*$#);
	    unless ($base =~ s#(^|$bound)$fromQ($bound|$)#$1$to$2#g) {
		warn "no luck replacing $fromQ in $l\n"
		    if $base =~ /$fromQ/;
	    } else {
		warn "ffb: fixing $from to $to in \@form\n";
		${$$bd{'cbd'}}[$fi] = "$prebase/$base $postbase";
	    }
#	    if (${$$bd{'cbd'}}[$fi] =~ s/($bound)$fromQ($bound)/$1$to$2/g) {
#		warn "ffb: fixing $from to $to in \@form\n";
#	    } else {
#		warn "no luck with $l\n"
#		    if $l =~ /$fromQ/;
#	    }
	}
    }
}

1;
