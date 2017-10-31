package ORACC::SMA::VPR;

require Exporter;
@ISA=qw/Exporter/;
@EXPORT = qw/is_vpr get_last_vpr/;

use warnings; use strict; use utf8;
use ORACC::SMA::Graphinfo;
my %last_vpr = ('vpr'=>undef,
		'ambig'=> [],
	       );

my $dig = '[0-9₀-₉₊]';

# VPR1   VPR2    VPR3   VPR4  VPR5  VPR6   VPR7   VPR8   VPR9   VPR10
# MP     CONJ    CN     DAT   P2    COM    P2     T/A    LOC/LT P1
# 0      inga    mu     a     m     da     m      ci     ni     m
# nu             ba     ra    e            e      ta     a      e
# ha             V      na    n            n             i      n
# na             Vmma   me    b            b                    b
# bara           al     ne
# ca
# ga
# u
# nuc

# all graphemes which could be verbal prefix morphemes
# WATCHME: aj2 ja2 can occur in prefixes but for now are not 
# recognized because of the confusion it causes with roots (this
# can be handled, but is it worth it?)
my @vpm = qw/
    a ab al al6 al₆ an am3 am₃ am2 am₂ am6 am₆
    b ba be be2 be₂ bi bi2 bi₂
    da da5 da₅ de de3 de₃ di di3 di₃ du10 du₁₀
    e eb eb2 eb₂ em en en6 en₆ eŋ₃ em₃
    ga ga2 ga₂ gi4 gi₄ gu2 gu₂
    ha he he2 he₂ hu
    i i3 i₃ ib ib2 ib₂ ij3 iŋ₃ im in iri
    ji6 ŋi₆
    la li
    m ma me mi mu ma₃
    n na nam nam2 nam₂ ne ne2 ne₂ neda ni nu
    ra re ri ri2 ri₂
    ca ša ce3 še₃ ci ši cu šu
    ta te ti
    u u3 u₃ u5 u₅ u8 u₈ ub ul um un uš
    /;
my %vpm = ();
@vpm{ @vpm } = ();

# graphemes that could be modal prefixes
my @mpg = qw/
    nu la li li₉
    ha he he2 he₂ hu
    da de3 de₃ du5 du₅ du10 du₁₀
    na nam nam2 nam₂
    ga ga2 ga₂ gi4 gi₄ gu2 gu₂
    ba
    a u u3 u₃ ub ul um un i3 i₃ i iri uš
    ca ša ce3 še₃ cu šu ci ši
    /;
my %mpg;
@mpg{ @mpg } = ();

# The array in 3rd place in mp_data and friends is the
# indexing information for g_reg calls.  The first integer
# is the index of the first character in the grapheme that
# is part of the morpheme; the second is the index of the last
# character in the grapheme that is part of the morpheme.
#
# [0,0] means only one character, the first
# [0] means the entire grapheme
#

