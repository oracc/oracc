#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";
use Getopt::Long;

my $atffile = '';
my $pqxid = '';
my $proj = '';

GetOptions(
    'atffile:s'=>\$atffile,
    'id:s'=>\$pqxid,
    'proj:s'=>\$proj,
    );

die "$0: must give -atf ATFFILE -id PQXID -proj PROJECT. Stop.\n"
    unless $atffile && $pqxid && $proj;

my $pqxdir = $pqxid; $pqxdir =~ s/...$//;
my $xtffile = "$ENV{'ORACC_BUILDS'}/$proj/01bld/$pqxdir/$pqxid/$pqxid.xtf";
my @linksets = `echo $proj:$pqxid | $ENV{'ORACC_BUILDS'}/bin/linksets`;

my %lem2wrd = ();
my %wrd2lin = ();
my %linksets = ();

foreach (@linksets) {
    next if /^#/;
    chomp;
    my @f = split(/\t/,$_);
    if ($#f == 1) { # map lem id to word id
	$lem2wrd{$f[0]} = $f[1];
    } elsif ($#f == 2) { # file:line for word id
	my $q = quotemeta($atffile);
	if ($f[1] =~ /$q$/o) {
	    $wrd2lin{$f[0]} = $f[2];
	} else {
	    warn "$0: atffile $atffile doesn't match $f[1]\n";
	}
    } elsif ($#f == 3) {
	my $lem = $f[3]; $lem =~ s/\s.*$//; $lem =~ s/^\#//;
	if ($lem2wrd{$lem}) {
	    my $wrd = $lem2wrd{$lem};
	    if ($wrd2lin{$wrd}) {
		push @{$linksets{$wrd2lin{$wrd}}}, [ $f[0] , $f[1] ];
	    } else {
		warn "$0: word $wrd has no line info\n";
	    }
	} else {
	    warn "$0: lem $lem has no word mapping\n";
	}
    } else {
	warn "$0: unknown number of fields in linksets output\n";
    }
}

# print Dumper \%linksets;

open(A,$atffile) || die "$0: unable to open atffile $atffile\n";
my $lnum = 0;
while (<A>) {
    ++$lnum;
    if ($linksets{$lnum}) {
	my $ls_lnum = $lnum;
	until (/^\#lem:/) {
	    print;
	    $_ = <A>;
	    ++$lnum;
	    last unless $_;
	}
	if (/^\#lem/) {
	    print;
	    print "\n";
	    foreach my $ls (@{$linksets{$ls_lnum}}) {
		my @lsinfo = @$ls;
		print "###ls.$lsinfo[0] $lsinfo[1]\n";
	    }
	    print "\n";
	} else {
	    warn "$0: strange--expected to find #lem: line at or after line $ls_lnum\n";
	}
    } else {
	print;
    }
}

1;
