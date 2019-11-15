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

#my $w2l_file = `cbdstash.plx locdata fixed`;
my $w2l_file = '01bld/location.data';
system 'makemake.plx' unless -r '00lib/Makefile';
system 'make', '-f', '00lib/Makefile', '01bld/xtfstamp', '01bld/updatestamp', $w2l_file;
my @w = ();
if ($w2l_file) {
    if (-r $w2l_file) {
	# @w = `unxz -c $w2l_file`; chomp @w;
	@w = `cat $w2l_file`; chomp @w;
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
    my($file,$line,$cfgw,@err) = split(/:/, $l);
    $cfgw =~ s/^\s*(.*?)\s*$/$1/;
    $sc{$cfgw} = $line;
    if ($err[0] =~ /new base (\S+)/) {
	#	push @sig, "\%sux:$cfgw/$1";
	push @sig, "$cfgw/$1";
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

#open(S,'>base-instances.lst'); print S join("\n", @sig), "\n"; close(S);

my %w2l = wid2lem_by_sig($w2l_file,\@w,$log_file,\@sig);

#open(W,'>base-instances.w2l'); print W Dumper \%w2l; close(W);
#open(W,'>base-instances.cgp'); print W Dumper \%cgp; close(W);

foreach my $cgp (sort { $sc{$a} <=> $sc{$b} } keys %cgp) {
    foreach my $sig (keys %{$cgp{$cgp}}) {
	my $w2l = $w2l{$sig};
	foreach my $ok (@$w2l) {
	    my %p = parse_sig($$ok[4]);
#	    unless ($seen{$cgp,$p{'form'}}++) {
#		warn "dumping $cgp with form $p{'form'}\n";
		$p{'form'} =~ s/^\%.*?://;
		if ($err{$sig} =~ /new base (\S+)/) {
		    my $b = $1;
		    print "$$ok[0]:$$ok[1]: $cgp:\t$b < $p{'form'} is new\n"; # =$ok[3]
		} else {
		    # the map entries don't generally need checking against the corpus
#		    print "$$ok[0]:$$ok[1]: $cgp:\t$p{'form'} check$err{$sig}\n"; # =$ok[3]
		}
#	    }
	}
    }
}


1;
