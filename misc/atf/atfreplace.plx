#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use Data::Dumper;
use constant {
    PRE_JUNK => 0,
    POST_JUNK => 1,
    PRE_DELIM => 2,
    POST_DELIM => 3,
    GRAPHEME => 4,
    FLAGS => 5,
    CELL_OR_FIELD => 6,
};

my $curr_lang = '';
my $flag_chars = '[\#?!*]';
my $grapheme_chars = '[a-zA-Z0-9₀-₉áéíúàèìùÁÉÍÚÀÈÌÙšŠḫḪŋŊṣṢṭṬʾʿ`\',…@~]';
my $pre_junk_chars = '[\[(<\|]';
my $post_junk_chars = '[\])>\|\/]';
my $pre_delim_chars = '\{[\{\+]?';
my $post_delim_chars = '[-.:;\}_+]';
my $showinsts = 0;
my $tok = '';

my $match_count;
my %match_indexes = ();
my @word_segments = ();

use Getopt::Long;

my $dir = '';
my $fixlist = '';
my $fixpat = '';
my $identity = '';
my %replace = ();
my $skipping = 0;
my $test = 0;
my $verbose = 0;

GetOptions(
    'dir:s'=>\$dir,
    'identity'=>\$identity,
    'list:s'=>\$fixlist,
    'test'=>\$test,
    'verbose'=>\$verbose,
    );

load_fixlist();
$fixpat = create_fixrexp();

if ($test && !$identity) {
    while (<DATA>) {
	process_line();
    }
} elsif ($dir) {
    my @files = `find $dir -name '*.atf'`; chomp @files;
    system 'rm', '-fr', 'atfreplaced';
    system 'mkdir', '-p', 'atfreplaced';
    foreach my $f (@files) {
	my $fdir = $f;
	$fdir =~ s#/([^/]+\.atf)##;
	my $fbase = $1;
	$fdir =~ s#^[^/]+##;
	system 'mkdir', '-p', "atfreplaced/$fdir";
	my $outf = "atfreplaced/$fbase";
	open(IN,$f) || die "atfreplace.plx: can't read `$f'\n";
	open(OUT,">$outf") || die "atfreplace.plx: can't write `$outf'\n"; select OUT;
	warn("$f -> $outf\n");
	while (<IN>) {
	    process_line();
	}
	close(IN);
	close(OUT);
	if ($test && $identity) {
	    system 'diff', '-w', '-q', $f, $outf;
	}
    }
} else {
    while (<>) {
	process_line();
    }
}

sub lang_of {
    my $l = shift;
    if (length($l) >= 3) {
	return $l;
    } else {
	my %l = (a=>'akk',
		 e=>'sux-x-emesal',
		 eg=>'sux-x-emegir',
		 es=>'sux-x-emesal',
		 ob=>'akk-x-olddbab'
		 s=>'sux'
		 sb=>'akk-x-stdbab',
	    );
	if ($l{$l}) {
	    $l{$l};
	} else {
	    warn "ignoring unknown language abbreviation `\%$l'\n";
	    return $l;
	}
    }
}

