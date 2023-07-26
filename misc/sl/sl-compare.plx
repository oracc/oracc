#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;

GetOptions(
    );

my %ireject = ();
if (-r 'itrace.log') {
    my @r = `grep rejecting itrace.log`; chomp(@r);
    foreach my $r (@r) {
	my $q = '';
	if ($r =~ /::(\S+)/) {
	    $q = $1;
	} else {
	    $r =~ /sign\s+(\S+)\s+/;
	    $q = $1;
	}
	my $v = '';
	$r =~ /of\s+(\S+)\s+because/;
	$v = $1;
	++$ireject{"$v($q);qv"} if $v && $q;
    }
    # warn Dumper \%ireject;
}

my $ignorec1 = 0;
my $ignorec2 = 0;

my %ignore = (); my @ignore = qw/lists parents uchar/; @ignore{@ignore} = ();

my %ignored_values1 = ();
my @ignored_values1 = qw/gitag peš₉ būdu namru ilī su₉ endib tertu/; @ignored_values1{@ignored_values1} = ();
$ignored_values1{'1/4'} = '';
$ignored_values1{'1/6'} = '';
$ignored_values1{'tana tibnu'} = '';

my %ignored_values2 = ();
my @ignored_values2 = qw/xₓ x₀₁ x₀₂ x₀₃ x₀₄ x₀₅ x₀₆ x₀₇ x₀₈ x₀₉ x₁₀ x₁₁ x₁₂ x₁₃ x₁₄ x₁₅ x₁₆ x₁₇ x₁₈ x₁₉ x₂₀ x₂₁ x₂₂ x₂₃/;
@ignored_values2{@ignored_values2} = ();
@ignored_values2{'ašgi'} = ''; # this is a duplicate in ogsl2.asl

my $sl1 = shift @ARGV;
my $sl2 = shift @ARGV;

my %fields1 = ();
my %fields2 = ();

my $nsl1 = 0;
my $nsl2 = 0;

warn "comparing $sl1 and $sl2\n";

my %sl1 = ();

open(SL1, $sl1) || die "no $sl1\n";

while (<SL1>) {
    ++$nsl1;
    chomp;
    if (/;(\S+)/) {
	my $c = $1;
	++$ignorec1 and next if exists $ignore{$c};
	++$fields1{$c};
	my($k,$v) = split(/\t/, $_);
	${$sl1{$c}}{$k} = $v;
    } else {
	my($k,$v) = split(/\t/, $_);
	unless ($k =~ /^o\d+$/) {
	    ${$sl1{'none'}}{$k} = $v;
	    ++$fields1{'none'};
	} else {
	    ++$ignorec1;
	}
    }
}

my %sl2 = ();

open(SL2, $sl2) || die "no $sl2\n";
while (<SL2>) {
    ++$nsl2;
    chomp;
    if (/;(\S+)/) {
	my $c = $1;
	++$ignorec2 and next if exists $ignore{$c};
	++$fields2{$c};
	my($k,$v) = split(/\t/, $_);
	${$sl2{$c}}{$k} = $v;
    } else {
	++$fields2{'none'};
	my($k,$v) = split(/\t/, $_);
	${$sl2{'none'}}{$k} = $v;
    }
}

warn "$sl1: $nsl1 ; $sl2: $nsl2; ignore1=$ignorec1; ignore2=$ignorec2\n";

foreach my $f (sort keys %fields1) {
    print "$sl1;$f $fields1{$f} ; $sl2;$f $fields2{$f}\n";
}

foreach my $f (sort keys %fields2) {
    unless (exists $fields1{$f}) {
	print "$sl1;$f 0 ; $sl2$f $fields2{$f}\n";
    }
}

# print Dumper \%sl1;

my %v1 = %{$sl1{'values'}};
my %v2 = %{$sl2{'values'}};

foreach my $v1 (sort keys %v1) {
    if (!exists $v2{$v1}) {
	warn "key $v1 is not in $sl2\n" unless exists $ignored_values1{$v1{$v1}};
    }
}

foreach my $v2 (sort keys %v2) {
    if (!exists $v1{$v2}) {
	warn "key $v2 is not in $sl1\n" unless exists $ignored_values2{$v2{$v2}};
    }
}

my %q1 = %{$sl1{'qv'}};
my %q2 = %{$sl2{'qv'}};

foreach my $q1 (sort keys %q1) {
    if (!exists $q2{$q1}) {
	if ($ireject{$q1}) {
	    # print "key $q1 is an ireject; suppressing error\n";
	} else {
	    warn "key $q1 is not in $sl2\n"; # unless exists $ignored_values1{$v1{$v1}};
	}
    }
}

foreach my $q2 (sort keys %q2) {
    if (!exists $q1{$q2}) {
	warn "key $q2 is not in $sl1\n"; # unless exists $ignored_values2{$v2{$v2}};
    }
}

my %c1 = %{$sl1{'contained'}};
my %c2 = %{$sl2{'contained'}};

foreach my $c1 (sort keys %c1) {
    if ($c1 =~ /\@[CTGZFN]/) {
	my($l,$r) = ($c1 =~ /^(.*?)\@(.\)?);contained$/);
	if ($l && $r) {
	    $c1 = "$l\@\L$r;contained";
	    $c1 =~ s/\@G\@/\@g\@/;
	} else {
	    warn "contained lc failed\n";
	}
    }
    if (!exists $c2{$c1}) {
	warn "key $c1 is not in $sl2\n"
	    unless $c1 =~ /\./ || $c1 =~ /^\d/;  # dotted compounds should not be in 'contained'; numbers different but OK
    }
}

foreach my $c2 (sort keys %c2) {
    if (!exists $c1{$c2}) {
	warn "key $c2 is not in $sl1\n"
	    unless $c2 =~ /\@/	# modified signs should have been included in sl.tsv but aren't because a bug
	    || $c2 =~ /N\d/; # numbers are handled differently but satisfactorily
	    
    }
}

my %n1 = %{$sl1{'none'}};
my %n2 = %{$sl2{'none'}};

foreach my $n1 (sort keys %n1) {
    if (!exists $n2{$n1}) {
	warn "key $n1 is not in $sl2\n"; # unless exists $ignored_values1{$v1{$v1}};
    }
}

foreach my $n2 (sort keys %n2) {
    if (!exists $n1{$n2}) {
	warn "key $n2 is not in $sl1\n"; # unless exists $ignored_values2{$v2{$v2}};
    }
}

1;
