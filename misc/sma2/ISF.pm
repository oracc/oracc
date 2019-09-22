package ORACC::SMA2::ISF;

require Exporter;
@ISA=qw/Exporter/;
@EXPORT = qw/is_isf get_last_isf/;

use warnings; use strict; use utf8;

my $dig = '[0-9₀-₉₊]';
my %last_isf = ('isf'=>undef,
		'ambig'=> [],
	       );

# ISF1   ISF2   ISF3  ISF4  ISF5  ISF6   ISF7   ISF8   ISF9   ISF10
# IAF    CP     DAT   0mnb  COM   0mnb   T/A    LOC/LT 0mnb   enzen
# a      mu     a           da           ci     ni
#        ba     na                       ta     I
#        V      (m)e                            
#        Vmma   ne

# all graphemes which could be ISF morphemes
my @isfm = qw/
    a ab al an am3 am₃
    ba bi2 bi₂
    da de de3 de₃ di di3 di₃
    ga gi4 gi₄
    i i3 i₃ ib ib2 ib₂ im in ni
    ma me mi mu
    na nam nam2 nam₂ ne ni nu
    ra re ri ri2 ri₂
    ca ci cu ša ši šu
    ta
    u3 u₃ u8 u₈ ub ul um un
    ze2 ze₂
    /;
my %isfm = ();
@isfm{ @isfm } = ();

# graphemes that could be conjugational prefixes
my @cpg = qw/
    a ab al am am3 am₃ an
    ma mi mu
    ba bi2 bi₂
    i3 i₃ ni i u u₃ en in eb ib eb2 eb₂ ib2 ib₂
    em im
    /;
my %cpg;
@cpg{ @cpg } = ();

my %cp_data = (
    'a'    => [ 'V',  ''],
    'ab'   => [ 'V',  'b'],
    'al'   => [ 'al', ''],
    'an'   => [ 'V',  'n'],
    'am3'  => [ 'V',  'm'],
    'am₃'  => [ 'V',  'm'],
    'i3'   => [ 'V',  '' ],
    'i₃'   => [ 'V',  '' ],
    'ni'   => [ 'V',  '' ],
    'i'    => [ 'V',  '' ],
    'ib'   => [ 'V',  'b'],
    'ib2'  => [ 'V',  'b'],
    'ib₂'  => [ 'V',  'b'],
    'im'   => [ 'V',  'm'],
    'in'   => [ 'V',  'n'],
    'eb'   => [ 'V',  'b'],
    'eb2'  => [ 'V',  'b'],
    'eb₂'  => [ 'V',  'b'],
    'em'   => [ 'V',  'm'],
    'en'   => [ 'V',  'n'],
    'en6'  => [ 'V',  'n'],
    'en₆'  => [ 'V',  'n'],
    'mu'   => [ 'mu', ''],
    'mi'   => [ 'mu', 'I'],
    'ma'   => [ 'mu', 'a'],
    'ba'   => [ 'ba', ''],
    'bi2'  => [ 'ba', 'I'],
    'bi₂'  => [ 'ba', 'I'],
    'Vmma' => [ 'Vmma',''],
    'Vmmi' => [ 'Vmma','I'],
    'u₃'   => [ 'V',  '' ],
);

#    'me'   => [ 'mu', 'e'],

# graphemes that could be dative infixes
# excluding  ni  -for now
my @datg = qw/
    a
    ra re re2 re₂ ri ri2 ri₂
    na
    me
    ne
    /;
my %datg;
@datg{ @datg } = ();

my %dat_data = (
    'a' => [ 'a',  '' ],
    'ra'=> [ 'ra', '' ],
    're'=> [ 'ra', 'e'],
    'ri'=> [ 'ra', 'I'],
    'ri2'=> [ 'ra', 'I'],
    'ri₂'=> [ 'ra', 'I'],
    'na'=> [ 'na', ''],
    'me'=> [ 'me', ''],
    'ne'=> [ 'ne', ''],
);

