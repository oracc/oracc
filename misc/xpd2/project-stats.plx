#!/usr/bin/perl
use warnings; use strict;

my $project=`oraccopt`;
my @stats_xml = ();

textinfo();
wordinfo();

print '<project-stats>', @stats_xml, '</project-stats>';

1;

########################################################################

sub
textinfo {
    my $individual_texts = (-r '02pub/cat/pqids.lst' ? `grep -c ^$project:[PX] 02pub/cat/pqids.lst` : 0);
    my $composite_texts = (-r '02pub/cat/pqids.lst' ? `grep -c ^$project:Q 02pub/cat/pqids.lst` : 0);
    my $individual_atfs = (-r '01bld/lists/have-atf.lst' ? `grep -c ^$project:[PX] 01bld/lists/have-atf.lst` : 0);
    my $composite_atfs = (-r '01bld/lists/have-atf.lst' ? `grep -c ^$project:Q 01bld/lists/have-atf.lst` : 0);
    my $individual_lemm = (-r '01bld/lists/have-lem.lst' ? `grep -c ^$project:[PX] 01bld/lists/have-lem.lst` : 0);
    my $composite_lemm = (-r '01bld/lists/have-lem.lst' ? `grep -c ^$project:Q 01bld/lists/have-lem.lst` : 0);
    chomp($individual_texts, $composite_texts, $individual_atfs, $composite_atfs, $individual_lemm, $composite_lemm);
    push @stats_xml, "<individual_texts>$individual_texts</individual_texts>";
    push @stats_xml, "<composite_texts>$composite_texts</composite_texts>";
    push @stats_xml, "<individual_atfs>$individual_atfs</individual_atfs>";
    push @stats_xml, "<composite_atfs>$composite_atfs</composite_atfs>";
    push @stats_xml, "<individual_lemm>$individual_lemm</individual_lemm>";
    push @stats_xml, "<composite_lemm>$composite_lemm</composite_lemm>";
}

sub
wordinfo {
    my ($wordcount, $signatures, $lemmed_words) = (0, 0, 0);

    my @PQXdirs = (<01bld/[PQX]*>);
    if ($#PQXdirs >= 0 && -r '01bld/lists/have-xtf.lst') {
	$wordcount = `cut -d: -f2 01bld/lists/have-xtf.lst | $ENV{'ORACC'}/bin/expand $project xtf | xargs cat | xmlnl | grep -c '^><[gn]:w'`;
	chomp $wordcount;
    }
    
    my @sig = <02pub/lemm*.sig>;
    if ($#sig >= 0) {
	$signatures = `grep -v '	0\$' 02pub/lemm*.sig | wc -l`;
	chomp $signatures;
	$signatures =~ s/^\s*(\S+).*$/$1/;
	$lemmed_words = 0;
	open(C, "cut -f2 02pub/lemm*.sig|");
	while (<C>) {
	    chomp;
	    $lemmed_words += $_;
	}
	close(C);
    }

    push @stats_xml, "<wordcount>$wordcount</wordcount>";
    push @stats_xml, "<attested_signatures>$signatures</attested_signatures>";
    push @stats_xml, "<lemmed_words>$lemmed_words</lemmed_words>";
}

1;
