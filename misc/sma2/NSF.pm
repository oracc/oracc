package ORACC::SMA2::NSF;

require Exporter;
@ISA=qw/Exporter/;
@EXPORT = qw/is_nsf get_last_nsf/;

use warnings; use strict; use utf8;
#use Encode;

my $dig = '[0-9₀-₉₊]';

my $disambig = '';

#N/V-NSF1-NSF2-  NSF3-NSF4-NSF5-NSF6-NSF7-NSF8
#    GEN  POSS   GEN  PLUR GEN  PLUR POST COP 
#    ak   ju     ak   ene  ak   ene  e    men
#         zu                         0    men
#         ani                        ra   am
#         bi                         ta   menden
#         me                         da   menzen
#         zunene                     a    mec
#         anene                      e
#         bi                         ece
#         bida                       gin

my @nsm = qw/
    a ka ke4 ke₄
    kam kam₂ kam₃ kam₄ am3 am₃ am6 am₆ nam um im
    mu gu10 gu₁₀ ju10 ŋu₁₀ zu zu5 zu₅ ni bi biₓ(DUG) be6 be₆ bi3 bi₃ ne ne2 ne₂
    ja2 ŋa₂ ga2 ga₂ za na ba
    e en6 en₆ ne2 ne₂ be2 be₂ 
    da 
    ra ar ir ur ur2 ur₂
    ta 
    ce3 še₃ ac aš aš₂ ec ec2 eš₂ ic iš uc uš
    gin7 gin₇ de ŋen
    me men3 men₃ jen mec meš
    /;

%NSF::nsm = ();
@NSF::nsm{@nsm} = ();

my @poss = qw/
    mu gu10 gu₁₀ ju10 ŋu₁₀ zu zu5 zu₅ ni 
    bi biₓ(DUG) be2 be₂ be₃ bi3 bi₃ be6 be₆ bi₆
    me ne ne2 ne₂
    ja2 ŋa₂ ga2 ga₂ za na ba
    /;

my %poss = ();
@poss{ @poss } = ();

my %poss_data = (
    'mu'  => [ 'ŋu',  ''],
    'gu10'=> [ 'ŋu',  ''],
    'ju10'=> [ 'ŋu',  ''],
    'gu₁₀'=> [ 'ŋu',  ''],
    'ŋu₁₀'=> [ 'ŋu',  ''],
    'zu'  => [ 'zu',  ''],
    'zu₅' => [ 'zu',  ''],
    'ni'  => [ 'ani', ''],
    'ne₂' => [ 'ani',  'e'],
    'be₂' => [ 'bi',  'e'],
    'bi'  => [ 'bi',  ''],
    'biₓ(DUG)'  => [ 'bi',  ''],
    'bi₃' => [ 'bi',  ''],
    'bi₆' => [ 'bi',  ''],
    'be₃' => [ 'bi',  'e'],
    'be₆' => [ 'bi',  'e'],
    'be6' => [ 'bi',  'e'],
    'bi3' => [ 'bi',  ''],
    'me'  => [ 'me', ''],
    'zunene'=> ['zunene', ''],
    'anene' => ['anene', ''],
    'ga₂' => [ 'ŋu',  'a'],
    'ŋa₂' => [ 'ŋu',  'a'],
    'za'  => [ 'zu',  'a'],
    'na'  => [ 'ani', 'a'],
    'ba'  => [ 'bi',  'a'],
);

my @post = qw/
    e
    a
    gi gin7 gin₇ de ŋen
    ce3 še₃ ac aš ec eš ec2 eš₂ ic iš uc uš
    ra ar er ir ur ur2 ur₂
    da
    ta
    /;

my %post = ();
@post{ @post } = ();

# note that we never generate LOC but default to GEN--disambiguation
# with \a is required for LOC
my %post_data = (
    'e'=>'e',
    'a'=>'a',
    'gin7'=>'gin',
    'ŋen'=>'gin',
    'gin₇'=>'gin',
    'gi-in'=>'gin',
    'de-ej3'=>'gin',
    'de-eŋ₃'=>'gin',
    'ce3'=>'eše',
    'ac'=> 'eše',
    'ac2'=> 'eše',
    'ec2'=>'eše',
    'ec'=>'eše',
    'ic'=>'eše',
    'uc'=>'eše',
    'še₃'=>'eše',
    'aš'=> 'eše',
    'aš₂'=> 'eše',
    'eš₂'=>'eše',
    'eš'=>'eše',
    'iš'=>'eše',
    'uš'=>'eše',
    'ra'=>'ra',
    'ar'=>'ra',
    'er'=>'ra',
    'ir'=>'ra',
    'ur'=>'ra',
    'ur2'=>'ra',
    'ur₂'=>'ra',
    'da'=>'da',
    'ta'=>'ta',
);