# graphemes that could be menb writings
my @menbg = qw/
    m n b
    am3 am₃ am im um
    ab ib ib2 ib₂ ub
    an in un
    /;
my %menbg;
@menbg{ @menbg } = ();

##########################################################

# return the index of the grapheme after any verbal prefix
# 0 means that no prefix was found
sub
is_isf {
    my($ix, $isf_ref, $ambig_ref) = parse_isf(@_);
    $last_isf{'post_isf'} = $ix;
    $last_isf{'isf'} = $isf_ref;
    $last_isf{'ambig'} = $ambig_ref;
    $ix;
}

# Arguments:
#   a list of graphemes
# Returns:
#   the index of the first grapheme which could not be parsed as
#   part of the ISF
#
sub
parse_isf {
    my $root = shift;
    my @g = @_;

    my $orig = join('-',@g);

    my @isf = ();
    my $isf = 0;
    my $g_index = 0;


    my @ambig = ();

    my $vx = -1; # next possible ISF index
    my $n = 0;
    my $rest = '';
    my $auslaut = '';

  PARSE:
    {
	$rest = '';
	if (!exists($cpg{$g[0]}) && $g[0] =~ /^(.)([aeiu])m?$dig*$/) {
	    my $anlaut = $1;
	    my $vowel = $2;
	    if ($root =~ /([bdgŋjhklmnprsšcz])$dig*$/) {
		$auslaut = $1;
		if ($g[0] eq 'nam') {
		    $rest = 'm';
		} 
		if (!length($anlaut) || $auslaut eq $anlaut
		   || ($auslaut eq 'd' && $anlaut eq 'r')) {
		    $g[0] = $vowel || 'a';
		} else {
		    last PARSE unless exists($datg{$g[0]});
		}
	    } else {
		if ($g[0] =~ /^u[3₃]$/) { # conservative--perhaps u5 and u8 also for ISF?
		    $g[0] = 'u';
		} else {
		    # $g[0] = $vowel;
		    last PARSE;
		}
	    }
	}

	# ISF1 = IAF (Imperative Affix)
	if ($g[0] =~ /^a/) {
	    ++$g_index unless $g[0] =~ /[mnb]$/;
	    $isf[1] = 'a';
	    $vx = 2;
	} elsif ($g[0] =~ /^u/) {
	    ++$g_index unless $g[0] =~ /[mnb]$/;
	    $isf[1] = 'u';
	    $vx = 2;
	}

	last PARSE if isf_terminal($vx,$rest,$g_index,@g);

	# ISF2 = Conjugational
	if ($n = cp($rest)) {
	    
	    ($isf[2],$rest) = @{$cp_data{$rest}};
	    $vx = 3;
	    
	} elsif (($n = cp($g[$g_index],$g[$g_index+1],$rest))
		 && (defined($isf[1]) || defined($isf[2]) 
		     || cp_pair($g[$g_index],$g[$g_index+1]))) {

	    my $gsub;
	    if ($n == 2) {
		$gsub = $g[$g_index+1] eq 'ma' ? 'Vmma' : 'Vmmi';
	    } else {
		$gsub = $g[$g_index];
		if ($rest eq 'm' && $gsub =~ /^m/) {
		    if ($gsub eq 'ma') {
			$gsub = 'Vmma';
		    } elsif ($gsub eq 'mi') {
			$gsub = 'Vmmi';
		    }
		    $rest = '';
		}
	    }
	    
	    ($isf[2],$rest) = @{$cp_data{$gsub}};
	    $g_index += $n;

	    if ($rest eq 'I') {
		$vx = 8;
	    } else {
		$vx = ($gsub =~ /^.[ei][2₂]?$/) ? 8 : 3; # immi, i3, i
	    }
	    
	} else {
	    # if we didn't find a CP
	    # just let it go
	}

    	last PARSE if isf_terminal($vx,$rest,$g_index,@g);

	# delete ni after bi2 or Vmmi (take it as 'i3')
	if ($vx == 8 && $g_index < $#g && $g[$g_index] eq 'ni') {
	    ++$g_index;
	}

	$g_index += skip_pleonastic($g_index, @g);

	# ISF 3 = Dative
	if ($vx <= 3) {
	    if (exists $datg{$rest}) {
		
		if ($g_index <= $#g && exists($datg{$g[$g_index]}) 
		    && $g[$g_index] =~ /a$/) {
		    ($isf[3],$rest) = @{$dat_data{$g[$g_index]}};
		    $rest = '';
		    ++$g_index;
		} else {
		    ($isf[3],$rest) = @{$dat_data{$rest}};
		}
		$vx = 4;
		
	    } elsif ($g_index <= $#g
		     && exists($datg{$g[$g_index]})) {
		
		($isf[3],$rest) = @{$dat_data{$g[$g_index]}};
		++$g_index;
		
	    }
	    
	    last PARSE if isf_terminal($vx,$rest,$g_index,@g);
	}

	$g_index += skip_pleonastic($g_index, @g);

	# ISF 4 = menb
	if (!enzen($rest,$g_index,@g)) {
	    if ($rest =~ /[menb]/) {
		$isf[4] = $rest;
		$vx = 5;
		$rest = '';
	    } elsif (!length($rest) && exists($menbg{$g[$g_index]})) {
		$isf[4] = menb($g[$g_index]);
		++$g_index;
		$vx = 5;
	    }
	}

	last PARSE if isf_terminal($vx,$rest,$g_index,@g);

	if ($g_index <= $#g && $vx <= 5) {

	    my $g = $g[$g_index];
	    
	    #ISF 5 = COM
	    if ($g eq 'da') {
		$isf[5] = 'da';
		$vx = 6;
		++$g_index;
	    } elsif ($g =~ /^d[ie][3₃]?$/) {
		$isf[5] = 'da';
		$vx = 8;
		$rest = 'i';
		++$g_index;
	    }
	    
	    # ISF 5 or 7: te/ti = da+I or ta+I ?
	    elsif ($g eq 'te' || $g eq 'ti') {
		if (defined($isf[5]) && length($isf[5])) {
		    $isf[7] = 'ta';
		    $vx = 8;
		    $rest = 'i';
		    ++$g_index;
		} else {
		    $isf[5] = 'da';
		    $vx = 8;
		    $rest = 'i';
		    ++$g_index;
		}
	    }

	    last PARSE if isf_terminal($vx,$rest,$g_index,@g);
	    
	    # ISF 6 = menb
	    if (!enzen($rest,$g_index,@g)) {
		if ($vx <= 6) {
		    if ($rest =~ /[menb]/) {
			$isf[6] = $rest;
			$vx = 7;
			$rest = '';
		    } elsif (!length($rest) && exists($menbg{$g[$g_index]})) {
			$isf[6] = menb($g[$g_index]);
			++$g_index;
			$vx = 7;
		    }
		}
	    }

	    last PARSE if isf_terminal($vx,$rest,$g_index,@g);

	    #ISF 7 = TERM/AT
	    $g = $g[$g_index];
	    if (defined($g) && $vx <= 7) {
		if ($g eq 'ra' || $g eq 'ta' ) {
		    $isf[7] = 'ta';
		    $vx = 8;
		    ++$g_index;
		} elsif ($g =~ /^ri[2₂]?$/) {
		    $isf[7] = 'ta';
		    $vx = 8;
		    $rest = 'i';
		    ++$g_index;
		} elsif ($g eq 're') {
		    $isf[7] = 'ta';
		    $vx = 9;
		    $rest = 'e';
		    ++$g_index;
		} elsif ($g =~ /^[šc]i$/) {
		    $isf[7] = 'ši';
		    $vx = 8;
		    ++$g_index;
		}
	    }
	    
	    last PARSE if isf_terminal($vx,$rest,$g_index,@g);
	}
	
	# delete ni after te/ti/de/di
	if ($rest =~ /I/ && $g_index < $#g && $g[$g_index] eq 'ni') {
	    ++$g_index;
	}

	#ISF 8 = Loc/LT
	if ($rest && $rest =~ /i/i) {
	    $isf[8] = 'i';
	    $vx = 9;
	    $rest = '';
	} elsif ($g_index <= $#g && $g[$g_index] eq 'ni') {
	    $isf[8] = 'ni';
	    ++$g_index;
	    $vx = 9;
	}

	last PARSE if isf_terminal($vx,$rest,$g_index,@g);

	# ISF 9 = menb
	if (!enzen($rest,$g_index,@g)) {
	    if ($rest =~ /[menb]/) {
		$isf[9] = $rest;
		$vx = 10;
		$rest = '';
	    } elsif (!length($rest) && exists($menbg{$g[$g_index]})) {
		$isf[9] = menb($g[$g_index]);
		++$g_index;
		$vx = 10;
	    }
	}

	last PARSE if isf_terminal($vx,$rest,$g_index,@g);

	# ISF 10 = enzen
	if ($g[$g_index] =~ /^ze[2₂]$/) {
	    $isf[10] = 'enzen';
	    ++$g_index;
	    $rest = '';
	} elsif ($g_index < $#g && $g[$g_index+1] =~ /^ze[2₂]$/) {
	    $isf[10] = 'enzen';
	    $g_index += 2;
	    $rest = '';
	}
        if ($g_index <= $#g && $g[$g_index] =~ /^en$dig*$/) {
	    ++$g_index;
	}
    }

    ($g_index, \@isf, \@ambig);
}

