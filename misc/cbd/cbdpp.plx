#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

use Getopt::Long;

my $check = 0;
my $ngm = 0;

GetOptions(
    'check'=>\$check,
    'ngm'=>\$ngm,
    );

my %rws_map = (
    EG => 'sux',
    ES => 'sux-x-emesal',
    CF => 'akk',
    CA => 'akk-x-conakk',
    OA => 'akk-x-oldass',
    OB => 'akk-x-oldbab',
    MA => 'akk-x-midass',
    MB => 'akk-x-midbab',
    NA => 'akk-x-neoass',
    NB => 'akk-x-neobab',
    SB => 'akk-x-stdbab',
    );

# C1MRW1X4DV30  100370013896 Will 877-388-0879 #38102 Sun-Thu 8:30-5:30pm EST

my @funcs = qw/free impf perf Pl PlObj PlSubj Sg SgObj SgSubj/;
my %funcs = (); @funcs{@funcs} = ();

my @poss = qw/AJ AV N V DP IP PP CNJ J MA O QP RP DET PRP POS PRT PSP
    SBJ NP M MOD REL XP NU AN BN CN DN EN FN GN HN IN JN KN LN MN NN
    ON PN QN PNF RN SN TN U UN VN WN X XN YN ZN/; 

push @poss, ('V/t', 'V/i'); 
my %poss = (); @poss{@poss} = ();

my @stems = qw/B rr RR rR Rr rrr RRR rrrr RRRR S₁ S₂ S₃ S₄/;
my %stems = (); @stems{@stems} = ();

my @tags = qw/entry parts bff bases stems phon root form length norms
              sense equiv inote prop end isslp bib defn note pl_coord
              pl_id pl_uid ngm/;

my %tags = (); @tags{@tags} = ();

my $cbd = shift @ARGV;
my $lng = $cbd; $lng =~ s/\.glo$//; $lng =~ s#.*?/([^/]+)$#$1#;

my @acd = ();
my @cbd = ();
my @ngm = ();

cbd_load();

dump_ngm() if $#ngm >= 0;
dump_acd() if $#acd >= 0; 
dump_cbd();

#######################################################################

sub cbd_load {
    open(C,$cbd) || die "cbdpp.plx: unable to open $cbd. Stop.\n";
    while (<C>) {
	chomp;
	if (/^\@ngm/) {
	    push @ngm, $_;
	    next;
	}
	if (/^\@bases/) {
	    my ($orig,$clean) = clean_bases($_);
	    if ($clean ne $orig) {
		push @acd, $clean;
	    }
	    push @cbd, $_;
	    next;
	}
	if (/^>.*?\@form/) {
	    push @acd, $_;
	    s/^.*?\@/\@/;
	    push @cbd, $_;
	    next;
	}
	if (/^[->+=]/) {
	    push @acd, $_;
	} else {
	    push @cbd, $_;
	}
    }
    close(C);
}

sub clean_bases {
    my $tmp = shift;
    my @b = split(/;\s+/, $tmp);
    my $rb = join('; ', @b);
    my @nb = ();
    foreach my $b (@b) {
	$b =~ s/^[-=+]//;
	$b =~ s/>\S+//;
    }
    ($rb, join('; ', @nb));
}

sub dump_acd {
    
}

sub dump_cbd {
    open(C, '>01bld/$lng.glo');
    foreach (@cbd) {
	print C $_, "\n";
    }
    close(C);
}

sub dump_ngm {
    open(N, '>01bld/$lng.ngm');
    foreach (@ngm) {
	print N $_, "\n";
    }
    close(N);
}

1;