my %mp_data = (
    'da'   => [ 'ga',  '', [ 0 ] ],
    'de3'  => [ 'ha',  '', [ 0 ] ],
    'du5'  => [ 'ha',  '', [ 0 ] ],
    'du10' => [ 'ha',  '', [ 0 ] ],
    'de₃'  => [ 'ha',  '', [ 0 ] ],
    'du₅'  => [ 'ha',  '', [ 0 ] ],
    'du₁₀' => [ 'ha',  '', [ 0 ] ],
    'ha'   => [ 'ha',  '', [ 0 ] ],
    'he'   => [ 'ha',  '', [ 0 ] ],
    'he2'  => [ 'ha',  '', [ 0 ] ],
    'he₂'  => [ 'ha',  '', [ 0 ] ],
    'hu'   => [ 'ha',  '', [ 0 ] ],
    'nu'   => [ 'nu',  '', [ 0 ] ],
    'la'   => [ 'nu',  '', [ 0 ] ],
    'li'   => [ 'nu',  '', [ 0 ] ],
    'li₉'  => [ 'nu',  '', [ 0 ] ],
    'na'   => [ 'na',  '', [ 0 ] ],
    'nam'  => [ 'na',  'm', [ 0,2 ] ],
    'nam2' => [ 'na',  'm', [ 0,2 ] ],
    'nam₂' => [ 'na',  'm', [ 0,2 ] ],
    'ga'   => [ 'ga',  '', [ 0 ] ],
    'ga2'  => [ 'ga',  '', [ 0 ] ],
    'gi4'  => [ 'ga',  '', [ 0 ] ],
    'gu2'  => [ 'ga',  '', [ 0 ] ],
    'ga₂'  => [ 'ga',  '', [ 0 ] ],
    'gi₄'  => [ 'ga',  '', [ 0 ] ],
    'gu₂'  => [ 'ga',  '', [ 0 ] ],
    'bara' => [ 'bara','', [ 0 ] ],
    'a'    => [ 'V',   '', [ 0 ] ],
    'u3'   => [ 'u',   '', [ 0,1 ] ],
    'i3'   => [ 'u',   '', [ 0,1 ] ],
    'u'    => [ 'u',   '', [ 0 ] ],
    'u₃'   => [ 'u',   '', [ 0,1 ] ],
    'i₃'   => [ 'V',   '', [ 0,1 ] ],
    'i'    => [ 'V',   '', [ 0 ] ],
    'iri'  => [ 'V',  'ri', [ 0,2 ] ],
    'ub'   => [ 'u',   'b', [ 0,0 ] ],
    'um'   => [ 'u',   'm', [ 0,0 ] ],
    'un'   => [ 'u',   'n', [ 0,0 ] ],
    'ul'   => [ 'u',   'al', [ 0,0 ] ],
    'nuc'  => [ 'nuš', '', [ 0 ] ],
    'nuš'  => [ 'nuš', '', [ 0 ] ],
    'ca'   => [ 'ša', '', [ 0 ] ],
    'ce3'  => [ 'ša', '', [ 0 ] ],
    'ci'   => [ 'ša', '', [ 0 ] ],
    'cu'   => [ 'ša', '', [ 0 ] ],
    'ša'   => [ 'ša', '', [ 0 ] ],
    'še₃'  => [ 'ša', '', [ 0 ] ],
    'ši'   => [ 'ša', '', [ 0 ] ],
    'šu'   => [ 'ša', '', [ 0 ] ],
);

# graphemes that could be conjugational prefixes
# aj2 ja2
my @cpg = qw/
    a ab al al6 al₆ am2 am₂ am3 am₃ an
    ma me mi mu
    ba be be2 be₂ bi bi2 bi₂
    e i3 i₃ i en in eb ib eb2 eb₂ ib2 ib₂ ij3 iŋ₃
    em im em₃ eŋ₃
    ŋa₂
    /;
my %cpg;
@cpg{ @cpg } = ();


# Note that 'um' can't be reached unless it's followed by /^m|i/, in which case
# we take it as Vmma

#    'aj2'  => [ 'V',  'm'],  #  this is an awkward one to allow b/c aj[measure]
#    'ŋa₂'  => [ 'mu', 'a'],
#    'ja2'  => [ 'mu', 'a'],

my %cp_data = (
    'a'    => [ 'V',  ''],
    'ab'   => [ 'V',  'b'],
    'al'   => [ 'al', ''],
    'al6'  => [ 'al', ''],
    'al₆'  => [ 'al', ''],
    'an'   => [ 'V',  'n'],
    'am2'  => [ 'V',  'm'],
    'am3'  => [ 'V',  'm'],
    'am6'  => [ 'V',  'm'],
    'am₂'  => [ 'V',  'm'],
    'am₃'  => [ 'V',  'm'],
    'am₆'  => [ 'V',  'm'],
    'e'    => [ 'V',  ''],
    'eŋ₃'  => [ 'V',  'm'],
    'i'    => [ 'V',  '' ],
    'i3'   => [ 'V',  '' ],
    'i₃'   => [ 'V',  '' ],
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
    'mi'   => [ 'mu', 'i'],
    'me'   => [ 'mu', 'e'],
    'ma'   => [ 'mu', 'a'],
    'ba'   => [ 'ba', ''],
    'be'   => [ 'ba', 'i'],
    'be2'  => [ 'ba', 'i'],
    'be₂'  => [ 'ba', 'i'],
    'bi'   => [ 'ba', 'i'],
    'bi2'  => [ 'ba', 'i'],
    'bi₂'  => [ 'ba', 'i'],
    'Vmma' => [ 'Vmma',''],
    'Vmme' => [ 'Vmma','e'],
    'Vmmi' => [ 'Vmma','i'],
    'Vmmu' => [ 'Vmma',''],
    'um'   => [ 'Vmma',''], 
);


