#!/usr/bin/perl
use warnings; use strict;
use lib "$ENV{'ORACC'}/lib";
use Data::Dumper;

foreach (`lex-provides-tab.plx`) {
    chomp;
    my($o,$x) = split(/\t/,$_);
    my $f = "01bld/www/cbd/sux/$o.html";
    if (-r $f) {
	my $h = `cat $f`;
	my ($a,$b) = ($h =~ /^(.*?)__LEXDATA__(.*?)$/s);
	open(N,">new/$o.html"); select N;
	print $a;
	xincludes($o,$x);
	print $b;
	close(N);
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