my @copg = qw/me en en3 en₃ en6 en₆ ec eš ec2 eš₂ de3 de₃ ze2 ze₂ am3 am₃ am6 am₆ 
    men3 men₃ jen ŋen/;
my %copg;
@copg{ @copg } = ();

my @nonauslaut = qw/ce3 še₃/;
my %nonauslaut;
@nonauslaut{ @nonauslaut } = ();

my %last_nsf = ('nsf'=>undef, 'ambig'=>undef);

my $original_g = '';

my $default_A = 'A';

sub
is_nsf {
    my($ix,$nsf_ref,$ambig_ref) = parse_nsf(@_);
    if ($ix) {
	$last_nsf{'nsf'} = $nsf_ref;
	$last_nsf{'post_nsf'} = $ix;
	$last_nsf{'ambig'}= $ambig_ref;
    }
    $ix;
}

sub
parse_nsf {
    my $root = shift;
    my $orth = shift;
    my @g = @_;
    my $g_index = 0;
    my @nsf = ();
    my @ambig = ();
    my $nx = -1;
    my $rest = '';
    my $auslaut = '';
    @nsf = ();

    $original_g = undef;

    warn "NSF: g#g = $g[$#g]\n"
	if $ORACC::SMA2::verbose;

    if ($g[$#g] =~ s/\\(.*)//) {
	$disambig = $1;
	warn "NSF: disambig = $disambig\n" 
	    if $ORACC::SMA2::verbose;
    }

  PARSE:
    {
	if ((($root && $root =~ /n$/)
	     || $orth && $orth =~ /n(?:\{.*?\})?$/)
	    && ($g[0] && ($g[0] eq 'na' || $g[0] eq 'ne₂'))) {
	    $nsf[0] = $original_g = $g[0];
	    $g[0] = ($g[0] =~ /a/ ? 'a' : 'e');
#	    $anlaut = 'n';
	} elsif ($g[0] 
		 && (!exists($nonauslaut{$g[0]})
		     && ( !exists($poss{$g[0]}) 
			  || ($g[1] && $g[1] =~ /^n[aie]₂?$/))
		     && ( $g[0] =~ /^(.)([ae]m?)$dig*(?:\{.*?\})?$/
			  || ((($root && $root =~ /u/)
			       || ($orth && $orth =~ /u/))
			      && $g[0] =~ /^(.?)(u)$dig*$/)))) {
	    my $anlaut = $1;
	    my $vowel = $2;
	    if ((($root && $root =~ /([^aeiu])$/))
		|| ($orth && $orth =~ /([^aeiu])$/)) {
		$auslaut = $1;

# This used to set the auslaut slot to the final letter of
# the root.  No longer used.
#		if (!$auslaut) {
#		    my $x = $root;
#		    $x =~ s/^(.*?).$//;
#		    $auslaut = $x;
#		}

		$original_g = $g[0];
		if (!length($anlaut) || $auslaut eq $anlaut) {
		    $g[0] = $vowel || 'e';
		}
		$nsf[0] = $original_g 
		    if ($anlaut && $auslaut && ($anlaut eq $auslaut));
		if ($nsf[0] && $g[1]) {
		    my $v = $nsf[0];
		    $v =~ s/^.*?(.)$/$1/;
		    if ($g[1] =~ /^$v/) {
			$nsf[0] .= $v;
		    }
		}
	    } elsif ($g[0] =~ /^u$dig$/) {
		$original_g = $g[0];
		$g[0] = 'e';
	    } else {
		$original_g = $g[0];
	    }
	} elsif ($g[0] && !exists($NSF::nsm{$g[0]})) {
	    last PARSE;
	} else {
	    unless ($original_g) {
		$original_g = $g[0] if $g[0] eq 'a';
	    }
	}
	last PARSE if $#g < 0;

	# detect and skip pleonastic vowel writings
	# like dijir-re-e-ne
	if ($#g > 0
	    && (($g[0] eq 'e' && ($g[1] eq 'e' 
				  || ($root && $root =~ /u/ && $g[1] =~ /^u$dig*$/)))
		|| ($g[0] eq 'a' && $g[1] eq 'a' && $#g > 1)
		|| $g[0] eq 'u' && $g[1] =~ /^u$dig*$/)) {
	    ++$g_index;
	}

	# NSF 1 -- GEN 1
	$nx = 1;
	($rest,$g_index,$nx) = gen($rest,$g_index,$root,$nx,@g);
	$nsf[1] = 'ak' if $nx > 1;
	my $last_nx = $nx;
	$nx = 1;
	($rest,$g_index,$nx) = gen($rest,$g_index,$root,$nx,@g);
	if ($nx > 1) {
	    # double genitive; 
	    # FIXME: this needs to be applied to other GEN ranks
	    $nsf[1] .= '.ak';
	} else {
	    $nx = $last_nx;
	}
	last PARSE if ($g_index > $#g && !length($rest));

	# NSF 2 -- POSS
	if ($nx <= 2 && $g_index <= $#g 
	    && (exists($poss{$g[$g_index]})
		|| ($g[$g_index] eq 'a' 
		    && ($g_index < $#g 
			&& $g[$g_index+1] =~ /^(na|ni|ne|ne2|ne₂)$/))
		|| ($rest eq 'a' && ($g[$g_index] =~ /^(na|ni|ne|ne2|ne₂)$/)))) {

	    ++$g_index if $g[$g_index] eq 'a';
	    if ($g[$g_index] eq 'zu'
		&& ($g_index < $#g && $g[$g_index+1] eq 'ne')) {
		$g_index += 2;
		++$g_index if ($g_index < $#g && $g[$g_index+1] eq 'ne');
		$nsf[2] = 'zunene';
		$rest = '';
		$nx = 3;
	    } elsif ($g[$g_index] eq 'ne' 
		     && ($g_index < $#g && $g[$g_index+1] eq 'ne')) {
		$g_index += 2;
		$nsf[2] = 'anene';
		$rest = '' if $rest eq 'a';
		$nx = 3;
	    } elsif ($g[$g_index] eq 'ne') {
		# single 'ne' is ignored so we pass on to plural
		$nx = 3;
	    } elsif ($g[$g_index] eq 'me') {
		# don't consider 'me' as possessive if followed by
		# en, ec, etc.
		if (($g_index < $#g
		     && exists($copg{$g[$g_index+1]}))
		    || ($ORACC::SMA2::period && $ORACC::SMA2::period eq 'UR3')) {
		    # leave it for analysis as COP
		    $nx = 5;
		} else {
		    $nsf[2] = 'me';
		    ++$g_index;
		    $nx = 3;
		}
	    } elsif ($g[$g_index] eq 'bi' && $g_index < $#g && $g[$g_index+1] eq 'da') {
		# -bi-da is always taken as .bida
		$nsf[2] = 'bida';
		$g_index += 2;
		$nx = 3;
	    } else {
		($nsf[2],$rest) = @{$poss_data{$g[$g_index]}};
		++$g_index;
		$nx = 3;
	    }
	    # trap ju/zu-u, zu-u3, zu-u8
	    if (defined($nsf[2]) && $nsf[2] =~ /u/) {
	       if ($g_index <= $#g && $g[$g_index] =~ /^u$dig*$/) {
		   $g[$g_index] = 'e';
	       }
	   }
	}
	last PARSE if ($g_index > $#g && !length($rest));

	# NSF 3 -- GEN 2
	if ($nx <= 3) {
	    $nx = 3;
	    ($rest,$g_index,$nx) = gen($rest,$g_index,$root,$nx,@g);
	    $nsf[3] = 'ak' if $nx > 3;
	}
	last PARSE if ($g_index > $#g && !length($rest));

	# NSF 4 -- PLUR 1
	if ($nx <= 4) {
	    $nx = 4;
	    ($rest,$g_index,$nx) = ene($rest,$g_index,$root,$nx,@g);
	    $nsf[4] = 'ene' if $nx > 4;
	}
	last PARSE if ($g_index > $#g && !length($rest));

	# NSF 5 -- GEN 3
	if ($nx <= 5) {
	    $nx = 5;
	    ($rest,$g_index,$nx) = gen($rest,$g_index,$root,$nx,@g);
	    $nsf[5] = 'ak' if $nx > 5;
	}
	last PARSE if ($g_index > $#g && !length($rest));

	# NSF 6 -- PLUR 2
	if ($nx <= 6) {
	    $nx = 6;
	    ($rest,$g_index,$nx) = ene($rest,$g_index,$root,$nx,@g);
	    $nsf[6] = 'ene' if $nx > 6;
	}
	last PARSE if ($g_index > $#g && !length($rest));

	if ($rest eq 'e' && $g_index <= $#g && $g[$g_index] =~ /^e[cš][2₂]?/) {
	    $rest = '';
	} elsif ($g_index < $#g && $g[$g_index] eq 'e' && $g[$g_index+1] =~ /^e[cš][2₂]?/) {
	    ++$g_index;
	}

	# NSF 7 -- POST
	if ($nx <= 7) {
	    $nx = 7;
	    my $rest_vowel = '';
	    if ($rest =~ /^([aeiu])$dig*/) {
		$rest_vowel = $1;
		if ($g_index <= $#g 
		    && ($g[$g_index] =~ /^$rest_vowel[šr]$dig*$/
			|| ($rest_vowel eq 'e' && $g[$g_index] =~ /^še₃/))) {
		    $rest = '';
		}
	    }
#	    if ($rest =~ /^e$dig*$/ 
#		&& $g_index <= $#g && $g[$g_index] =~ /^ec|eš|ec2|eš₂|ce3|še₃$/) {
#		$rest = ''; # zur-re-ec2 > zur.ece
#	    }
	    if (exists($post{$rest}) || $rest =~ s/^e$dig*$/e/) {
		$nsf[7] = $post_data{$rest};
		$rest = '';
		$nx = 8;
	    } elsif (!length($rest) && exists($post{$g[$g_index]})) {
		if ($root && $root =~ /a$dig*$/ && $g[$g_index] eq 'a') {
		    $nsf[7] = $post_data{'a'}; # fix a-ba-a > aba,e in ngrams
		    ++$g_index;
		} elsif ($g[$g_index] eq 'de' 
			 && $g_index < $#g 
			 && $g[$g_index+1] =~ /^e[jŋ][3₃]$/) {
		    $nsf[7] = $post_data{'de-ej3'};
		    $g_index += 2;
		} elsif ($g[$g_index] eq 'gi' 
			 && $g_index < $#g 
			 && $g[$g_index+1] =~ /^in$/) {
		    $nsf[7] = $post_data{'gi-in'};
		    $g_index += 2;
		} else {
		    $nsf[7] = $post_data{$g[$g_index]};
		    ++$g_index;
		}
		$nx = 8;
	    }
	    $disambig = '' unless $disambig;
	    if ($nsf[7] && $nsf[7] eq 'e' 
		&& ($disambig eq 'e' || $disambig eq 'l' || $disambig eq 'v')) {
		$nsf[7] = "$nsf[7]\U$disambig";
	    }
	}
	last PARSE if ($g_index > $#g && !length($rest));

	# NSF 8 -- COP
	if ($nx <= 8 && ($rest eq 'am' 
			 || ($g_index <= $#g 
			     && ($g[$g_index] =~ /^[aiu]m[36₃₆]?$/
				|| ($g[$g_index] eq 'nam' 
				    && ($auslaut eq 'n' 
					|| (defined($nsf[7]) 
					    && $nsf[7] eq 'gin'))))))) {
	    $nsf[8] = 'am';
	    if ($rest eq 'am') {
		$rest = '';
	    } else {
		++$g_index;
	    }
	    $nx = 9;
	} elsif ($nx <= 8 && ($g_index <= $#g && $g[$g_index] =~ /^me[cš]?$/)) {
	    if ($g[$g_index] eq 'me' 
		&& $g_index < $#g 
		&& exists($copg{$g[$g_index+1]})) {
		my $n = 0;
		($nsf[8],$n) = cop($g_index, @g);
		$g_index += $n;
	    } else {
		if ($g_index <= $#g && $g[$g_index] =~ /^e[cš][2₂]?$/) {
		    $nsf[8] = 'meš';
		    $g_index += 2;
		} else {
		    $nsf[8] = 'meš'; # default to plural; is this OK?
		    ++$g_index 
		}
	    }
	    $nx = 9;
	} elsif ($nx <= 8 && ($g_index <= $#g 
			      && $g[$g_index] =~ /^en[₃₆]?$/)) { # men3|jen|men₃|ŋen we do these with space now
	    $nsf[8] = 'men';
	    ++$g_index;
	    $nx = 9;
	}

	warn("NSF: leftover rest '$rest'\n") if length($rest);
    }

    unless ($disambig eq 'a') {
	if ($nsf[7] && $nsf[7] eq 'a') {
	    my $tmp = $nx;
	    $nsf[7] = undef;
	    while ($tmp--) {
		last if $nsf[$tmp];
	    }
	    if ($tmp <= 1) {
		$nsf[1] = 'ak';
	    } elsif ($tmp <= 5) {
		$nsf[5] = 'ak';
	    } else {
		# leave it as 'a'
		$nsf[7] = 'a';
	    }
	}
    }

    if ($g_index <= $#g) {
	@nsf = @ambig = ();
	$g_index = 0;
	return ($g_index, \@nsf, \@ambig);
    } else {
	return ($g_index, \@nsf, \@ambig);
    }
}

	#	|| $g[$g_index+1] !~ /^[aeiun]/)
sub
gen {
    my ($rest,$g_index,$root,$nx,@g) = @_;
    return ($rest,$g_index,$nx) if $g_index > $#g;
    if (($rest eq 'a' || $g[$g_index] =~ /^[ae][2₂]?$/)
	&& ($g_index < $#g 
	    && $g[$g_index+1] =~ /^(?:ka|kam|kam2|kam₂|ke4|ke₄)$/)) {
	++$g_index;
	$rest = $g[$g_index];
	$rest =~ s/^k//;
	$rest =~ s/₂$//; # kam₂ hack
	$nx = ($rest && $rest eq 'am') ? 8 : $nx+1;
	++$g_index;
    } elsif ($g[$g_index] =~ /^(?:ka|kam|kam2|kam₂|ke4|ke₄)$/) {
	$rest = $g[$g_index];
	$rest =~ s/^k//;
	$rest =~ s/[₂2]$//;
	$nx = ($rest && $rest eq 'am') ? 8 : $nx+1;
	++$g_index;
    } elsif ($g[$g_index] eq 'a'
	     && (
		 ($g_index == 0 && $#g == 0 
		  && $original_g ne 'a' && $root !~ /[aeiu]$dig*$/)
		 || ($g_index < $#g 
		     && ($g[$g_index+1] !~ /^[aeiun]/
			 || $g[$g_index+1] =~ /^a[cš]$dig*$/)))
	)
		 {
	$rest = '';
	++$nx;
	++$g_index;
    } elsif ($rest eq 'a' && $nx > 2 && $g[$g_index] !~ /^[aeiun]/) {
	$rest = '';
	++$nx;
    }
    ($rest,$g_index,$nx);
}

sub
ene {
    my ($rest,$g_index,$root,$nx,@g) = @_;
    if ($g_index <= $#g && $g[$g_index] eq 'ne') {
	$rest = '';
	++$g_index;
	++$nx;
    } elsif ($g_index <= $#g && $g[$g_index] eq 'e' 
	     && ($g_index < $#g && $g[$g_index+1] eq 'ne')) {
	$rest = '';
	$g_index += 2;
	++$nx;
    } elsif ($g_index <= $#g && $g[$g_index] eq 'e' 
	     && ($g_index < $#g && $g[$g_index+1] eq 'na')) {
	$g_index += 2;
	$rest = 'a';
	++$nx;
    }
    ($rest,$g_index,$nx);
}

sub
cop {
    my ($g_index,@g) = @_;
    my $cop = '';
    my $len = 1;
    ++$g_index; # we know the first g is 'me'
    if ($g[$g_index] =~ /^en$dig*$/) {
	++$g_index;
	++$len;
	if ($g_index <= $#g && $g[$g_index] =~ /^de$dig*$/) {
	    $cop = 'menden';
	    ++$len;
	    ++$g_index;
	    ++$len if $g_index <= $#g && $g[$g_index] =~ /^en$dig*$/;
	} elsif ($g_index <= $#g && $g[$g_index] =~ /^ze$dig*$/) {
	    $cop = 'menzen';
	    ++$len;
	    ++$g_index;
	    ++$len if $g_index <= $#g && $g[$g_index] =~ /^en$dig*$/;
	} elsif ($g_index <= $#g && $g[$g_index] =~ /^ec$dig*$/) {
	    $cop = 'mec';
	    ++$len;
	} else {
	    $cop = 'men';
	    ++$len;
	}
    } elsif ($g[$g_index] =~ /^ze$dig*$/) {
	$cop = 'menzen';
	++$len;
    } elsif ($g[$g_index] =~ /^e[cš]$dig*$/) {
	$cop = 'meš';
	++$len;
    } else {
	$cop = 'me';
	++$len;
    }
    ($cop,$len);
}

sub
get_last_nsf {
    return \%last_nsf;
}

1;
