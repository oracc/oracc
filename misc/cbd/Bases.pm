package ORACC::CBD::Bases;
require Exporter;
@ISA=qw/Exporter/;

@EXPORT = qw/bases_align bases_hash bases_init bases_log bases_collect
    bases_log_errors bases_fixes bases_process bases_stats bases_term
    bases_merge bases_string bases_serialize bases_fix_base bases_sigs 
    bases_prefer bases_primaries/;

use warnings; use strict; use open 'utf8'; use utf8;

use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;
my $acd_rx = $ORACC::CBD::acd_rx;
use Data::Dumper;

$ORACC::CBD::Bases::ignore_empty_serializations = 0;
$ORACC::CBD::Bases::serialize_ref = 0;

my $base_trace = 0;
my $bound = '(?:[-\|.{}()/ ]|$)';
my %fixes = ();
my %homographs = ();
my %log_errors = ();
my %stats = ();
my $use_map_fh;

my %base_cpd_flags = ();

my $map_fh = undef;

my $p_entry = '';

sub bases_primaries {
    my $bases = shift; chomp $bases; $bases =~ s/^\@bases\S*\s+//;
    my @b = split(/;\s+/, $bases);
    my @new_b = ();
    foreach my $b (@b) {
	$b =~ s/\s.*$//;
	push @new_b, $b;
    }
    @new_b;
}

sub bases_sigs {
    my ($args, $x) = @_;
    my %x = %$x;
    my $l = ORACC::CBD::Util::lang();
    unless ($$args{'stdout'}) {
	open(B,">01bld/$l/base-sigs.tab"); select B;
    }
    foreach my $c (sort keys %x) {
	my @b = @{$x{$c}};
	foreach my $b (@b) {
	    print "$c\t$$b[0]\t$$b[1]\n";
	}
    }
    unless ($$args{'stdout'}) {
	close(B);
	select STDOUT;
    }
}

sub bases_homographs_init {
    my($proj,$lang) = @_;
    return unless $proj && $lang;
    return if $homographs{"##$proj\:$lang"}++;
    my $tab = "$ENV{'ORACC_BUILDS'}/$proj/01bld/$lang/base-sigs.tab";
    if (open(T,$tab)) {
	while (<T>) {
	    chomp;
	    my($c,$b,$s) = split(/\t/,$_);
	    push @{$homographs{"$proj\:$lang\:$s"}}, [ $c , $b ];
	    ${$homographs{"$proj\:$lang\:$c"}{$s}} = $b;
	}
	close T;
	return 1;
    } else {
	return 0;
    }
}

sub bases_homographs_dump {
    use Data::Dumper;
    open(B,'>bases_homographs.dump') || die;
    print B Dumper \%homographs;
    close(B);
}

sub bases_homographs_lookup {
    my ($proj,$lang,$key) = @_;
    my @ret = ();
    if ($key =~ /^o\d/) {
	my $k = "$proj\:$lang\:$key";
	if ($homographs{$k}) {
	    @ret = @{$homographs{$k}};
	}
    } else {
	$key =~ s/\s*\[/ [/;
	$key =~ s/\]\s*/] /;
	my $k = "$proj\:$lang\:$key";
	if ($homographs{$k}) {
	    foreach my $s (keys %{$homographs{$k}}) {
		my $b = ${$homographs{$k}}{$s};
		push @ret, [ $s , $b ];
	    }
	}
    }
    @ret;
}

sub bases_fix_base {
    my($b,$f,$p) = @_;
    my $orig_b = $b;
#    foreach my $f (@f) {
	my $fQ = quotemeta($f);
	unless ($b =~ s#(^|$bound)$fQ($bound|$)#$1$p$2#g) {
	    warn "no $f in $b\n" if $base_trace;
	} else {
	    warn "---\nin: ${orig_b}ou: $b---\n" if $base_trace;
	}
#    }
}