# graphemes that could be dative infixes
# excluding  ni  -for now
my @datg = qw/
    a
    ra re re2 re₂ ri ri2 ri₂
    na
    me
    ne ne2 ne₂ neda
    /;
my %datg;
@datg{ @datg } = ();

my %dat_data = (
    'a' => [ 'a',  '' ],
    'ra'=> [ 'ra', '' ],
    're'=> [ 'ra', 'e'],
    'ri'=> [ 'ra', 'i'],
    'ri2'=> [ 'ra', 'i'],
    'ri₂'=> [ 'ra', 'i'],
    'na'=> [ 'na', ''],
#    'ni'=> [ 'na', 'i'],
    'me'=> [ 'me', ''],
    'ne'=> [ 'ne', ''],
    'neda'=> [ 'ne', 'da'],
    'ne2'=>[ 'ne', ''],
    'ne₂'=>[ 'ne', ''],
);

## graphemes that could be writings of e[to speak]
#my @eg = qw/
#    be2 be₂ be7 be₇
#    me
#    ne ne2 ne₂
#    /;
#my %eg;
#@eg{ @eg } = ();

# graphemes that could be menb writings
my @menbg = qw/
    m e n b
    am3 am₃ em am im um
    ab eb eb2 eb₂ ib ib2 ib₂ ub
    an en en6 en₆ in un
    u3 u₃ u5 u₅ u8 u₈
    /;
my %menbg;
@menbg{ @menbg } = ();

# verbal-morpheme graphemes that could be verbal roots
my @root = qw/
    ba
    gi4 gi₄
    ib2 ib₂
    me
    nu
    ra
    ri
    ti
    ul
    be2 be₂
    be7 be₇
    ne
    /;

%VPR::root = ();
@VPR::root{ @root } = ();

my $e_rest = '';

##########################################################

# return the index of the grapheme after any verbal prefix
# 0 means that no prefix was found
sub
is_vpr {
    my($ix, $vpr_ref, $ambig_ref) = parse_vpr(@_);
    $last_vpr{'post_vpr'} = $ix;
    $last_vpr{'vpr'} = $vpr_ref;
    $last_vpr{'ambig'} = $ambig_ref;
    $last_vpr{'graph'} = g_data();
    $ix;
}

sub
get_e_rest {
    $e_rest;
}

