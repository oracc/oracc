package ORACC::SMA::VSF;

require Exporter;
@ISA=qw/Exporter/;
@EXPORT = qw/is_vsf get_last_vsf/;

use warnings; use strict; use utf8;

use ORACC::SMA::NSF;

my $dig = '[0-9₀-₉₊]';
my %last_vsf = ('vsf'=>undef,
		'ambig'=>undef,
	       );


my @alessadj = qw/gal kug sikil/;
my %alessadj;
@alessadj{@alessadj} = ();

# VSF6 is not really VSF, but this is a convenient way to
# handle ece and jicen

#R-VSF1-VSF2-VSF3-  VSF4-     VSF5 VSF6
#
#  e   de    PRO    NOM/COP   ri   ece
#            en     a              jicen
#            en     am
#            0      ma
#            enden
#            enzen 
#            ec
# 	     ene

my @vsm = qw/
    a am
    da dam de3 de₃
    e en en3 en₆ en6 en₆ ec eš ec2 eš₂ ša
    ma me
    ne
    ri
    ce še jic ŋiš
    u8 u₈
    /;

%VSF::vsm = ();
@VSF::vsm{@vsm} = ();

my @de_graphemes = qw/
    de3 de₃
    da
    dam
    /;
my %de_graphemes = ();
@de_graphemes{@de_graphemes} = ();

# return the index of the grapheme after any verbal suffix
# 0 means that no suffix was found
sub
is_vsf {
    my ($root,$orth,$vpr,@g) = @_;
    my($ix,$vsf_ref,$ambig_ref) = parse_vsf(@_);
    my $ret;
    if ($ix <= $#g && exists($NSF::nsm{$g[$ix]})) {
	${$vsf_ref}[4] = '*a' unless defined ${$vsf_ref}[4];
	$ret = 1;
    } else {
	$ret = $ix;
    }

    $last_vsf{'post_vsf'} = $ix;
    $last_vsf{'vsf'} = $vsf_ref;
    $last_vsf{'ambig'}= $ambig_ref;

    $ret;
}

