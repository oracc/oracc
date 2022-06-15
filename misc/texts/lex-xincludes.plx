#!/usr/bin/perl
use warnings; use strict;
use lib "$ENV{'ORACC'}/lib";
use Data::Dumper;

my $verbose = 1;
my $webdir = $ARGV[0];
my $lang = $ARGV[1];

$webdir = '01bld/www' unless $webdir;
$lang = 'sux' unless $lang;

foreach (`lex-provides-tab.plx`) {
    chomp;
    my($o,$x) = split(/\t/,$_);
    my $f = "$webdir/cbd/$lang/$o.html";
    if (-r $f) {
	warn "$0: processing $f\n" if $verbose;
	my $h = `cat $f`;
	my ($a,$b) = ($h =~ /^(.*?)__LEXDATA__(.*?)$/s);
	open(N,">$f"); select N;
	print $a;
	xincludes($o,$x);
	print $b;
	close(N);
    } else {
	warn "$0: no such file $f\n" if $verbose;
    }
}

sub xincludes {
    my ($o,$x) = @_;
    if ($x =~ /b/) {
	print "<xi:include href=\"\@\@OB\@\@/www/dcclt/cbd/sux/pb/$o.html\"/>"
    }
    if ($x =~ /e/) {
 	print "<xi:include href=\"\@\@OB\@\@/www/dcclt/cbd/sux/eq/$o.html\"/>"
    }
    if ($x =~ /p/) {
	print "<xi:include href=\"\@\@OB\@\@/www/dcclt/cbd/sux/wp/$o.html\"/>"
    }
}

1;