sub
enzen {
    my($rest,$g_index,@g) = @_;
    #rest = 'n' and g = 'ze2'
    if (defined($rest) && length($rest) && $rest =~ /n$/
	&& $g_index <= $#g && $g[$g_index] =~ /^ze[2₂]$/) {
	return 1;
    }
    if ($g_index <= $#g && $g[$g_index] =~ /^[aeiu]n$/
	&& $g_index < $#g && $g[$g_index+1] =~ /^ze[2₂]$/) {
	return 1;
    }
    if ($g_index <= $#g) {
	return $g[$g_index] =~ /^ze[2₂]$/;
    }
    0;
}

sub
known_datg {
    foreach my $d (@datg) {
	return 1 if $_[0] eq $d;
    }
    0;
}

sub
cp {
    my ($g0,$g1) = @_;

    if (exists($cpg{$g0})) {
	return 2 if ($g0 =~ /^[aei]m/ && defined($g1) && $g1 =~ /^m/);
	return 1;
    }

    return 0;
}

sub
cp_pair {
    my($g0,$g1,$rest) = @_;
    $rest = '' unless defined $rest;
    $g0 =~ /^ab|al|am3|am₃|an|mu|mi|ma|ba|bi2|bi₂|im|in|i3|i₃|ni|ib2|ib₂$/
	|| $rest eq 'm'
	|| 0;
}

sub
menb {
    my $g = shift;
    if ($g =~ /u$dig*$/) {
	'e';
    } else {
	$g =~ /([menb])$dig*$/;
	$1;
    }
}

# delete an in na-an-na, um in nu-um-ma etc.
sub
skip_pleonastic {
    my($g_index,@g) = @_;
    if ($g_index > 0 && $g_index < $#g) {
	if (($g[$g_index-1] =~ /a$/ 
	     && $g[$g_index] =~ /a[nm]$dig*/ && $g[$g_index+1] =~ /^na|ni|ne$/)
	    || ($g[$g_index-1] =~ /u$/ 
		&& ($g[$g_index] eq 'un' && $g[$g_index+1] =~ /^na|ni|ne$/)
		|| ($g[$g_index] eq 'um' && $g[$g_index+1] =~ /^ma|mi|me$/))) {
	    return 1;
	}
    }
    return 0;
}

sub
isf_terminal {
    my ($vx0,$rest,$ix,@g) = @_;
    return 0 if length $rest;
    $vx0 == 11  || $ix > $#g || !exists($isfm{$g[$ix]});
}

sub
get_last_isf {
    return \%last_isf;
}

1;
