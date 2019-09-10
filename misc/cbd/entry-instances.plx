#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use Data::Dumper;
use ORACC::L2GLO::Util;
use ORACC::Texts::Util;
use Data::Dumper;

my $log_file = shift @ARGV;
my @log = ();
if ($log_file) {
    open(L,$log_file) || die "$0: can't open log $log_file for read\n";
    @log = (<L>);
    chomp @log; close(L);
}

my $w2l_file = `cbdstash.plx locdata init`;
my @w = ();
if ($w2l_file) {
    if (-r $w2l_file) {
	@w = `unxz -c $w2l_file`; chomp @w;
    } else {
	die "$0: can't open wid2loc data $w2l_file for read\n";
    }
}

my %cgp = ();
my %err = ();
my %sc = ();
my %seen = ();
my @sig = ();

foreach my $l (@log) {
    next unless $l =~ /not in base/;
    my($file,$line,@err) = split(/:/, $l);
    my ($cfgw) = ($err[0] =~ /entry\s+(.*?)\s+not in base/);
    $cfgw =~ s/\s+\[(.*?)\]\s+/[$1]/;
    $sc{$cfgw} = $line;
    push @sig, $cfgw;
    ++${$cgp{$cfgw}}{$sig[$#sig]};
    $err{$sig[$#sig]} = $err[0];
}

#open(S,'>base-instances.lst'); print S join("\n", @sig), "\n"; close(S);

my %w2l = wid2lem_by_sig($w2l_file,\@w,$log_file,\@sig);

open(W,'>entry-instances.w2l'); print W Dumper \%w2l; close(W);
open(W,'>entry-instances.cgp'); print W Dumper \%cgp; close(W);

foreach my $cgp (sort { $sc{$a} <=> $sc{$b} } keys %cgp) {
    foreach my $sig (keys %{$cgp{$cgp}}) {
	my $w2l = $w2l{$sig};
	foreach my $ok (@$w2l) {
	    print "$$ok[0]:$$ok[1]: $cgp is new\n";
	}
    }
}


1;
