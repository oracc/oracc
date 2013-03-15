package ORACC::CATF;
use warnings; use strict; use utf8;

sub
catf_fixer {
    fix_text(@_);
}

########################################################################

my @text = ();

sub
fix_misc {
    for (my $i = 0; $i <= $#text; ++$i) {
	$text[$i] =~ s/\+\[x/-[x/g;
	$text[$i] =~ s/x\]\+/x]-/g;
    }
}

sub
fix_text {
    @text = @_;
    fix_misc();
    fix_uscore();
    fix_uchars();
    map { "$_\n" } @text;
}

sub
fix_uchars {
    my %digram = (
	ṣ=>'s,',
	Ṣ=>'S,',
	š=>'sz',
	Š=>'SZ',
	ṭ=>'t,',
	Ṭ=>'T,',
	ḫ=>'h',
	Ḫ=>'H',
	ŋ=>'j',
	Ŋ=>'J',
	'ä'=>'ae',
	'ö'=>'oe',
	'ü'=>'ue',
	);
    
    for (my $i = 0; $i <= $#text; ++$i) {
	$text[$i] =~ tr/₀-₉ₓʾ×/0-9x'x/;
	$text[$i] =~ s/([äöüṣṢšŠṭṬḫḪŋŊ])/$digram{$1}/eg;
    }
}

sub
fix_uscore {
    for (my $i = 0; $i <= $#text; ++$i) {
	if ($text[$i] =~ /\@_/) {
	    $text[$i] =~ s/\!_\@(\(.*?\))/\!$1_\@/g; # fix IR3!(TI) where IR3 is a singleton logogram and therefore not grouped
	    my @u = grep length, split(/(\@_.*?_\@\s*)/, $text[$i]);
	    my $needs_replace = 0;
	    for (my $j = 0; $j <= $#u; ++$j) {
		if ($u[$j] =~/\@_(\%\S+)?/) {
		    my $lang1 = $1 || '';
		    my $dup_lang = 0;
		    my $k = $j+1;
		    while ($k <= $#u && ($u[$k] =~ /^[-\.:]$/ || $u[$k] =~ /^\@_(\%\S+)?/)) {
			if ($u[$k] =~ /^[-\.:]/) {
			    ++$k;
			} else {
			    my $lang2 = $1 || '';
			    if ($lang1 eq $lang2) {
				$u[$k] =~ s/^\@_(?:\%\S+)?//;
				$dup_lang = 1;
				++$k;
			    } else {
				last;
			    }
			}
		    }
		    if ($dup_lang) {
			$u[$j] =~ s/^\@//;
			for (my $j2 = $j; $j2 < ($k-1); ++$j2) {
			    $u[$j2] =~ s/_\@//;
			}
			$u[$k-1] =~ s/_\@/_/;
		    } else {
			$u[$j] =~ s/\@_/_/;
			$u[$j] =~ s/_\@/_/;
		    }
		    ++$needs_replace;
		}
	    }
	    if ($needs_replace) {
		my $rep = join('', @u);
		$rep =~ s/ +/ /g;
		$rep =~ s/\s+$//;
		$text[$i] = $rep;
	    }
	}
    }
}

1;