# Arguments:
#   a list of graphemes
# Returns:
#   the index of the first grapheme which could not be parsed as
#   part of the VPR
#
sub
parse_vpr {

    my @g = @_;
    my @b = ();
    my $orig = join('-',@g);

    warn("parse_vpr passed $orig\n") if $ORACC::SMA::verbose;

    my @vpr = ();
    my $vpr = 0;
    my $g_index = 0;

    my $NI_flag = 0;
    my @ambig = ();

    my @breg = ();
    my $dreg = undef;
    my $sreg = '';

    my $vx = -1; # next possible VPR index
    my $n = 0;
    my $rest = '';
    $e_rest = '';

    my ($gref,$bref) = g_breaks(@g);
    @b = @$bref;
    @g = @$gref;    

  PARSE:
    {

	# VPR1 = Modals
	if ($n = mp($g[$g_index],$g[$g_index+1])) {
	    
	    my $gsub;
	    $sreg = $g[$g_index];
	    if ($n == 2) {
		$gsub = $g[$g_index] eq 'ba' ? 'bara' : 'nuš';
		$sreg = "$g[$g_index]-$g[$g_index+1]";
		@breg = @b[$g_index,$g_index+1];
	    } else {
		$gsub = $g[$g_index];
		@breg = ($b[$g_index]);
	    }
	    ($vpr[1],$rest,$dreg) = @{$mp_data{$gsub}};
	    g_reg(1,[$sreg,\@breg,@$dreg]);
	    $g_index += $n;
	    $vx = 2;
	}
	
	last PARSE if vpr_terminal($vx,$rest,$g_index,@g);

	# VPR2 = Conjunctive
	if (($g[$g_index] =~ /^[aeiu][nm]?$dig*$/o || $rest =~ /^[mn]$/)
	    && $g_index < ($#g-1) && $g[$g_index+1] =~ /^[gjŋ]a$dig*$/o) {
	    $vpr[2] = 'inga';
	    if (length $rest) {
		++$g_index;
		$rest = '';
	    } else {
		$g_index += 2;
	    }
	    $vx = 3;
	} elsif ($g_index < $#g
		 && ($g[$g_index] eq 'ga')) {
#	    || $g[$g_index] eq 'ga2'
#		     || $g[$g_index] eq 'ja2'|| $g[$g_index] eq 'ŋa₂')) {
	    $vpr[2] = 'inga';
	    ++$g_index;
	    $vx = 3;
	    $rest = '';
	}

	last PARSE if vpr_terminal($vx,$rest,$g_index,@g);

#	if ($g_index < ($#g-1)) {
#	    $g_index += skip_pleonastic($g_index, @g);
#	}

	# VPR3 = Conjugational
	if (length($rest) && ($n = cp($rest))) {
	    
	    ($vpr[3],$rest) = @{$cp_data{$rest}};
	    $vx = 4;
	    
	} elsif (($n = cp($rest,$g[$g_index],$g_index,@g))
		 && ((defined($vpr[1]) && $vpr[1] ne 'nu' || $g[$g_index] !~ /^b/)
		     || defined($vpr[2])
		     || cp_pair($g[$g_index],$g[$g_index+1], $g[$g_index-1]||''))) {

	    my $gsub;

	    # delete /m/ of nam-ba < na.ba
	    if ($n == 1 
		&& $rest eq 'm' 
		&& defined($vpr[1]) && $vpr[1] eq 'na'
		&& $g[$g_index] =~ /^b/) {
		$rest = '';
	    }

	    if ($n == 3) {
		$g[$g_index+2] =~ /([aeiu])/;
		$gsub = "Vmm$1";
	    } elsif ($n == 2) {
		if ($rest eq 'm') {
		    $g[$g_index] =~ /([aeiu])/;
		    $gsub = "Vmm$1";
		    $rest = '';
		    --$n;
		} else {
		    $g[$g_index+1] =~ /([aeiu])/;
		    $gsub = "Vmm$1";
		}
	    } elsif ($g_index < $#g && $g[$g_index+1] eq 'ri') {
		$gsub = 'me';
	    } else {
		$gsub = $g[$g_index];
		if ($rest eq 'm' 
		    && $gsub =~ /^m/) {  # |ja2|ŋa₂/) {
		    if ($gsub =~ /^ma/) { # |ja2|ŋa₂/) {
			$gsub = 'Vmma';
		    } elsif ($gsub eq 'mi') {
			$gsub = 'Vmmi';
		    }
		    $rest = '';
		} elsif ($gsub eq 'um' && $g[$g_index+1] =~ /^i/) {
		    $gsub = 'Vmmi';
		}
	    }

	    if (!defined($cp_data{$gsub})) {
		warn("VPR internal error: gsub value '$gsub' not in \%cp_data\n");
		($vpr[3],$rest) = ('','');
	    } else {
		($vpr[3],$rest) = @{$cp_data{$gsub}};
	    }

	    $g_index += $n;

#	    if ($n == 2 && $rest eq 'e' && $g_index < $#g) {
#		$gsub =~ s/e$/a/;
#		$rest = '';
#	    }

	    # delete vowel if Ci is followed by -ri-, -ni-, etc.
	    my $niri = ($g_index <= $#g ? $g[$g_index] =~ /^[nr][ei][2₂]?$/ : 0);
	    if ($rest =~ /^[ei]$/i && $g_index <= $#g && $niri) {
		$rest = '';
	    }
	    
	    if ($rest eq 'i') {
		$vx = 9;
	    } else {
		$vx = ($gsub =~ /^.[ei][2₂]?$/ && !$niri) ? 9 : 4; # immi, i3, i
	    }
	    
	} else {
	    # if we didn't find a CP
	    # supply 'V' if there's a modal
	    # else bail
	    if (defined $vpr[1] && $vpr[1] ne 'nuš') {
		$vpr[3] = 'V';
		$vx = 4;
#	    } elsif (defined $vpr[2]) {
#		# FIXME: is inga+NO-CP ok?
	    } else {
		warn "VPR: spurious 'rest' value '$rest' in '$orig'\n" if length $rest;
		$vx = 11;
	    }
	}

    	last PARSE if vpr_terminal($vx,$rest,$g_index,@g);

	if ($g_index < ($#g-1)) {
	    $g_index += skip_pleonastic($g_index, @g);
	}

	# delete ni after bi2, Vmmi, etc. (take it as 'i3')
	if ($vx == 9 && $g_index < $#g && $g[$g_index] eq 'ni') {
	    ++$g_index;
	}

	# VPR 4 = Dative
	if ($vx <= 4) {
	    if (exists $datg{$rest}) {
		
		if ($g_index <= $#g && exists($datg{$g[$g_index]}) 
		    && $g[$g_index] =~ /a$/) {
		    ($vpr[4],$rest) = @{$dat_data{$g[$g_index]}};
		    $rest = '';
		    ++$g_index;
		} else {
		    ($vpr[4],$rest) = @{$dat_data{$rest}};
		}
		$vx = 5;
		
	    } else {
		my $gsub = $g[$g_index];
		if ($g_index < $#g && $rest =~ /^[mn]$/ && $g[$g_index] =~ /^$rest/) {
		    $rest = '';
		} elsif ($rest eq 'e' && $g_index < $#g && is_niri($g[$g_index])) {
		    $rest = '';
		} elsif ($g_index < $#g && $g[$g_index] eq 'e' 
			 && is_niri($g[$g_index+1]) && $g[$g_index+1] ne 'ni') {
		    # for now take mu-e-ni- as mu.e.NI; mu-e-ne as mu.ne
		    ++$g_index;
		    $gsub = $g[$g_index];
		    $gsub =~ s/\d*$//;
		} elsif ($g_index < $#g && $g[$g_index] eq 'en' && $g[$g_index+1] eq 'e') {
		    ++$g_index;
		    $gsub = 'ne';
		}
		if (!length($rest)
		    && $g_index <= $#g
		    && exists($datg{$gsub})) {
		    
		    if ($g[$g_index] eq 'ni') {
			$NI_flag = 1;
		    }
		    ($vpr[4],$rest) = @{$dat_data{$gsub}};
		    ++$g_index;
		    $vx = ($rest eq 'i') ? 9 : 5;
		    
		}
	    }
	    
	    last PARSE if vpr_terminal($vx,$rest,$g_index,@g);
	}

	if ($g_index < ($#g-1)) {
	    $g_index += skip_pleonastic($g_index, @g);
	}

	# VPR 5 = menb
	if ($rest =~ /^[menb]$/) {
	    $vpr[5] = $rest;
	    $vx = 6;
	    $rest = '';
	} elsif (!length($rest) && exists($menbg{$g[$g_index]})) {
	    ($vpr[5]) = menb($g[$g_index]);
	    ++$g_index;
	    $vx = 6;
	}

	last PARSE if vpr_terminal($vx,$rest,$g_index,@g);

	if ($vx <= 7) {

	    my $g = $g[$g_index];
	    
	    #VPR 6 = COM
	    if (($rest && $rest eq 'da') || 
		$g eq 'da' || $g eq 'da5' || $g eq 'da₅') {
		$vpr[6] = 'da';
		$rest = '' if $rest;
		$vx = 7;
		++$g_index;
	    } elsif ($g =~ /^d[ie][3₃]?$/) {
		$vpr[6] = 'da';
		$vx = 9;
		$rest = 'i';
		++$g_index;
	    }
	    
	    # VPR 6 or 8: te/ti = da+i or ta+i ?
	    elsif ($g eq 'te' || $g eq 'ti') {
		if (defined($vpr[6]) && length($vpr[6])) {
		    $vpr[8] = 'ta';
		    $vx = 9;
		    $rest = 'i';
		    ++$g_index;
		} else {
		    $vpr[6] = 'da';
		    $vx = 9;
		    $rest = 'i';
		    ++$g_index;
		}
	    }

	    last PARSE if vpr_terminal($vx,$rest,$g_index,@g);

	    # VPR 7 = menb
	    if ($vx <= 7) {
		if ($rest =~ /^[menb]$/) {
		    $vpr[7] = $rest unless double_menb(7,$rest,\@vpr);
		    $vx = 8;
		    $rest = '';
		} elsif (!length($rest) && exists($menbg{$g[$g_index]})) {
		    my $menb;
		    ($menb) = menb($g[$g_index]);
		    $vpr[7] = $menb unless double_menb(7,$menb,\@vpr);
		    ++$g_index;
		    $vx = 8;
		}
	    }

	    last PARSE if vpr_terminal($vx,$rest,$g_index,@g);

	    #VPR 8 = TERM/AT
	    $g = $g[$g_index];
	    if ($vx <= 8) {
		if ($g eq 'ra' || $g eq 'ta' ) {
		    $vpr[8] = 'ta';
		    $vx = 9;
		    ++$g_index;
		} elsif ($g eq 'ri' || $g eq 'ri2' || $g eq 'ri₂') {
		    $vpr[8] = 'ta';
		    $vx = 9;
		    $rest = 'i';
		    ++$g_index;
		} elsif ($g eq 're') {
		    $vpr[8] = 'ta';
		    $vx = 10;
		    $rest = 'e';
		    ++$g_index;
		} elsif ($g eq 'ci' || $g eq 'ši'
			 || $g eq 'ce3' || $g eq 'še₃'
			 ) {
		    $vpr[8] = 'ši';
		    $vx = 9;
		    ++$g_index;
		}
	    }
	    
	    last PARSE if vpr_terminal($vx,$rest,$g_index,@g);
	}
	
	# delete ni after te/ti/de/di
	if ($rest =~ /i/ && $g_index < $#g && $g[$g_index] eq 'ni') {
	    ++$g_index;
	}

	#VPR 9 = Loc/LT
	if ($rest =~ /i/) {
	    $vpr[9] = 'i';
	    $vx = 10;
	    $rest = '';
	} elsif ($g[$g_index] eq 'a') {
	    $vpr[9] = 'i';
	    ++$g_index;
	    $vx = 10;
	} elsif (($g[$g_index] =~ /^i([mnb])$dig*$/ && $g_index 
		  && $g[$g_index-1] !~ /i$dig*$/) 
		 || ($g[$g_index] =~ /^n[ie]$dig*$/ 
		     && $g_index && $g[$g_index-1] =~ /i$dig*$/)) {
	    $rest = $1 || '';
	    $vpr[9] = 'i';
	    ++$g_index;
	    $vx = 10;
	} elsif ($g[$g_index] eq 'ni') {
#	    $vpr[9] = 'NI';
	    $vpr[9] = 'ni';
	    ++$g_index;
	    $vx = 10;
	}

	last PARSE if vpr_terminal($vx,$rest,$g_index,@g);

	# VPR 10 = menb
	if ($rest =~ /[menb]/) {
	    $vpr[10] = $rest unless double_menb(10,$rest,\@vpr);
	    $vx = 11;
	    $rest = '';
	} elsif (!length($rest) && exists($menbg{$g[$g_index]})) {
	    my $menb;
	    ($menb) = menb($g[$g_index]);
	    $vpr[10] = $menb unless double_menb(10,$menb,\@vpr);
	    ++$g_index;
	    $vx = 11;
	}

    }

    $ambig[0] = $NI_flag;

    print STDERR "VPR.pm: leftover rest '$rest'\n" if length $rest;

    ($g_index, \@vpr, \@ambig);
}

sub
is_niri {
    $_[0] =~ /^[rn][ei][2₂]?$/;
}

sub
double_menb {
    my($ix,$menb,$vpr_ref) = @_;
    my $j = $ix-1;
    while ($j && !defined $$vpr_ref[$j]) {
	--$j;
    }
    if ($ix == 10) {
	if ($j == 7 || $j == 5) {
	    my $prev_menb = $$vpr_ref[$j];
	    return $prev_menb eq $menb || $menb eq 'e';
	}
    } elsif ($ix == 7) {
	if ($j == 5) {
	    my $prev_menb = $$vpr_ref[$j];
	    return $prev_menb eq $menb || $menb eq 'e';
	}
    }
    return 0;
}

sub
known_datg {
    foreach my $d (@datg) {
	return 1 if $_[0] eq $d;
    }
    0;
}

# FIXME: ??? always return true if exists($cpg{$_[0]}) ???
sub
mp {
    my $g = shift;
    if (exists $mpg{$g}) {
	if ($g eq 'a') {
	    return 1 if $_[0] eq 'ba' || $_[0] eq 'ma';
	} elsif ($g eq 'i3' || $g eq 'i') {
	    return 1 if $_[0] eq 'bi2' || $_[0] eq 'bi₂' || $_[0] eq 'mi';
	} elsif ($g eq 'gi4' || $g eq 'gi₄') {
	    return 1 if exists($cpg{$_[0]}) || $_[0] =~ /^i[nb]$dig*$/;
	} else {
	    return 1 if (($g ne 'nu' || $_[0] !~ /^u[cš]$/) && $g ne 'ba');
	    return 2 if ($g eq 'nu' && $_[0] =~ /^u[cš]$/)
		|| ($g eq 'ba' && $_[0] eq 'ra');
	}
    }
    return 0;
}

sub
cp {
    my ($rest, $g, $gx, @g) = @_;

    return 0 if defined($gx) && $gx <= $#g && !exists($vpm{$g[$gx]});

    # pick up 'um-me', 'um-in' as Vmma without getting nu-um-cum mixed up in things
    if (defined $gx) {
	if ($gx < $#g && $g[$gx] =~ 'um') {
	    if ($g[$gx+1] =~ /^m/) {
		return 2;
	    } elsif ($g[$gx+1] =~ /^i/) {
		return 1;
	    }
	} elsif (($gx < $#g && $g[$gx] eq 'e')
		 && (($gx+1) < $#g && $g[$gx+1] eq 'ma')) {
	    return 2;
	}
    }

    if (length $rest) {
	if ($rest eq 'm' && defined($gx) && $gx <= $#g && $g[$gx] =~ /^m/) {
	    return 2;
	} else {
	    if ($rest eq 'm' && defined($gx) && $gx <= $#g && $g[$gx] =~ /^b/) {
		# we parse nam-ba as vpr[][na][ba]
		return 1;
	    } else {
		return exists($cpg{$rest});
	    }
	}
    } else {
	if ($gx <= $#g) {
	    if (exists($cpg{$g})) {
		if ($gx < ($#g-1)) {
		    return 3 if $g[$gx] =~ /^i[3₃]?/ 
			&& $g[$gx+1] =~ /^[aeiu][mjŋ]\d*/ 
			&& $g[$gx+2] =~ /^[mjŋ][aeiu]$dig*$/;
		}
		if ($gx < $#g) {
		    return 2 if ($g[$gx] =~ /^[aeiu][mjŋ][\d₀-₉]*/ 
				 && $g[$gx+1] =~ /^[mjŋ][aeiu][\d₀-₉]*$/);
		}
		return 1;
	    }
	}
    }

    return 0;
}

sub
cp_pair {
    my($g0,$g1,$prev) = @_;

    return 0 if $g0 =~ /^b/ && $prev eq 'nu';

    $g0 =~ /^ab|al|am3|am₃|an|mu|me|mi|ma|ba|be|bi|bi2|bi₂|e|i|im|in|i3|i₃|ib2|ib₂|ub$/
	|| ($g0 eq 'a' && $g1 =~ /^ba|ab|ra|an|na$/)
	|| ($g0 =~ /^i[3₃]?$/ && $g1 =~ /^ni|ra|ri|ri2|ri₂|ib|ib2|ib₂|ij3|iŋ₃|im|in$/)
	|| ($g0 eq 'um' && $g1 =~ /^(?:m|i)/)
	|| ($g0 =~ /^(?:em₃|eŋ₃|am₂|am₃)$/ && $g1 =~ /^(?:ma₃|ŋa₂)/)
	|| 0;
}

sub
menb {
    my $g = shift;
    if ($g =~ /u$dig*$/) {
	('e');
    } else {
	$g =~ /([menb])(e?)$dig*$/;
	my $ret;
	($ret,$e_rest) = ($1,$2);
	($ret);
    }
}

# delete an in na-an-na, um in nu-um-ma etc.
sub
skip_pleonastic {
    my($g_index,@g) = @_;
    if ($g_index > 0 && $g_index < $#g) {
	if (($g[$g_index-1] =~ /a$/ 
	     && $g[$g_index] =~ /a[nm]$dig*/ && $g[$g_index+1] =~ /^(?:na|ni|ne)$dig*$/)
	    || ($g[$g_index-1] =~ /u$/ 
		&& ($g[$g_index] eq 'un' && $g[$g_index+1] =~ /^(?:na|ni|ne)$dig*$/)
		|| ($g[$g_index] eq 'um' && $g[$g_index+1] =~ /^ma|mi|me$/))) {
	    return 1;
	}
    }
    return 0;
}

sub
vpr_terminal {
    my ($vx0,$rest,$ix,@g) = @_;
    return 0 if length $rest;
#    return 0 if $ix <= $#g && exists($eg{$g[$ix]});
    $vx0 == 11  || $ix > ($#g-1) || !exists($vpm{$g[$ix]});
}

sub
get_last_vpr {
    return \%last_vpr;
}

1;
