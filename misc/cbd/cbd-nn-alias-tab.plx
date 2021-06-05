#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;
my %aliases = ();
my %seen = ();
my $sigs = '';
my %sigforms = ();
my $xalias = 0;
my $q = '';
GetOptions(
    q=>\$q,
    'sigs:s'=>\$sigs,
    'x'=>\$xalias,
    );

$q = shift @ARGV unless $q;
open(Q,$q) || die "$0: must give valid glossary on command line\n";
if ($sigs) {
    open(S,$sigs) || die "$0: can't read $sigs\n";
    while (<S>) {
	chomp;
	s/^.*?://;
	s/'.*$//;
	s,//.*?\],\],;
	my($f,$cgp) = (/^(.*?)=(.*?)$/);
	++${$sigforms{$cgp}}{$f};
    }
    close(S);
}
#print Dumper \%sigforms; exit 1;
my $e = '';
my @a = ();
my @f = ();
system 'mkdir', '-p', '00etc';
open(A,'>00etc/nn-alias.tab') || die;
open(G,'>00etc/nn-global.tab') || die;
open(S,'>00lib/NN-simple.sig') || die;
while (<Q>) {
    if (/\@entry/) {
	($e) = (/\s+(.*?)\s*$/);
	$e =~ s/\s+(\[.*?\])\s+/$1/;
	@a = ();
	push @a, 'X' if $xalias;
    } elsif (/\@alias/) {
	my ($a) = (/\s+(.*?)\s*$/);
	$a =~ s/\s+(\[.*?\])\s+/$1/;
	push @a, $a;
	if ($aliases{$a}) {
	    warn "$0:$.: duplicate alias $a points to $aliases{$a} and now also $e\n";
	} else {
	    $aliases{$a} = $e;	    
	}
	print A "$a\t$e\n";
    } elsif (/\@form/) {
	my ($f) = (/\s(\S+)\s/);
	foreach my $a (@a) {
	    print G "$f\t$a\t$e\n" unless $seen{$f,$a}++;
	}
    } elsif (/\@end\s+entry/) {
	if ($#a >= 0) {
	    @a = uniq(@a);
	    print S "$e\n";
	    foreach my $a (@a) {
		$a = augment($a) unless $a =~ m#//#;
		print S "\t$a\n";
	    }
	}
    }
}
close(A);
close(S);
close(Q);

foreach my $s (keys %sigforms) {
    my $a = '';
    if (($a = $aliases{$s})) {
	my @f = keys %{$sigforms{$s}};
	foreach my $f (@f) {
	    print G "$f\t$s\t$a\n" unless $seen{$f,$s}++;
	}
    } else {
	warn "$0: no \@entry found for expected alias $s\n";
    }
}

close(G);

sub augment {
    my $tmp = shift;
    $tmp =~ s/[ \t]+$//;
    $tmp =~ s/\s*(\[.*?\])\s*/$1/;
    $tmp =~ s#\[(.*?)\]#[$1//$1]#;
    $tmp =~ s/\](.*?)$/]$1'$1/;
    $tmp;
}

sub uniq {
    my %u = ();
    @u{@_} = ();
    sort keys %u;
}

1;
