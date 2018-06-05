package ORACC::CBD::Corpus;
require Exporter;
@ISA=qw/Exporter/;
@EXPORT = qw/pp_fix_file pp_fix_new_atf pp_fix_grand_total pp_fix_close_log/;

use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

use ORACC::CBD::Fix;
use ORACC::CBD::PPWarn;
use Data::Dumper;

$ORACC::CBD::Corpus::nopos = 1;

my %fixes = ();
my %fixes_made = ();
my $lexical = 0;

my %lextags = (); @lextags{qw/cs eq pr sg sn sv tx wp/} = ();

my %langtags = ();
my @langshort = qw/2   3   a   ag  an  e   es  g   n   s   ob  sb  ugag/;
my @langlong =  qw/akk akk akk akk akk sux sux akk akk sux akk akk uga/;
@langtags{@langshort} = @langlong;


my $log_open = 0;
my $no_fix_warned = 0;
my $grand_total_fixes = 0;
my $total_fixes = 0;
my $trace = 0;

sub pp_fix_close_log {
    close(FIXLOG);
}

sub pp_fix_grand_total {
    $grand_total_fixes;
}

sub pp_fix_file {
    my($args, $file) = @_;
    open(FIXLOG,'>cbdfixatf.log') unless $log_open++;
    if ($$args{'fix'}) {
	%fixes = pp_fix_load_fixes($args, undef) unless scalar keys %fixes;
    } else {
	warn "$0: proceeding without fixes table\n" unless $no_fix_warned++;
    }
    corpus_fixes();
    if ($ORACC::CBD::PPWarn::trace) {
	open(D,'>fixes.dump');
	print D Dumper \%fixes;
	close(D);
    }
    my %f = file_words($args,$file);
    %f = apply_fixes(\%f,$file);
    if ($total_fixes) {
	foreach my $f (sort keys %fixes_made) {
	    print FIXLOG "$file: $f output as a fix $fixes_made{$f} times\n";
	}
	%fixes_made = ();
	$grand_total_fixes += $total_fixes;
	$total_fixes = 0;
    } else {
	%f = ();
    }
    %f;
}

sub pp_fix_new_atf {
    my($file,$outf,$fixedref) = @_;
    my @lines = @{$$fixedref{'lines'}};
    my @lemma = @{$$fixedref{'lemma'}};
    
    open(O, ">$outf") || die "$0: unable to open $outf for output\n";
    # print O Dumper $h;
    for (my $i = 0; $i <= $#lines; ++$i) {
	if ($lines[$i] =~ /^\#lem/ && $lemma[$i] ne "\000") {
	    print O 
		'#lem: ', 
		join ('; ', grep defined&&length, (map { ${$_}{'lem'} } @{$lemma[$i]})),
		"\n";
	} else {
	    print O $lines[$i], "\n";
	}
    }
    close(O);
    warn "$file: new version written to $outf\n";
}

sub apply_fixes {
    my($f,$file) = @_;
    my %f = %$f;
    my @newlem = ();
    my $line = 0;
    foreach my $lref (@{$f{'lemma'}}) {
	++$line;
	if (!ref($lref)) {
	    push @newlem, $lref; # "\000"
	} else {
	    my @wds = @{$lref};
	    foreach my $w (@wds) {
		next unless $$w{'lem'};
		my $inst = $$w{'lem'};
		my $lang = "$$w{'lang'}:";
		my ($pre,$lem,$post) = ($inst =~ /^(.*?)(\S+\[.*?\]\S*)(.*)$/);

		if ($lem) {
		    $pre = '' unless $pre;
		    $post = '' unless $post;
		    $lem =~ s/\[\]/[1]/;
		    if (!$lang) {
			warn "$file:$line: unknown language in effect for '$lem'\n";
			$$w{'lem'} = $inst;
		    } elsif ($fixes{$lang.$lem}) {
			my $fix = $fixes{$lang.$lem};
			++$total_fixes;
			++$fixes_made{$fix};
			$fix =~ s/^.*?://;
			if ($ORACC::CBD::Corpus::nopos) {
			    $fix =~ s/\].*/]/;
			} else {
			    $fix =~ s/\].*/]/ unless $lem =~ /\]\S/; # preserve POS etc.
			}
			my $ninst = "$pre$fix$post";
			print FIXLOG "$file:$line: $inst => $ninst\n";
			$$w{'lem'} = $ninst;
		    } else {
			my $sl = nopos_form($lem);
			warn "trying nopos form $sl\n" if $ORACC::CBD::PPWarn::trace;
			my $fix = $fixes{$lang.$sl};
			unless ($fix) {
			    $sl = short_form($lem);
			    warn "trying nopos form $sl\n" if $ORACC::CBD::PPWarn::trace;
			    $fix = $fixes{$lang.$sl};
			}			
			if ($fix) {
			    ++$total_fixes;
			    ++$fixes_made{$fix};			
			    $fix =~ s/^.*?://;
			    if ($ORACC::CBD::Corpus::nopos) {
				$fix =~ s/\].*/]/;
			    } else {
				$fix =~ s/\].*/]/ unless $lem =~ /\]\S/; # preserve POS etc.
			    }
			    my $ninst = "$pre$fix$post";
			    print FIXLOG "$inst => $ninst\n";
			    $$w{'lem'} = $ninst;
			}
		    }
		} else {
		    # u n X L M S etc.
		    $$w{'lem'} = $inst;
		}
	    }
	    push @newlem, [ @wds ];
	}
    }
    @{$f{'lemma'}} = @newlem;
    %f;
}

