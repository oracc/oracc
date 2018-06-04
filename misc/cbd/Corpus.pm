package ORACC::CBD::Corpus;
require Exporter;
@ISA=qw/Exporter/;
@EXPORT = qw/file_words/;

use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

use ORACC::CBD::PPWarn;

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
    @{$f{'lines'}} = @f;
    @{$f{'lemma'}} = lem_file(@f);
    use Data::Dumper;
    print Dumper \%f;
}

sub lem_file {
    my @f = @_;
    my $curr_lang = '';
    my $text_lang = '';
    my $curr_line = '';
    my %sparse = ();
    my $use_sparse = 0;
    for (my $i = 0; $i <= $#f; ++$i) {
	if ($f[$i] =~ /^\#lem/) {
	    $f[$i] = lem_hashes($curr_line,$f[$i],$text_lang,($use_sparse?\%sparse:undef),$f,$i+1);
	    $curr_line = '';
	} else {
	    if (/^\#lemmatizer:\s+sparse\s+do\s+(.*?)\s*$/) {
		my $sparse = $1;
		@sparse{@split(/\s+/, $sparse)} = ();
		$use_sparse = 1;
	    } elsif (/^\#atf:\s+lang\s+(\S+)\s*$/) {
		$curr_lang = $text_lang = $1;
	    } elsif (!/^/ && /^[\s\$\#\@]\S*?\.(\s|$)/) {
		$curr_line = $f[$i];
		$curr_lang = $text_lang;
	    } elsif (/^\&/) {
		$curr_lang = 'sux';
		$curr_line = '';
		$use_sparse = 0;
	    }
	    $f[$i] = "\000";
	}
    }
    @f;
}

sub lem_hashes {
    my($wds,$lem,$lang,$sparseref,$file,$line) = @_;
    my @wds = split_wds($wds,$lang,$file,$line);
    my @lem = split_lem($lem,$sparseref,@wds);
    if ($#lem < 0) { #return empty list on failure in split_lem
	return "\000";
    } else {
	return [ @lem ];
    }
}

sub split_wds {
    my ($l,$lang,$file,$line) = @_;
    chomp $l;
    $l =~ s/\(\#.*?\#\)//g;
    $l =~ s/\(\$.*?\$\)//g;
    $l =~ s/\{\{/ /g;
    $l =~ s/\}\}/ /g;
    $l =~ s/^\S*\s+//;
    $l =~ s/\s$//;
    $l =~ s/<<.*?>>//g;
    $l =~ s/--/-/;
    $l =~ s/\!([a-z][a-z])\s/%$1/g; # hack !sn to %sn
    $l =~ tr/-:. a-zA-Z0-9šŋŠŊ₀-₉ₓ\|\@&~%{}()//cd;
    $l =~ s/\s+/ /g;
    $l =~ s/\(::\)//g; # for etcsl
    #  my @line = grep(defined&&length&&!/^%/&&!/^\d+::\d+/ , split(/\s+/, $l));
    my @l = grep(defined&&length&&!/^\d+::\d+/ , split(/\s+/, $l));

    # now factor out langs and lex field codes and build list of word hashes
    my $curr_field = '';
    my $curr_lang = $lang;
    for my ($i = 0; $i <= $#line; ++$i) {
	if ($l[$i] =~ /^\%(.*?)$/) {
	    my $tag = $1;
	    if ($lextags{$tag}) {
		$curr_field = $tag;
	    } else {
		if ($tag =~ s/-.*$// || length($tag) == 3) {
		    $curr_lang = $lang;
		} else {
		    if ($langtags{$tag}) {
		    } else {
			warn "$file:$line: unknown language tag '$tag'\n";
		    }
		}
	    }
	} else {
	    push @wds, { w=>$l[$i], lang=>$curr_lang, field=>$curr_tag };
	}
    }
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
    for (my $i = 0; $i <= $#wds; ++$i) {
	my %w = %{$wds[$i]};
	# if field is null then default is word and is always lemmatizable
	if (!$sparseref || !$w{'field'} || exists($$sparseref{$w{'field'}})) {
	    if ($lem[$j]) {
		${$wds[$i]}{'lem'} = $lem[$j++];
	    } else {
		# we have more words than lemmata
		warn "$file:$line: too few lemmata\n";
		return ();
	    }
	}
	$wds[$i] = { %w };
    }
    if ($j < $#lem) {
	# we had more lemmata than words
	warn "$file:$line: too many lemmata\n";
	return ();
    }
    return @wds;
}

1;
