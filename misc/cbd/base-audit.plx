#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use Data::Dumper;
use lib "$ENV{'ORACC'}/lib";
use ORACC::CBD::Bases;

my $input = shift @ARGV;

my $curr_cfgw = '';
my %baseconts = ();
my %count = ();
my %count_w_cont = ();
my @fit_counts = ();
my @fit_zeroes = ();
my $fit_max = 0;
my %insts = ();

my $nwords = 0;
#my @bcount = `base-count.plx 01bld/from-prx-glo.sig`; chomp(@bcount);
open(B,'00etc/base-count.tab');
while (<B>) {
    chomp;
    my($k,$c,$i) = split(/\t/,$_);
    $count_w_cont{$k} = $c;
    my($ba,$co) = ($k =~ /^(.*?)\+(?:(o)|-(.*))$/);
    $count{$ba} += $c;
    push @{$insts{$ba}}, $i;
    push @{$baseconts{$ba}}, $co;
}
close(B);

open(L,'>00etc/base-audit.tab'); select L;

bases_init();
open(IN, $input) || die "$0: can't open $input for base audit\n";
print "=file: $input\n";
while (<IN>) {
    if (/^[-+>]?\@(entry|bases)/) {
	if (/^[-+>]?\@entry\!?\s+(.*?)\s*$/) {
	    $curr_cfgw = $1;
	} else {
	    s/^\@bases\s+//;
	    my %b = bases_hash($_);
	    audit(grep(!/\#/, keys %b)) if scalar keys %b;
	}
    }
}
bases_term();

summary();

close(L);

#############################################################################

sub audit {
    my @b = @_;
#    warn "audit @b\n";
    my $cf = $curr_cfgw;
    $cf =~ s/\s+\[.*$//;
    $cf = mangle($cf);
    return if $cf =~ /\s/;
    
    $curr_cfgw =~ s/\s+\[/[/; $curr_cfgw =~ s/\]\s+/]/;
    my $l = $.+1;
    print "=entry:$l: $curr_cfgw: $cf ~ @b\n";

    my @i = ();
    my $total = 0;
    foreach my $b (@b) {
	my $k = "$curr_cfgw/\%sux:$b";
	my %info = ();
	$info{'base'} = $b;
	if ($count{$k}) {
	    $info{'count'} = $count{$k};
	    @{$info{'insts'}} = @{$insts{$k}};
	} else {
	    $info{'count'} = 0;
	    @{$info{'insts'}} = ();
	}
	$total += $info{'count'};
	@info{qw/form fit/} = base_fit($cf,$b);
	++$fit_counts[$info{'fit'}];
	++$fit_zeroes[$info{'fit'}] unless $info{'count'};
	
	$fit_max = $info{'fit'} if $info{'fit'} > $fit_max;
	$info{'type'} = base_type($cf,$b);
	push @i, { %info };
    }
    foreach my $i (sort { $$b{'count'} <=> $$a{'count'} } @i) {
	print_info($cf, %$i);
    }

    ++$nwords;
}

sub summary {
    print "=summary\n";
    for (my $i = 0; $i <= $fit_max; ++$i) {
	if (defined $fit_counts[$i]) {
	    print "$fit_counts[$i] fits at level $i \[$fit_zeroes[$i] are unattested]\n";
	}
    }
}

sub mangle {
    my $b = shift;
    $b =~ s/ₓ\(\|.*?\|[^(]*\)//;
    $b =~ s/ₓ\(.*$?\)//;
    $b =~ tr/-.+&%@₀-₉ʾ|//d;
    $b =~ tr/ₓ//d;
    $b =~ s/\{.*?\}//;
    $b =~ s/^.*?°//;
    $b =~ s/·.*$//;
    1 while $b =~ s/([a-zA-ZŋŊšŠ])\1/$1/;
    $b =~ s/\(.*?\)$//;
    $b;
}

sub print_info {
    my($b, %i) = @_;
    print "$b";
    foreach my $k (qw/base count type form fit/) {
	print "\t$i{$k}";
    }
    if ($i{'fit'} > 1) {
	print "\t@{$i{'insts'}}";
    }
    print "\n";
}

sub base_fit {
    my($c,$b) = @_;
    my $fit = 1;

    $b = mangle($b);

    1 while $b =~ s/([a-z])\1/$1/;
    return ($b,$fit) if $b eq $c;
    ++$fit;

    if ($c =~ /k$/) {
	my $tmp = $c;
	$tmp =~ s/k$//;
	return ($b,$fit) if $b eq $tmp;
    }
    ++$fit;

    ($b,$fit);
}

sub base_type {
    'log';
}

1;