sub nopos_form {
    my $x = shift;
    $x =~ s/\].*$/]/; # match on form without POS; possibly keep some in output?
    $x =~ s/\s*\[/[/;
    $x =~ s/\]\s*/]/;
    $x;
}

sub short_form {
    my $x = shift;
    $x =~ s#//.*?\]#]#;
    $x =~ s/\].*$/]/; # match on form without POS; possibly keep some in output?
    $x =~ s/\s*\[/[/;
    $x =~ s/\]\s*/]/;
    $x;
}

# Return a hash consisting of a list of original lines and a
# corresponding list of the lines broken into words.  The second list
# only has entries for #lem lines, but has empty cells for other lines.
#
# Each cell that is a list of words consists of a list of anonymous hashes,
# one per lemma.  The list preserves meta-lemmatization features and the
# #lem line can be restructed by concatenating the 'lem' members of the
# word hashes.  The hash also contains the word-form associated with the
# lemmatization and the word's language.

sub file_words {
    my($args,$f) = @_;
    my %f = ();
    my @f = ();
    open(F,$f) || die "$0: unable to open $f for input\n";
    @f = (<F>);
    close(F);
    chomp @f;
    @{$f{'lines'}} = @f;
    @{$f{'lemma'}} = lem_file($f,@f);
    %f;
}

