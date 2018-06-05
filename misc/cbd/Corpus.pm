package ORACC::CBD::Corpus;
require Exporter;
@ISA=qw/Exporter/;
@EXPORT = qw/pp_fix_file pp_fix_new_atf pp_fix_grand_total pp_fix_close_log/;

use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

use ORACC::CBD::Fix;
use ORACC::CBD::PPWarn;
use Data::Dumper;

my %fixes = ();
my %fixes_made = ();
my $lexical = 0;

my %lextags = (); @lextags{qw/eq pr sg sn sv tx wp/} = ();

my %langtags = ();
my @langshort = qw/a e s ob/;
my @langlong = qw/akk sux sux akk/;
@langtags{@langshort} = @langlong;


my $log_open = 0;
my $no_fix_warned = 0;
my $grand_total_fixes = 0;
my $total_fixes = 0;

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
#    print Dumper \%fixes;
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
    my($file,$outf,$h) = @_;
    open(O, ">$outf") || die "$0: unable to open $outf for output\n";
    print O Dumper $h;
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
		next unless $lem; # u n X L M S etc.
		$pre = '' unless $pre;
		$post = '' unless $post;
		if (!$lang) {
		    warn "$file:$line: unknown language in effect for '$lem'\n";
		    next;
		}
		if ($fixes{$lang.$lem}) {
		    my $fix = $fixes{$lang.$lem};
		    ++$total_fixes;
		    ++$fixes_made{$fix};
		    $fix =~ s/^.*?://;
		    $fix =~ s/\].*/]/ unless $lem =~ /\]\S/;
		    my $ninst = "$pre$fix$post";
		    print FIXLOG "$file:$line: $inst => $ninst\n";
		    $$w{'lem'} = $ninst;
		} else {
		    my $sl = short_form($lem);
		    if ($fixes{$lang.$sl}) {
			my $fix = $fixes{$lang.$sl};
			++$total_fixes;
			++$fixes_made{$fix};			
			$fix =~ s/^.*?://;
			$fix =~ s/\].*/]/ unless $lem =~ /\]\S/;
			my $ninst = "$pre$fix$post";
			print FIXLOG "$inst => $ninst\n";
			$$w{'lem'} = $ninst;
		    }
		}
	    }
	    push @newlem, [ @wds ];
	}
    }
    @{$f{'lemma'}} = @newlem;
    %f;
}

sub short_form {
    my $x = shift;
    $x =~ s#//.*?\]#]#;
    $x =~ s/\](\s+\S+).*$/]$1/;
    $x =~ s/\s*\[/ [/;
    $x =~ s/\]\s*/] /;
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
	    } elsif (/^\#atf:\s+use\s+lexical\s*$/) {
		$lexical = 1;
	    } elsif (/^[^\s\$\#\@]\S*?\.(?=\s|$)/) {
		$curr_line = $f[$i];
		$curr_lang = $text_lang;
	    } elsif (/^\&/) {
		$curr_lang = $text_lang = 'sux';
		$curr_line = '';
		$lexical = $use_sparse = 0;
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
    
    $l =~ s/\(\#.*?\#\)//g;
    $l =~ s/\(\$.*?\$\)//g;
    $l =~ s/\{\{/ /g;
    $l =~ s/\}\}/ /g;
    $l =~ s/^\S+\s+//;
    $l =~ s/\s$//;
    $l =~ s/<<.*?>>//g;
    $l =~ s/--/-/;
    $l =~ s/\!([a-z][a-z])\s/%$1 /g; # hack !sn to %sn
    $l =~ tr/-:. a-zA-Z0-9šŋŠŊ₀-₉ₓ\|\@&~%{}()//cd;
    $l =~ s/\s+/ /g;
    $l =~ s/\(::\)//g; # for etcsl
    #  my @line = grep(defined&&length&&!/^%/&&!/^\d+::\d+/ , split(/\s+/, $l));
    my @l = grep(defined&&length&&!/^\d+::\d+/ , split(/\s+/, $l));

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
		    $curr_lang = $lang;
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
