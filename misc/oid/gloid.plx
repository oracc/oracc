#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

use lib "$ENV{'ORACC_BUILDS'}/lib";
use ORACC::OID;

# gloid.plx : create a version of a .glo file which is a table of @entry/@sense and oid in first col

my $glo = shift @ARGV;
die "$0: must give glossary name on command line\n" unless $glo;
die "$0: glossary $glo not readable\n" unless $glo;
my @skel = `cbdskel.sh $glo`; chomp @skel;

my $e = '';
my $oid = '';

oid_init();

foreach my $skel (@skel) {
    if ($skel =~ /\@(entry|sense)/) {
	my($t,$v) = split(/\t/,$skel);
	if ($t =~ /\@entry/) {
	    $e = $v;
	    $e =~ s/\s+\[(.*?)\]\s+/[$1]/;
	    $oid = oid_lookup('sux',$e);
	    if ($oid) {
		print "$oid\t$t\t$e\n";
	    } else {
		warn "$0: no OID for entry $e\n";
	    }
	} else {
	    my $s = $e;
	    my ($epos,$mean) = ($v =~ /^(\S+)\s+(.*?)$/);
	    $s =~ s#\](\S+)#//$mean]$1'$epos#;
	    $oid = oid_lookup('sux',$s);
	    if ($oid) {
		print "$oid\t$t\t$s\n";
	    } else {
		warn "$0: no OID for sense $s\n";		
	    }
	}
    } else {
	print "$skel\n";
    }
}

1;