sub lem_file {
    my ($file,@f) = @_;
    my $curr_lang = '';
    my $text_lang = '';
    my $curr_line = '';
    my %sparse = ();
    my $use_sparse = 0;
    for (my $i = 0; $i <= $#f; ++$i) {
	if ($f[$i] =~ /^\#lem:/) {
	    $f[$i] = lem_hashes($curr_line,$f[$i],$curr_lang,
				($use_sparse?\%sparse:undef),
				$file,$i+1);
	    $curr_line = '';
	} else {
	    local($_) = $f[$i];
	    if (/^\#lemmatizer:\s+sparse\s+do\s+(.*?)\s*$/) {
		my $sparse = $1;
		@sparse{split(/\s+/, $sparse)} = ();
		$lexical = 1;
		$use_sparse = 1;
	    } elsif (/^\#atf:\s+lang\s+(\S+)\s*$/) {
		$curr_lang = $text_lang = $1;
	    } elsif (/^\#atf:\s+use\s+lexical\s*$/
		     || /^\#project:\s+dcclt/) {
		$lexical = 1;
	    } elsif (/^[^\s\$\#\@]\S*?\.(?=\s|$)/) {
		$curr_line = $f[$i];
		$curr_lang = $text_lang;
	    } elsif (/^==/) {
		$curr_line = $f[$i];
		$curr_line =~ s/^==/b. /;
		$curr_lang = $text_lang;
	    } elsif (/^\&/) {
		$curr_lang = $text_lang = 'sux';
		$curr_line = '';
		$lexical = $use_sparse = 0;
		%sparse = ();
	    }
	    $f[$i] = "\000";
	}
    }
    @f;
}

sub lem_hashes {
    my($wds,$lem,$lang,$sparseref,$file,$line) = @_;
    my @wds = split_wds($wds,$lang,$file,$line);
    my @lem = split_lem($lem,$sparseref,$file,$line,@wds);

    if ($#lem < 0) { #return empty list on failure in split_lem
	return "\000";
    } else {
	return [ @lem ];
    }
}

sub split_wds {
    my ($l,$lang,$file,$line) = @_;

    if ($lexical) {
	1 while ($l =~ s/\s+([@!#="\|\^~])\s+/lex_shorthands($1)/e);
    }

    # remove punctuation
    1 while $l =~ s/\s[\[⸢<({]*(?:\*|:\.|::|:)\S*/ /;

    # remove & cell markers and their optional colspans
    $l =~ s/\s\&\d*/ /g;

    # move gloss/meta after the word that is enclosing it in constructs
    # like a-{{ba}}-bi
    $l =~ s/(\S+)(\{\{.*?\}\})(\S+)/$1$3$2/g;

    # unwrap surrogates like MIN<(a)> and remove them from containing words if necessary
    1 while $l =~ s/(?:KIMIN|kimin|KI.MIN|MIN|ŠU|\d+)(?:\S*?)<\((.*?)\)>/$1/;

    # remove corrections
    1 while $l =~ s/\!\(.*?\)//g;
    
    $l =~ s/\(\#.*?\#\)//g;
    $l =~ s/\(\$.*?\$\)//g;
    $l =~ s/\{\{/ /g;
    $l =~ s/\}\}/ /g;
    $l =~ s/\{\(/ /g;
    $l =~ s/\)\}/ /g;
    $l =~ s/^\S+\s+//;
    $l =~ s/\s$//;
    $l =~ s/<<.*?>>//g;
    $l =~ s/--/-/;
    $l =~ s/\!([a-z][a-z])\s/%$1 /g; # hack !sn to %sn
    $l =~ tr/-:. a-zA-Z0-9ḫḪšŠṣṢṭṬʾŋŊ₀-₉ₓ\|\@&~%{}()//cd; ## FIXME: not good enough for some langs
    $l =~ s/\s+/ /g;
    $l =~ s/\(::\)//g; # for etcsl
    #  my @line = grep(defined&&length&&!/^%/&&!/^\d+::\d+/ , split(/\s+/, $l));
    my @l = grep(defined&&length&&!/^\d+::\d+/ , split(/\s+/, $l));

    if ($ORACC::CBD::PPWarn::trace) {
	warn "words = ", join('::', @l), "\n";
    }
    
    # now factor out langs and lex field codes and build list of word hashes
    my $curr_field = '';
    my $curr_lang = $lang;
    my @wds = ();
    for (my $i = 0; $i <= $#l; ++$i) {
	if ($l[$i] =~ /^\%(.*?)$/) {
	    my $tag = $1;
	    if (exists $lextags{$tag}) {
		$curr_field = $tag;
	    } else {
		if ($tag =~ s/-.*$// || length($tag) == 3) {
		    $curr_lang = $tag;
		} else {
		    if ($langtags{$tag}) {
			$curr_lang = $langtags{$tag};
		    } else {
			warn "$file:$line: unknown language tag '$tag'\n";
		    }
		}
	    }
	} else {
	    push @wds, { w=>$l[$i], lang=>$curr_lang, field=>$curr_field };
	}
    }
    @wds;
}

sub split_lem {
    my ($lem,$sparseref,$file,$line,@wds) = @_;
    chomp $lem;
    $lem =~ s/^\S*\s+//;
    $lem =~ s/\s$//;
    my @lem = grep(defined&&length, split(/;\s+/, $lem));

    if ($#lem < 0) {
	# existing practice in ATF is that #lem: with no lemmata is ignored
	# rather than generating a too-few-lemmata error, so we emulate that
	# behaviour here.
	return ();
    }

    if ($ORACC::CBD::PPWarn::trace) {
	warn "lemma = ", join('::', @lem), "\n";
    }

    #
    # iterate over words with $i and lemmata with $j
    # in sparse mode, these can be different; it's only an error if
    # we don't use all the @lem cells or if there are words that are
    # lemmatizable after we have used all the @lems.
    #
    my $j = 0;
    my @nw = ();
    for (my $i = 0; $i <= $#wds; ++$i) {
	my %w = %{$wds[$i]};
#	print Dumper \%w;
	# if field is null then default is word and is always lemmatizable
	if (!$sparseref || !$w{'field'} || exists($$sparseref{$w{'field'}})) {
	    if ($lem[$j]) {
		$w{'lem'} = $lem[$j++];
		push @nw, { %w };
	    } else {
		# we have more words than lemmata
		warn "$file:$line: too few lemmata\n";
		return ();
	    }
	} else {
	    push @nw, { %w };
	}
    }
    if ($j < $#lem) {
	# we had more lemmata than words
	warn "$file:$line: too many lemmata\n";
	return ();
    }
#    print Dumper \@nw;
    return @nw;
}

sub lex_shorthands {
    my $ch = shift;
    my $fld = '';
    if ($ch eq  '=') {
	$fld = "!eq";
    } elsif ($ch eq  '#') {
	$fld = "!sv";
    } elsif ($ch eq  '"') {
	$fld = "!pr";
    } elsif ($ch eq  '~') {
	$fld = "!sg";
    } elsif ($ch eq  '|') {
	$fld = "!sn";
    } elsif ($ch eq  '^') {
	$fld = "!wp";
    } elsif ($ch eq  '@') {
	$fld = "!cs";
    } else {
	$ch = '';
    }
    " $fld ";
}

sub corpus_fixes {
    foreach my $k (keys %fixes) {
	my $v = $fixes{$k};
	$v =~ s/\s+\[/[/;
	$v =~ s/\]\s+/]/;
	$fixes{$k} = $v;
	my $orig = $k;
	$k =~ s/\s+\[/[/;
	$k =~ s/\]\s+/]/;
	$fixes{$k} = $fixes{$orig};
	$k =~ s/\].*$/]/;
	$fixes{$k} = $fixes{$orig};
    }
}

1;
