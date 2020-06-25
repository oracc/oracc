package ORACC::SMA::Base;
use warnings; use strict; use utf8; use open 'utf8';
use lib '@@ORACC@@/lib';
use ORACC::SL::BaseC;
our $VERSION = '2.0';

use ORACC::SMA::NSF;
use ORACC::SMA::VPR;
use ORACC::SMA::VSF;
use ORACC::SMA::ISF;
use ORACC::SMA::Display;
#use ORACC::SMA::PSL;
use ORACC::SMA::StripVPR;
use Data::Dumper;

binmode STDERR, ':utf8';
binmode STDIN, ':utf8';
#binmode STDOUT, ':utf8';

$ORACC::SMA::no_external_bases = 0;
$ORACC::SMA::try_only_zero = 0;
$ORACC::SMA::period = '';
$ORACC::SMA::verbose = 0;

my $sma_debug = 0;
my %bases = ();
my %basemap = ();
my %nodet_bases = ();
my %no_pos_lemma = ();
my %redup_bases = ();
my %stems = ();

sub
has_nodet_bases {
    defined $nodet_bases{$_[0]};
}

sub
load {
    my($proj) = @_;
    my $glo = "@@ORACC@@/pub/epsd2/sux.glo";
    my $qpn = "@@ORACC@@/pub/epsd2/qpn.glo";
    my $sal = "@@ORACC@@/pub/epsd2/sux-x-emesal.glo";
    $sma_debug = $ORACC::SMA::verbose;
    load_glo($proj,$glo);
    load_glo($proj,$qpn);
    load_glo($proj,$sal);
    ORACC::SL::BaseC::init(1);
}