sub
parse_vsf {
    my $root = shift;
    my $orth = shift;
    my $vpr = shift;
    my @g = @_;
    my $g_index = 0;
    my @vsf = ();
    my $rest = '';
    my $vx = 1;
    my @ambig = ();
    my $auslaut = '';
    my $original_g0 = undef;

#    print STDERR "parse_vsf: entered with root = '$root'\n";

  PARSE:
    {
	last PARSE if $#g < 0;
	$original_g0 = $g[0] unless $original_g0;
	if ($g[0] =~ /^(.)([ae])$dig*$/
	   || ($root =~ /u/ && $g[0] =~ /^(.?)(u)$dig*$/)
	   ) {
	    my $anlaut = $1;
	    my $vowel = $2;
	    if ($root =~ /([bdgŋjhklmnprsšcz])$dig*$/) {
		$auslaut = $1;
		if (!length($anlaut) || $auslaut eq $anlaut
		   || ($auslaut eq 'd' && $g[0] =~ /^(?:ra[2₂]?|re[6₆]?|ru)$/)) {
		    if ($vowel eq 'a') {
			$g[0] = 'a';
		    } else {
			$g[0] = 'e';
		    }
		} else {
		    $auslaut = '';
		}
		$vsf[0] = $original_g0 
		    if $anlaut && $auslaut; # && ($auslaut eq $anlaut);
		if ($vsf[0] && $g[1]) {
		    my $v = $vsf[0];
		    $v =~ s/^.*?(.)$/$1/;
		    if ($g[1] =~ /^$v/) {
			$vsf[0] .= $v;
		    }
		}
	    } elsif ($root =~ /u$dig*$/ && $g[0] =~ /^u$dig*$/) {
		$g[0] = 'e';
	    }
	}

	#VSF1
	if ($g[0] eq 'e' || $g[0] eq 'u₈'
	    || ($g[0] eq 'i' && ($root =~ /i/ || $orth =~ /i$dig*$/))
	   ) {
	    ++$g_index;
	    ++$g_index if $g_index <= $#g && $g[$g_index] eq 'e'; # skip pleonastic 'e'
	    $vsf[1] = 'e';
	    $vx = 2;
	}

	last PARSE if $g_index > $#g;

	#VSF2
	if (exists $de_graphemes{$g[$g_index]}
	   && ($g_index > ($#g-1) || $g[$g_index+1] ne 'en')) {
	    $vsf[2] = 'de';
	    if ($g[$g_index] eq 'dam') {
		$rest = 'am';
		$vx = 4;
	    } elsif ($g[$g_index] eq 'da' 
		     && ($g_index == $#g || $g[$g_index+1] !~ /^n/)) {
		$rest = 'a';
		$vx = 4;
	    } else {
		$rest = '';
		$vx = 3;
	    }
	    ++$g_index;
	}

	last PARSE if ($g_index > $#g && !length($rest));

	#VSF3
	if ($vx <= 3) {
	    if ($g[$g_index] =~ /^en[236₂₃₆]?$/
		|| (($root =~ /a/ || $orth =~ /a/) && $g[$g_index] =~ /^an$/)
		|| (($root =~ /i/ || $orth =~ /i/) && $g[$g_index] =~ /^in$/)
		|| (($root =~ /u/ || $orth =~ /u/) && $g[$g_index] =~ /^un$/)
		|| ($g_index > 0
		    && ($g[$g_index-1] =~ /e\d*$/ 
			|| (($root =~ /a/ || $orth =~ /a/) && $g[$g_index-1] =~ /a$dig*$/)
			|| (($root =~ /i/ || $orth =~ /i/) && $g[$g_index-1] =~ /i$dig*$/)
			|| (($root =~ /u/ || $orth =~ /u/) && $g[$g_index-1] =~ /u$dig*$/))
		    && $g[$g_index] =~ /^[aeiu]?n(?:[aeiu]m?)?$dig*$/)
		|| ($g[$g_index] =~ /^de[3₃]$/ && $g_index < $#g && $g[$g_index+1] eq 'en')
		|| ($g[$g_index] =~ /^ze[2₂]$/ && $g_index < $#g && $g[$g_index+1] eq 'en')
	       ) {
		if ($g_index < $#g && $g[$g_index+1] =~ /^ze[2₂]$/) {
		    $vsf[3] = 'enzen';
		    $g_index += 2;
		    if ($g_index <= $#g && $g[$g_index] =~ /^en$dig*$/) {
			++$g_index;
		    }		    
		} elsif ($g_index < $#g && $g[$g_index+1] =~ /^de[3₃]$/) {
		    $vsf[3] = 'enden';
		    $g_index += 2;
		    if ($g_index <= $#g && $g[$g_index] =~ /^en$dig*$/) {
			++$g_index;
		    }
		} elsif ($g[$g_index] =~ /^de[3₃]$/) {
		    $vsf[3] = 'enden';
		    $g_index += 2; # only reach here on R-de3-en
		} elsif ($g[$g_index] =~ /^ze[2₂]$/) {
		    $vsf[3] = 'enzen';
		    $g_index += 2; # only reach here on R-ze2-en
		} elsif ($g[$g_index] eq 'ne') {
		    $vsf[3] = 'ene';
		    ++$g_index;
		} elsif (($rest =~ /^[aeiu]$/ && $g[$g_index] =~ /^n/)
			 || ($g_index 
			     && $g[$g_index-1] =~ /[aeiu]$dig*$/ && $g[$g_index] =~ /^n/)
			 || $g[$g_index] =~ /^[aeiu]n$dig*$/) {
		    $vsf[3] = 'en';
		    if ($g[$g_index] =~ /a$dig*$/) {
			$rest = 'a';
		    } elsif ($g[$g_index] =~ /am$dig*$/) {
			$rest = 'am';
		    } else {
			$rest = '';
		    }
		    ++$g_index;
		} else {
		    warn("untrapped VSF[3]: g = '$g[$g_index]'\n");
		}
		$vx = 4;
	    } elsif ($g[$g_index] eq 'ne') {
		$vsf[3] = 'ene';
		++$g_index;
		if ($g_index <= $#g && $g[$g_index] eq 'e') {
		    ++$g_index;
		}
		$vx = 4;
	    } elsif ($g[$g_index] =~ /^e[cš][2₂]?$/
		    || (($root =~ /u/ || $orth =~ /u/) && $g[$g_index] =~ /^u[cš]$/)) {
		$vsf[3] = 'eš';
		++$g_index;
		$vx = 4;
	    } elsif ($g[$g_index] =~ /^[cš]a?$/) {
		$vsf[3] = 'eš';
		++$g_index;
		$vx = 4;
		$rest = 'a';
	    }
	}

	last PARSE if ($g_index > $#g && !length($rest));

	#VSF4 -am/-a/-ma
	if ($vx <= 4) {
	    if ($rest eq 'am' 
		|| ($g_index <= $#g && $g[$g_index] =~ /^am[36₃₆]$/)) {

		$vsf[4] = 'am';
		++$g_index unless $rest eq 'am';
		$vx = 5;
		$rest = '';

	    } elsif ($rest eq 'a' 
		     || (!length($rest) && $g_index <= $#g && $g[$g_index] eq 'a')) {

		if (exists($alessadj{$root}) && !$vpr) {
		    $rest = '';
		    last PARSE if $g[$g_index] && $g[$g_index] =~ /^.*?a$/;
		    $vx = 6;
		} else {
		    $vsf[4] = 'a';
		    if ($rest eq 'a') {
			$rest = '';
		    } else {
			++$g_index if $g_index <= $#g && $g[$g_index] eq 'a';
		    }
		    $vx = 5;
		}

	    } elsif (defined($vsf[3]) 
		     && $vsf[3] =~ /n$/ && $g[$g_index] =~ /^n/) {

		$vsf[4] = ($g[$g_index] =~ /m$dig*$/) ? 'am' : 'a';
		++$g_index;
		$vx = 5;

	    } elsif ($g_index <= $#g && $g[$g_index] eq 'ma') {

		$vsf[4] = 'ma';
		++$g_index;
		$vx = 5;
	    }

	}

	last PARSE if ($g_index > $#g && !length($rest));
	
	#VSF5 -ri
	if ($g_index <= $#g && $g[$g_index] eq 'ri' 
	    && (defined($vsf[4]) && $vsf[4] eq 'a')) {
	    
	    $vsf[5] = 'ri';
	    ++$g_index;
	    
	}

	warn("leftover rest '$rest'\n") if length $rest;

	last PARSE if ($g_index > $#g && !length($rest));

	# FIXME: this is very unlikely to be generally correct; probably need
	# to move it to post-NSF parsing, or transliterate independently

	if ($g[$g_index] eq 'e' && $g_index < $#g && $g[$g_index+1] eq 'še') {
	    $vsf[6] = 'eše';
	    $g_index += 2;
	} elsif (defined($vsf[1]) && $vsf[1] eq 'e' 
		 && $g_index == $#g-1 && $g[$g_index] =~ /^e[cš]/) {
	    $vsf[1] = 'e';
	    $vsf[6] = 'eš';
	    ++$g_index;
	}
	
    }

    return ($g_index, \@vsf, \@ambig);

}

sub
get_last_vsf {
    return \%last_vsf;
}

1;
