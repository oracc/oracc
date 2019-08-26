#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use Data::Dumper;
use ORACC::L2GLO::Util;
use ORACC::Texts::Util;

my($w2l_file,$log_file) = @ARGV;
open(W,$w2l_file); my @w = (<W>); chomp @w; close(W);
open(L,$log_file); my @log = (<L>); chomp @log; close(L);

my %cgp = ();
my %err = ();
my %sc = ();
my %seen = ();
my @sig = ();

foreach my $l (@log) {
    my($file,$line,$cfgw,@err) = split(/:/, $l);
    $cfgw =~ s/^\s*(.*?)\s*$/$1/;
    $sc{$cfgw} = $line;
    if ($err[0] =~ /new base (\S+)/) {
	push @sig, "\%sux:$cfgw/$1";
	++${$cgp{$cfgw}}{$sig[$#sig]};
	$err{$sig[$#sig]} = $err[0];
    } elsif ($err[0] =~ /map base (\S+)/) {
	push @sig, "\%sux:$cfgw/$1";
	++${$cgp{$cfgw}}{$sig[$#sig]};
	$err{$sig[$#sig]} = $err[0];
    } else {
	warn "unhandled error $err[0]\n";
    }
}

open(S,'>base-instances.lst'); print S join("\n", @sig), "\n"; close(S);

my %w2l = wid2lem_by_sig($w2l_file,\@w,$log_file,\@sig);

foreach my $cgp (sort { $sc{$a} <=> $sc{$b} } keys %cgp) {
    foreach my $sig (keys %{$cgp{$cgp}}) {
	my $w2l = $w2l{$sig};
	foreach my $ok (@$w2l) {
	    my %p = parse_sig($$ok[4]);
	    unless ($seen{$cgp,$p{'form'}}++) {
		$p{'form'} =~ s/^\%.*?://;
		print "$$ok[0]:$$ok[1]: $cgp:\t$p{'form'}=$$ok[3] check$err{$sig}\n";
	    }
	}
    }
}


1;