sub
process_line {
    if (/^[\&\@\#\$]/) {
	if (/^\@translation/) {
	    $skipping = 1;
	} elsif (/^\&/) {
	    $skipping = 0;
	}
	print;
    } elsif (/^\s*$/) {
	print;
    } elsif (!$skipping) {
	s/\.\.\./…/g;
	my @words = split(/\s+/,$_);
	my @neww = ();
	my $leading_junk = '';
	print shift @words;
	print "\t";
	for (my $i = 0; $i <= $#words; ++$i) {
	    my $w = $words[$i];
	    if ($w =~ /^\%(\S+)/) {
		my $lang = $1;
		$curr_lang_atf = lang_of($lang);
		push @neww, $w;
		next;
	    }
	    if ($w !~ /$grapheme_chars/o) {
		push @neww, $w;
		next;
	    }
	    if ($w =~ /^\(\$/ || $w =~ /^\(\#/) {
		while ($w !~ /[\#\$]\)/) {
		    push @neww, $w;
		    $w = $words[++$i];
		}
		push @neww, $w;
		next;
	    } else {
		if ($w =~ s/^($pre_junk_chars+)//o) {
		    $leading_junk = $1;
		    if ($w =~ s/^($post_junk_chars+)//o) {
			$leading_junk .= $1;
		    }
		} else {
		    $leading_junk = '';
		}
		my $matchw = setup_w_match($w);
		print STDERR "matchw = $matchw\n" if $verbose;
		unless ($identity) {
		    do {
			$match_count = 0;
			$matchw =~ s/(^|.*?[-\cA:;])$fixpat($|\cA)$/&fixrep($1,$2)/eg;
		    } while ($match_count);
		}
	    }
	    push @neww, recompose_w($leading_junk);
	}
	my $line = join ' ', @neww;
	$line =~ s/…/\.\.\./g;
	print "$line\n";
    } else {
	print;
    }
}

###################################################################

sub
controlify {
    my $x = shift;
    $x =~ s/([-.:;{])/\cA$1/g;
    $x =~ s/\}/\}\cA/g;
    $x;
}

sub
create_fixrexp {
    my $r = '(';
    $r .= join('|', map { $_ = controlify($_); s/^\cA//; $_; } sort { length($b)<=>length($a) } keys %replace);
    $r .= ')';
    print STDERR "regexp: $r\n" if $verbose;
    return $r;
}

sub
fixrep {
    my($pre,$rep) = @_;
    my $new = '';
    $pre =~ tr/\cA//d;
    $rep =~ tr/\cA//d;
    my @fix = @{$replace{$rep}};
    my $rep_skip = $match_indexes{length($pre)};
    if (!defined $rep_skip) {
	print STDERR "no entry in match_indexes for $pre with length ", length($pre),"\n";
	print STDERR Dumper(\%match_indexes);
	exit 1;
    }
    my @rep = @word_segments[$rep_skip..$#word_segments];

#    print STDERR Dumper(\@fix);
#    print STDERR Dumper(\@rep);
#    exit 1;

    print STDERR "\@fix = @fix\n\@rep = @rep\nrep_skip=$rep_skip\n\@rep used = @rep\n" if $verbose;
    for (my $i = 0; $i <= $#rep; ++$i) {
	my %new = ();
	$new{'g'} = ${$fix[$i]}{'g'} || '';
	$new{'pre_delim'} = ${$fix[$i]}{'pre_delim'} || '';
	$new{'post_delim'} = ${$fix[$i]}{'post_delim'} || '';
	$new{'flags'} = ${$rep[$i]}{'flags'} || '';
	$new{'pre_junk'} = ${$rep[$i]}{'pre_junk'} || '';
	$new{'pre_delim_junk'} = ${$rep[$i]}{'pre_delim_junk'} || '';
	$new{'pre_flags_junk'} = ${$rep[$i]}{'pre_flags_junk'} || '';
	$new{'post_junk'} = ${$rep[$i]}{'post_junk'} || '';
	$new{'post_delim_junk'} = ${$rep[$i]}{'post_delim_junk'} || '';
	$new{'post_junk_delim'} = ${$rep[$i]}{'post_junk_delim'} || '';
	$word_segments[$rep_skip+$i] = { %new };
	if (${$fix[$i]}{'leading_delim'} && ($rep_skip+$i)) {
	    ${$word_segments[$rep_skip+$i-1]}{'post_junk_delim'} = ${$fix[$i]}{'leading_delim'};
	}
	if ($verbose) {
	    print STDERR "===fix[$i]====\n";
	    print STDERR Dumper($fix[$i]);
	    print STDERR "===rep[$i]====\n";
	    print STDERR Dumper($rep[$i]);
	    print STDERR "===new[$i]====\n";
	    print STDERR Dumper(\%new);
	}
	$new .= "$new{'pre_delim'}$new{'g'}$new{'post_delim'}";
    }
    $new = controlify($new);
    "$pre$new";
}

sub
get_token {
    if ($tok =~ s/^($grapheme_chars+)//o) {
	return [ GRAPHEME , $1 ];
    } elsif ($tok =~ s/^($post_junk_chars+)//o) {
	return [ POST_JUNK , $1 ];
    } elsif ($tok =~ s/^($pre_junk_chars+)//o) {
	return [ PRE_JUNK , $1 ];
    } elsif ($tok =~ s/^($pre_delim_chars)//o) {
	return [ PRE_DELIM , $1 ];
    } elsif ($tok =~ s/^($post_delim_chars)//o) {
	return [ POST_DELIM , $1 ];
    } elsif ($tok =~ s/^($flag_chars+)//o) {
	return [ FLAGS , $1 ];
    } elsif (length $tok) {
	$tok =~ s/^(.)//;
	warn "$.: character `$1' not known to tokenizer\n";
	return undef;
    } else {
	return undef;
    }
}

sub
init_token {
    $tok = shift;
}

sub
load_fixlist {
    open(F,$fixlist) || die "atfreplace.plx: can't open `$fixlist'\n";
    while (<F>) {
	my($from,$to) = (/^(\S+)\s+(\S+)\s*$/);
	die "$fixlist:$.: syntax error--expected format is WORDTABWORD\n"
	    unless $from && $to;
	save_replace($from,$to);
    }
    close(F);
}

sub
recompose_w {
    my $leading_junk = shift;
    my @neww = ();
    push @neww, $leading_junk if $leading_junk;
    for (my $i = 0; $i <= $#word_segments; ++$i) {
	my %x = %{$word_segments[$i]};
	print STDERR "===x[$i]===\n", Dumper(\%x) if $verbose;
	push @neww, "$x{'pre_delim_junk'}$x{'pre_delim'}$x{'pre_junk'}$x{'g'}$x{'pre_flags_junk'}$x{'flags'}$x{'post_junk'}$x{'post_delim'}$x{'post_delim_junk'}$x{'post_junk_delim'}";
    }
    return join('',@neww);
}

sub
save_replace {
    my($from,$to) = @_;
    warn "$fixlist:$.: from/to are identical, skipping\n" and return
	if $from eq $to;
    my @from = split_graphemes($from);
    my @to = split_graphemes($to);
    warn "$fixlist:$.: from/to lists are different lengths, skipping\n" and return
	if $#from != $#to;
    $replace{$from} = [ @to ];
}

sub
setup_w_match {
    my $w = shift;
#    warn "$w\n";
    @word_segments = split_graphemes($w);
    my @segs = map { join('',@{$_}{qw/pre_delim g post_delim/}) } @word_segments;
    my $mw = '';
    for (my $i = 0; $i <= $#segs; ++$i) {
	$match_indexes{length($mw)} = $i;
	$mw .= $segs[$i];
    }
    print STDERR Dumper(\%match_indexes) if $verbose;
    return controlify($mw);
}

sub
split_graphemes {
    my @flags = ();
    my @pre_junk = ();
    my @post_junk = ();
    my @pre_delim = ();
    my @pre_delim_junk = ();
    my @pre_flags_junk = ();
    my @post_delim = ();
    my @post_junk_delim = (); # }>- => post_delim post_delim_junk post_junk_delim
    my @post_delim_junk = ();
    my @graphemes = ();
    my @data = ();
    my $index = 0;
    my $s = '';
    my $leading_delim = '';
    my $leading_junk = '';

    init_token($_[0]);
    while ($s = get_token()) {
	if ($$s[0] eq PRE_JUNK) {
	    if ($pre_delim[$index]) {
		$pre_junk[$index] = $$s[1];
	    } else {
		$pre_delim_junk[$index] = $$s[1];
	    }
	} elsif ($$s[0] eq POST_JUNK) {
	    if ($flags[$index-1]) {
		if ($post_delim[$index-1]) {
		    $post_delim_junk[$index-1] = $$s[1];
		} else {
		    $post_junk[$index-1] = $$s[1];
		}
	    } else {
		if ($post_delim[$index-1]) {
		    $post_delim_junk[$index-1] = $$s[1];
		} else {
		    $pre_flags_junk[$index-1] = $$s[1];
		}
	    }
	} elsif ($$s[0] eq PRE_DELIM) {
	    $pre_delim[$index] = $$s[1];
	} elsif ($$s[0] eq POST_DELIM) {
	    $$s[1] =~ tr/_/ /;
	    if ($index) {
		if ($post_delim[$index-1]) {
		    $post_junk_delim[$index-1] = $$s[1];
		} else {
		    $post_delim[$index-1] = $$s[1];
		}
	    } else {
		$leading_delim = $$s[1];
	    }
	} elsif ($$s[0] eq GRAPHEME) {
	    $graphemes[$index] = $$s[1];
	    ++$index;
	} elsif ($$s[0] eq FLAGS) {
	    $flags[$index-1] = $$s[1];
	} else {
	    warn "atfreplace.plx: unhandled token $$s[0] = $$s[1]\n";
	}
    }

    for (my $i = 0; $i <= $#graphemes; ++$i) {
	my %g = ();
	$g{'g'} = $graphemes[$i];
	$g{'pre_junk'} = $pre_junk[$i] || '';
	$g{'post_junk'} = $post_junk[$i] || '';
	$g{'pre_delim'} = $pre_delim[$i] || '';
	$g{'pre_delim_junk'} = $pre_delim_junk[$i] || '';
	$g{'pre_flags_junk'} = $pre_flags_junk[$i] || '';
	$g{'post_delim'} = $post_delim[$i] || '';
	$g{'post_delim_junk'} = $post_delim_junk[$i] || '';
	$g{'post_junk_delim'} = $post_junk_delim[$i] || '';
	$g{'flags'} = $flags[$i] || '';
	push @data, { %g };
    }

    ${$data[0]}{'leading_delim'} = $leading_delim;
    ${$data[0]}{'leading_junk'} = $leading_junk;
    if (!$graphemes[$index] && ($post_junk[$index] || $post_delim[$index])) {
	${$data[$#data]}{'trailing_delim'} = $post_delim[$index];
	${$data[$#data]}{'trailing_junk'} = $post_junk[$index];
    }

    return @data;
}

1;

#############################################################################
__DATA__
&P121212=ATF Replace Test Data
1. INNIN
2. INNIN#
3. DUMU-MI2
4. {m}e2-kur
5. [BA#-sza2?]-<(an-ni)>
6. {m}{d}60-BA]-sza2#-an-ni
7. TIL{mesz}
8. KU3-BABBAR-a4
9. {lu2}LUNGA-u2-tu2
10. SZU{II}