sub bases_tab {
    my %base_bases = ();
    my $b_ln = 0;
    my $cpd = 0;
    my $e = '';
    my $e_ln = 0;
    my $ln = 0;
    my $file = pp_file();
    foreach (@_) {
	++$ln;
	if (/^\@bases/) {
	    # warn "bases_tab calling bases_hash\n";
	    $b_ln = $ln;
	    my %b = bases_hash($_,$cpd);
	    print "$file\t$e_ln\t$b_ln\t";
	    print "$e\t", join("\t", sort grep(!/\#/, keys %b)), "\n";
	} elsif (/^\@parts/) {
	    $cpd = 1;
	} elsif (/^$acd_rx?\@entry\S*\s*(.*?)\s*$/) {
	    $e = $1;
	    $e =~ s/\s+\[/[/;
	    $e =~ s/\]\s/]/;
	    $cpd = 0;
	    $e_ln = $ln;
	}
    }
}

sub bases_fixes {
    my %tmp = %fixes;
    %fixes = ();
    %tmp;
}

sub bases_align {
    my($args,$base_cbd,$cbd,$xmap_fh) = @_;

    my @base_cbd = @$base_cbd;
    my @cbd = @$cbd;

    my %bases = ();

    if ($xmap_fh) {
	$map_fh = $xmap_fh;
	$use_map_fh = 1;
    } else {
	$use_map_fh = 0;
    }

    my %base_bases = bases_collect(@base_cbd);
    my $curr_entry = '';

    for (my $i = 0; $i <= $#cbd; ++$i) {
	if ($cbd[$i] =~ /^$acd_rx?\@entry\S*\s+(.*?)\s*$/) {
	    $curr_entry = $1;
	    $p_entry = $curr_entry;
	    $p_entry =~ s/\s*\[(.*?)\]\s*/[$1]/;
	} elsif ($cbd[$i] =~ /^\@bases/) {
	    my $base_i = $base_bases{$curr_entry};
	    if ($base_i) {
		warn "aligning:\n\t$cbd[$i]\ninto\t$base_cbd[$base_i]\n" if $base_trace;
		pp_line($i+1);
		my $b = bases_merge($base_cbd[$base_i], $cbd[$i], $base_cpd_flags{$curr_entry}, $base_i, $curr_entry);
		if ($$b{'#map'} || $$b{'#new'}) {
		    if ($$args{'apply'}) {
			$bases{$curr_entry} = $b;
		    } else {
			if ($$b{'#map'}) {
			    my %bmap = %{$$b{'#map'}};
			    foreach my $b (keys %bmap) {
				if ($use_map_fh) {
				    print $map_fh "map base $p_entry => $b ~ $bmap{$b}\n";
				} else {
				    pp_warn("$p_entry: map base $b ~ $bmap{$b}");
				}
			    }
			}
			$base_cbd[$base_i] = bases_string($b);
			warn "=>$base_cbd[$base_i]\n" if $base_trace;
			if ($$b{'#new'}) {
			    if ($use_map_fh) {
				print $map_fh "new bases $p_entry => \@bases $base_cbd[$base_i]\n";
			    } # else already reported in bases_merge
			}
		    }
		}
	    }
	}
    }
    return %bases;
}

sub bases_collect {
    my @cbd = @_;
    my $curr_entry = '';
    my %b = ();
    for (my $i = 0; $i <= $#cbd; ++$i) {
	if ($cbd[$i] =~ /^$acd_rx?\@entry\S*\s+(.*?)\s*$/) {
	    $curr_entry = $1;
	    my $cf = $curr_entry;
	    $cf =~ s/\s+\[.*$//;
	    if ($cf =~ /\s/) {
		$base_cpd_flags{$curr_entry} = 1;
	    }
	} elsif ($cbd[$i] =~ /^\@bases/) {
	    $b{$curr_entry} = $i;
	}
    }
    %b;
}

sub bases_init {
    my $args = shift;
    my $bases_outfile = ORACC::CBD::Util::lang().'.map';
    if (-d '01tmp') {
	$bases_outfile = "01tmp/$bases_outfile";
    }
    ORACC::SL::BaseC::init();
    open(MAP_FH, ">>$bases_outfile");
    $map_fh = \*MAP_FH;
}

sub bases_term {
    close(MAP_FH);
    ORACC::SL::BaseC::term();
}

# This routine assumes that the bases conform to the constraints enforced by cbdpp
sub bases_merge {
    my($b1,$b2,$cpd,$base_i,$curr) = @_;

    $p_entry = $curr || '';

    #    warn "bases_merge calling bases_hash\n";
    
    my %h1 = bases_hash($b1,$cpd, $base_i);
    
    my %h2 = bases_hash($b2,$cpd, pp_line());

    $h1{'#new'} = 0;

    foreach my $p2 (keys %h2) {
	next if $p2 =~ /#/;
	warn "processing incoming base $p2\n" if $base_trace;
 	if ($h1{$p2}) { # primary in b2 is already in b1
	    warn "found $p2 as primary in both\n" if $base_trace;
 	    $h1{"$p2#alt"} = merge_alts($p2, $h1{"$p2#alt"}, $h2{"$p2#alt"});
	    ++${$h1{'#ref'}}{$p2};
 	} elsif (${$h1{"#alt"}}{$p2}) { # primary in b2 is an alt in b1
 	    my $p1 = ${$h1{"#alt"}{$p2}};
 	    warn "found $p2 as alternate to $p1 in base\n" if $base_trace;
 	    $h1{"$p1#alt"} = merge_alts($p1, $h1{"$p1#alt"}, $h2{"$p2#alt"});
 	    ${$h1{'#map'}}{$p2} = $p1;
	    ++${$h1{'#ref'}}{$p1};
 	} else { # primary in b2 isn't known in b1
	    my $p2sig = ORACC::SL::BaseC::check(undef,$p2,1);
	    my $p1 = '';
	    if (($p1 = ${$h1{'#sigs'}}{$p2sig})) {
		# This is a new alternate transliteration of $p1
		warn "found $p2 as new alternate to $p1 in base\n" if $base_trace;
		
		# register it in the global alt array
		${${$h1{'#alt'}}{$p2}} = $p1;
		
		# register it in the pri-local alt array
		++${$h1{"$p1#alt"}}{$p2};

		# $h1{"$p1#alt"} = merge_alts($p1, $h1{"$p1#alt"}, $h2{"$p2#alt"});
		${$h1{'#map'}}{$p2} = $p1;

#		my $p1 = ${$h1{"#alt"}{$p2}};
		++${$h1{'#ref'}}{$p1};

	    } else {
		# This is a new primary transliteration
		warn "incoming $p2 is new primary\n" if $base_trace;
		if ($use_map_fh) {
		    print $map_fh pp_file().':'.pp_line().": add base $p_entry => $p2\n";
		} else {
		    pp_notice("$p_entry: new base $p2");
		}
#		warn Dumper \%h1;
		$h1{$p2} = $h2{$p2};
		if ($h2{"$p2#alt"}) {
		    $h1{"$p2#alt"} = $h2{"$p2#alt"};
		}
		++${$h1{'#ref'}}{$p2};
	    }
	    $h1{'#new'} = 1;
 	}
    }
# print STDERR "merged bases => ", Dumper \%h1;
    return { %h1 };
}

sub merge_alts {
    my($pri, $a1,$a2) = @_;
    if ($a1 && $a2) {
	foreach my $k (keys %$a2) {
	    $$a1{$k} = 1 unless $k eq $pri;
	}
	return $a1;
    } elsif ($a1) {
	return $a1;
    } else {
	return $a2;
    }
}

sub bases_string {
    my $href = shift;
    my %h = %$href;
    my $s = '';
    foreach my $p (sort keys %h) {
	next if $p =~ /#/;
	$s .= '; ' if $s;
	$s .= $p;
	if ($h{"$p#alt"}) {
	    my $as = '';
	    foreach my $a (sort keys %{$h{"$p#alt"}}) {
		next if $a =~ /#/;
		$as .= ', ' if $as;
		$as .= $a;
	    }
	    if ($as) {
		$s .= " ($as)";
	    }
	}
    }
    $s;
}

sub bases_hash {
    my($arg,$is_compound,$line) = @_;
    my $saved_line = pp_line();
#    warn 'bases_hash caller: ', join(':', caller()), "\n";
    if (defined $line) {
#	warn "bases_hash: resetting line to $line\n";
	pp_line($line);
    }
    if ($arg =~ s/;\s*$//) {
	pp_warn("bases entry ends with semi-colon--please remove it");
    }
    $arg =~ s/^\@bases\s+//;
    $arg =~ s/\s*$//;
    if ($arg =~ s/^\s*;//) {
	pp_warn("bases entry starts with semi-colon--please remove it");
    }
    $arg =~ s/^\s*$//;
    
    my @bits = split(/;\s+/, $arg);

    my $alt = '';
    my $stem = '';
    my $pri = '';
    my %vbases = (); # this one is just for validation of the current @bases field
    my $pricode = 0;

    foreach my $b (@bits) {
	warn "processing bit $b\n" if $base_trace;
	if ($b =~ s/^\*(\S+)\s+//) {
	    $stem = $1;
	} elsif ($b =~ /^\*/) {
	    $b =~ s/^\*\s*//;
	    pp_warn("misplaced '*' in \@bases");
	}
	if ($b =~ /\s+\(/) {
	    warn "primary with alternates $b\n" if $base_trace;
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
			${$vbases{'#sigs'}}{ ORACC::SL::BaseC::check(undef,$pri, 1) } = $pri;
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
	    warn "primary $b with no alternates\n" if $base_trace;
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
		    ${$vbases{'#sigs'}}{ ORACC::SL::BaseC::check(undef, $pri, 1) } = $pri;
		}
	    }
	}
    }
#    warn "bases $arg => ", Dumper \%vbases;
    ORACC::CBD::Validate::pp_sl_messages();
    pp_line($saved_line);
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

    # try only-difference-is-plus first
    my $asansplus = $a;
    my $bsansplus = $b;
    $asansplus =~ s/\{\+*//g;
    $bsansplus =~ s/\{\+*//g;

    if ($asansplus eq $bsansplus) {
	if ($a =~ /\{\+/) {
	    return $a;
	} else {
	    return $b;
	}
    }
    
    # cdot or degree preferred
    if ($a =~ /[·°]/ && $b !~ /[·°]/) {
	return $a;
    } elsif ($b =~ /[·°]/) {
	return $b;
    }

    # now try qualified vs. non-qualified
    unless ($a =~ /ₓ/ || $b =~ /ₓ/) {
	if ($a =~ /\(.*?\)$/ && $b !~ /\(.*?\)$/) {
	    return $b;
	} elsif ($b =~ /\(.*?\)/) {
	    return $a;
	}
    }

    # try stats 
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

    # how about ŋ vs. g
    if ($a =~ /ŋ/ && $b !~ /ŋ/) {
	return $a;
    } elsif ($b =~ /ŋ/) {
	return $b;
    }

    # + in det is prefered to non-+
    if ($a =~ /\{\+/ && $b !~ /\{\+/) {
	return $a;
    } elsif ($b =~ /\{\+/) {
	return $b;
    }

    my $cf = $cfgw; $cf =~ s/\s.*$//; $cf = "\L$cf";
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
    $x =~ "\L$x";
    $x;
}

sub bases_stats {
    my($cfgw,$base) = @_;
    ++${$stats{$cfgw}}{$base};
}

sub bases_process {
    my %bd = @_;
    my @log_errors = bases_log_errors($bd{'line'});
#    warn "bases_process calling bases_hash\n";
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
	next unless $b; ## FIXME: should issue a warning
	if (!$ORACC::CBD::Bases::serialize_ref || ${$b{'#ref'}}{$b}) {
	    $res .= '; ' if $res;
	    $res .= $b;
	    if (defined $b{"$b#alt"}) {
		next if $b =~ /\#/;
		$res .= ' (';
		$res .= join(', ', sort keys %{$b{"$b#alt"}});
		$res .= ')';
	    }
	}
    }
    $res;
}

sub bases_fix {
    my($bdref,$bref,@e) = @_;
    foreach my $e (@e) {
	if ($e =~ /^primary bases '(.*?)' and '(.*?)' are the same/) {
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
#	    warn "trying to fix $fromQ to $to in '$l'\n";
	    my ($prebase,$base,$postbase) = ($l =~ m#^(\@form\s+\S+\s+.*?)/(\S+)\s+(.*)\s*$#);
	    unless ($base =~ s#(^|$bound)$fromQ($bound|$)#$1$to$2#g) {
		warn "no luck replacing $fromQ in $l\n"
		    if $base =~ /$fromQ/;
	    } else {
		warn "ffb: fixing $from to $to in \@form\n";
		${$$bd{'cbd'}}[$fi] = "$prebase/$base $postbase";
	    }
	}
    }
}

1;
