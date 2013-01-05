#!/usr/bin/perl
use warnings; use strict;
use ORACC::ATF::Tokenizer;
use ORACC::ATF::Serializer;

my @words;

while (<>) {
    next if /^\s*$/;
    if (/^[=\@#|<>]/) {
	if (/^\#lem:/) {
	    s/^\#lem:\s+//;
	    my @lem = split(/;\s+/,$_);
	    if ($#lem != $#words) {
		warn("$.: $#lem != $#words\n");
	    } else {
		for (my $i = 0; $i <= $#words; ++$i) {
		    my $l = clean_lem($lem[$i]);
		    print "$words[$i]\t$l\n";
		}
	    }
	    @words = ();
	}
    } else {
	s/^\S+\s+//;
	my $words = toWords(tokenize_inline_fast($_));
	$words =~ s/%s%eg_=\s+//;
	$words =~ s/%a%eg/%a/g;
	$words =~ s/%a_/%a%syll_/g;
	@words = split(/\s+/, $words);
    }
}    

sub
clean_lem {
    my $tmp = shift;
    $tmp =~ s/\s+(?:[+-]\.)?\s*$//;
    $tmp;
}

1;
