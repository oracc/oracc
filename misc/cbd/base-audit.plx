#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use Data::Dumper;
use lib "$ENV{'ORACC'}/lib";
use ORACC::CBD::Bases;

my $curr_cfgw = '';
my %count = ();
my $nwords = 0;
my @bcount = `base-count.plx 01bld/from-prx-glo.sig`; chomp(@bcount);
foreach my $b (@bcount) {
    my($k,$c) = split(/\t/,$b);
    $count{$k} = $c;
}

open(L,'>audit.log'); select L;

bases_init();
while (<>) {
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

close(L);

sub audit {
    my @b = @_;
    warn "audit @b\n";
    my $cf = $curr_cfgw;
    $cf =~ s/\s+\[.*$//;
    return if $cf =~ /\s/;
    
    $curr_cfgw =~ s/\s+\[/[/; $curr_cfgw =~ s/\]\s+/]/;
    print "=start: $curr_cfgw: $cf ~ @b\n";

    my @i = ();
    my $total = 0;
    foreach my $b (@b) {
	my $k = "$curr_cfgw/\%sux:$b";
	my %info = ();
	$info{'base'} = $b;
	$info{'count'} = $count{$k} || 0;
	$total += $info{'count'};
	@info{qw/form fit/} = base_fit($cf,$b);
	$info{'type'} = base_type($cf,$b);
	push @i, { %info };
    }
    foreach my $i (sort { $$b{'count'} <=> $$a{'count'} } @i) {
	print_info($cf, %$i);
    }

    ++$nwords;
}
# print "=summary\n";

sub print_info {
    my($b, %i) = @_;
    print "$b";
    foreach my $k (qw/base count type form fit/) {
	print "\t$i{$k}";
    }
    print "\n";
}

sub base_fit {
    my($c,$b) = @_;
    my $fit = 1;

    $b =~ tr/-.₀-₉//d;
    $b =~ s/ₓ\(\|.*$?\|\)//;
    $b =~ s/ₓ\(.*$?\)//;
    $b =~ s/\{.*?\}//;
    1 while $b =~ s/([a-z])\1/$1/;
    return ($b,$fit) if $b eq $c;

    ++$fit;
    ($b,$fit);
}

sub base_type {
    'log';
}

1;