sub
load_glo {
    my($proj,$glo) = @_;
    open(GLO, $glo) || die "open failed on $glo";
    my $lemma = undef;
    my $lemma_no_pos = undef;
    my $bstar = '';
    my $in_bases = 0;
    my @b = ();
    while (<GLO>) {
	if (s/^\@entry\s+//) {
	    my($cf,$gw,$pos) = (/^(.*?)\s+\[(.*?)\]\s+(\S+)\s*$/);
	    if ($cf =~ /\s/) {
		$lemma = undef;
		next;
	    }
	    $lemma = "$cf\[$gw\]$pos";
	    $lemma_no_pos = "$cf\[$gw\]";
#	    push @{$no_pos_lemma{"$cf\[$gw\]"}}, $lemma;
	    $bstar = 0;
	    @b = ();
#	    warn("$lemma\n") if $sma_debug;
	} elsif ($lemma && (s/^\@bases\s+// || ($in_bases && s/^\s+//))) {
	    chomp;
	    foreach my $b (split(/;(?:\s+|$)/,$_)) {
		if ($b =~ s/^\*(\S+)\s*//) {
		    $bstar = $1;
		    next unless $b;
		}
#		warn "$lemma \*$bstar $b\n" if $bstar;
		my $b_alt = undef;
		if ($b =~ s/\s+\((.*)\)$//) {
		    $b_alt = $1;
	        }
		push @b, $b;
		$stems{$lemma,$b} = $bstar;
		if ($b_alt) {
		    foreach my $a (split(/,\s+/,$b_alt)) {
			push @b, $a;
			$stems{$lemma,$a} = $bstar;
			$basemap{"$lemma/$a"} = $b;
		    }
		}
	    }
	    push @{$bases{$lemma}}, @b;
	    push @{$bases{$lemma_no_pos}}, @b;
	    my @b_nodet = @b;
	    map { s/\{.*?\}//g; $_ } @b_nodet;
	    if ($#b_nodet >= 0) {
#		warn "@b\n";
		my @b_undet 
		    = map { s/\{(.*)\}/-$1-/g; s/--/-/g; s/^-//; s/-$//; $_ } @b;
		my %u = ();
		@u{@b_nodet} = ();
		@u{@b_undet} = ();
		push @{$nodet_bases{$lemma}}, keys %u;
	    }
	    ++$in_bases;
	} else {
	    $in_bases = 0;
	}
    }
    close(GLO);
#    use Data::Dumper; open(B,'>bases.log'); print B Dumper \%bases; close(B);
}

sub
unload {
    ORACC::SL::BaseC::term();
}

sub
parse {
    my ($form,$lemma,$try_number,$arg_base) = @_;
    my @parses = ();
    my @bases = ();
    my %found_bases = ();
    my $disamb = '';

    if ($form =~ s/\\(.*)$//) {
	$disamb = $1;
    }

    if ($sma_debug) {
	$arg_base = '' unless $arg_base;
	warn "Base::parse try #$try_number entered with $form=$lemma and arg_base=$arg_base\n";
    }

    if (!$arg_base && !defined $bases{$lemma}) {
#	warn("lemma '$lemma' has no known bases\n") 
#	    unless $try_number == 6;
	return undef; # in xli mode this is not reached
    } elsif ($arg_base && $ORACC::SMA::no_external_bases) {
	@bases = ($arg_base);
    } else {
	if ($try_number == 0) {
	    @bases = ($arg_base) if $arg_base;
	    if ($bases{$lemma}) {
		push @bases, @{$bases{$lemma}};
	    }
	} elsif ($try_number == 1) {
	    # we are trying sans determinatives
	    if (defined $nodet_bases{$lemma}) {
		@bases = @{$nodet_bases{$lemma}};
	    } elsif ($bases{$lemma}) {
		@bases = @{$bases{$lemma}};
	    } else {
		@bases = ();
	    }
	} elsif ($try_number == 2) {
	    # we are trying reduplication
	    my $verbal = ($lemma =~ /\]V/);
	    foreach my $ub (@{$bases{$lemma}}) {
		my $ub_nodet = '';
		if ($ub =~ /\{/) {
		    $ub_nodet = $ub;
		    $ub_nodet =~ s/\{.*?\}//;
		}
		push @bases, "$ub-$ub";
		push @bases, "$ub_nodet-$ub_nodet" if $ub_nodet;
		if ($verbal) {
		    push @bases, "$ub-$ub-$ub", "$ub-$ub-$ub-$ub";
		}
	    }
	} elsif ($try_number == 3) {
	    # we are trying without sign name qualifiers
	    my @bcopy = @{$bases{$lemma}};
	    foreach my $ub (@bcopy) {
		$ub =~ s/\(.*?\)//g;
		push @bases, $ub;
	    }
	} elsif ($try_number == 7) {
	    my $res = find_same_tlit_base($form,@{$bases{$lemma}});
	    if ($res) {
		@bases = @{$bases{$lemma}};
		$form = $res;
	    } else {
		return ();
	    }
	} elsif ($arg_base) {
	    push @bases, $arg_base;
	}
    }

    @bases = uniq(@bases);
    print STDERR "bases for lemma `$lemma': @bases\n" if $ORACC::SMA::verbose;

    my ($cf,$gw,$POS) = ('','','');
    if ($lemma) {
	($cf,$gw,$POS) = ($lemma =~ /^(.*?)\[(.*?)\](\S+)$/);
    }

    foreach my $b (sort {&basecmp} @bases) {
	my %p = ();
	my $obase = $b;
	my $backup = ($b =~ tr/·//d);
	my $prefbase = ($b =~ tr/°//d);
#	if ($form =~ /(?:^|(\S+?)-)($b(?:-$b)*)(?:-(\S+))?$/) {
	my $qb = quotemeta($b);
	warn "matching quoted base `$qb'\n" if $ORACC::SMA::verbose;
	if ($form =~ /(?:^|(\S+?)-)($qb)(?:-(\S+))?$/) {
	    my($pre,$bas,$post) = ($1||'',$2||'',$3||'');

	    if (exists($ORACC::SMA::VPR::vpm{$b})) {
		# try to compensate for ga-am₃-ma-ga type forms
		if ($post =~ /(?:^|(\S+?)-)($qb)(?:-(\S+))?$/) {
		    my($npre,$nbas,$npost) = ($1||'',$2||'',$3||'');
		    $pre = "$bas-$npre";
		    $bas = $nbas;
		    $post = $npost;
		}	    
	    }
	    
	    if ($pre && ($POS && $POS !~ /^(?:V|AJ)/)) {
		print STDERR "INIT: rejecting base `$qb' because $POS != 'V/AJ'\n"
		    if $sma_debug;
		next;
	    }
	    print STDERR "INIT: $form => '$pre' / '$bas' / '$post'\n" 
		if $sma_debug;
	    if ($bas) {
		if ($basemap{"$lemma/$obase"}) {
		    ++$found_bases{$basemap{"$lemma/$obase"}};
		} else {
		    ++$found_bases{$obase};
		}
		if ($backup) {
		    if ($bas =~ s/-([^-]+)$//) {
			if ($post) {
			    $post = "$1-$post";
			} else {
			    $post = $1;
			}
			$p{'backup'} = 1;
		    }
		}
		# This is too clever: parser can't handle am3-m it needs am3-me
#		if ($prefbase) {
#		    my($pref,$base) = ($obase =~ /^(.*?)°(.*)$/);
#		    $pre .= "-$pref" unless $pre =~ /$pref$/;
#		}
		if (!$lemma || ($POS && $POS =~ /^(?:V|AJ)/)) {
		    my $have_vpr = 0;
		    my $have_vsf = 0;
		    my @nsf_g = ();
		    my $nsf_cf = '';
		    if ($pre) {
			warn("vpr_g = `$pre'\n") if $ORACC::SMA::verbose;
			my @vpr_g = split(/-/,$pre);
			if (is_vpr(@vpr_g,'')) {
			    my $vpr_ref = get_last_vpr();
			    my $vpr = $$vpr_ref{'vpr'};
			    print STDERR "$pre => ", 
			    ORACC::SMA::Display::mcat($vpr), "\n"
				if $sma_debug;
			    if ($$vpr_ref{'post_vpr'} > $#vpr_g) {
				$have_vpr = 1;
				$p{'vpr'} = $vpr_ref;
			    } else {
				next; # junk between VPR and BASE
			    }
			} else { # if there is pre but no vpr it doesn't parse
			    next;
			}
		    }
		    if ($post) {
			my @vsf_g = split(/-/,$post);
			if (($have_vsf = is_vsf($cf,$bas,$have_vpr,@vsf_g,''))) {
			    my $vsf_ref = get_last_vsf();
			    my $vsf = $$vsf_ref{'vsf'};
			    $p{'vsf'} = $vsf_ref;
			    print STDERR "VSF: $post => ", 
			    ORACC::SMA::Display::mcat($vsf), "\n"
				if $sma_debug;
			    if ($$vsf_ref{'post_vsf'} <= $#vsf_g) {
				if ($$vsf_ref{'post_vsf'} == $#vsf_g) {
				    @nsf_g = ($vsf_g[$#vsf_g]);
				} else {
				    @nsf_g = @vsf_g[$$vsf_ref{'post_vsf'},$#vsf_g];
				}
				if ($$vsf_ref{'post_vsf'}) {
				    $nsf_cf = $vsf_g[$$vsf_ref{'post_vsf'}-1];
				} else {
				    $nsf_cf = $cf;
				}
			    } else {
				goto ok;
			    }
			} else {
			    print STDERR "VSF: is_vsf failed\n" if $sma_debug;
			    @nsf_g = @vsf_g;
			    $nsf_cf = $cf;
			}

			if ($#nsf_g >= 0 && is_nsf($nsf_cf,$bas,@nsf_g)) {
			    my $nsf_ref = get_last_nsf();
			    if ($$nsf_ref{'post_nsf'} > $#nsf_g) {
				my $nsf = $$nsf_ref{'nsf'};
				$p{'nsf'} = $nsf_ref;
				goto ok;
			    }
			}

			my @isf_g = split(/-/,$post);
			if (!$have_vpr && $#isf_g >= 0) {
			    if (is_isf($cf,@isf_g)) {
				my $isf_ref = get_last_isf();
				if ($$isf_ref{'post_isf'} > $#isf_g) {
				    my $isf = $$isf_ref{'isf'};
				    $p{'isf'} = $isf_ref;
				    delete $p{'vsf'};
				    delete $p{'nsf'};
				    goto ok;
				}
			    }
			}
			next;
		    }
		} else {
		    if ($post) {
			my @nsf_g = split(/-/,$post);
			if (is_nsf($cf,$bas,@nsf_g)) {
			    my $nsf_ref = get_last_nsf();
			    if ($$nsf_ref{'post_nsf'} > $#nsf_g) {
				my $nsf = $$nsf_ref{'nsf'};
				$p{'nsf'} = $nsf_ref;
				goto ok;
			    }
			}
			next;
		    }
		}
	      ok:
		$p{'stem'} = $stems{$lemma,$obase};
		push @parses, { %p };
		last;
	    }
	}
    }
    my @found_bases = sort {&basecmp} keys %found_bases;
    if ($sma_debug) {
	print STDERR "Dump of \@found_bases:\n";
	print STDERR Dumper(\@found_bases);
	print STDERR "Dump of \@parses:\n";
	print STDERR Dumper(\@parses);
    }
    (\@found_bases, @parses);
}

sub
basecmp {
    my $alen = ($a =~ tr/{-/{-/);
    my $blen = ($b =~ tr/{-/{-/);
    $blen <=> $alen;
}

sub
find_same_tlit_base {
    my($test,@bases) = @_;
    my @longest = ();
    my $ntries = 0;
    my $prefix = '';
  try_again:
    foreach my $b (@bases) {
	my @res = try_one_base($test,$b);
	if ($#res > $#longest) {
	    @longest = @res;
	}
    }
    if ($#longest >= 0) {
	$prefix = '';
	return $prefix.join('-',@longest);
    } else {
	if (!$ntries) {
	    ++$ntries;
	    my $xtest = strip_vpr(split(/-/,$test));;
	    if ($xtest) {
		my $qxtest = quotemeta($xtest);
		if ($test =~ /^(.*?)$qxtest$/) {
		    $prefix = $1;
		}
		$test = $xtest;
		goto try_again;
	    }
	}
	return undef;
    }
}
sub
try_one_base {
    my($test,$test_base) = @_;
    my @bits = split(/-/,$test_base);
    while ($#bits >= 0) {
	my $base = join('-',@bits);
	my $res = ORACC::SL::BaseC::same_tlit("SMA",$test,$base);
	if ($res) {
	    return @bits;
	} else {
	    pop @bits;
	    $base = undef;
	}
    }
    return ();
}
sub
uniq {
    my %seen = ();
    my @n = ();
    foreach my $a (@_) {
	push @n, $a unless $seen{$a}++;
    }
    return @n;
}

sub
get_info {
    my %p = %{$_[0]};
    my ($cont,$morph) = ('0','');
    if ($p{'vpr'}) {
	my $vpr_ref = $p{'vpr'};
	$morph .= ORACC::SMA::Display::mcat($$vpr_ref{'vpr'},$$vpr_ref{'graph'});
	$morph .= ':';
    }
    $morph .= '~';
    if ($p{'vsf'}) {
	my $vsf_ref = $p{'vsf'};
	$cont = ORACC::SMA::Display::auslaut($$vsf_ref{'vsf'}) || '0';
	if ($cont =~ s/\*=/=/) {
	    $morph .= ";(a).";
	} else {
	    $morph .= ';';
	}
	$morph .= ORACC::SMA::Display::mcat($$vsf_ref{'vsf'});
    }
    if ($p{'nsf'}) {
	my $nsf_ref = $p{'nsf'};
	$morph .= ',';
	$morph .= ORACC::SMA::Display::mcat($$nsf_ref{'nsf'});
	$cont = ORACC::SMA::Display::auslaut($$nsf_ref{'nsf'}) || '0';
    }
    if ($p{'isf'}) {
	my $isf_ref = $p{'isf'};
	$morph .= '!';
	$morph .= ORACC::SMA::Display::mcat($$isf_ref{'isf'});
	$cont = ORACC::SMA::Display::auslaut($$isf_ref{'isf'}) || '0';
    }
    if ($cont && !$p{'backup'}) {
	$cont =~ s/=/=+/;
    }
    ($cont,$morph,$p{'stem'}||'');
}

1;
