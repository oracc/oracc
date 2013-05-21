#!/usr/bin/perl
use warnings; use strict; use open ':utf8';
binmode STDIN,':utf8'; binmode STDOUT, ':utf8';
my %bad = ();

my @badlem = @ARGV; @ARGV = ();

foreach my $badlem (@badlem) {
    open(B,$badlem) || die;
    while (<B>) {
	next if /^\#/;
	die unless /^(.*?\[.*?\]\S*)\t(.+?(?:\[.*?\]\S*)?)$/;
	$bad{$1} = $2;
    }
    close(B);
}

while (<>) {
    print and next unless s/^\#lem:\s*//;
    chomp;
    tr/\r\n//d;
    my @newlem = ();
    foreach my $lem (split(/\;\s*/,$_)) {
	my @lem = ();
	foreach my $l (split(/(?<!\\)\|/,$lem)) {
	    push(@lem,$l) and next unless $l =~ /\[/;
	    $lem =~ s/\s*$//;
	    warn("$ARGV:$.: bad cfgw $l\n") and next
		unless $l =~ 
/^.*?\[.*?\][A-Z]*(?:\'[A-Z]*)?(?:[\\\/][-a-z0-9\.\*]+)?(?:\+0|\+\*[0a-zE\.\*]+)?(?:\s+[\+-]\.\s*)?(?:\#.*?)?$/;
	    push @lem, $bad{$l} || $l;
	}
	push @newlem, join('|',@lem);
    }
    print "#lem: ", join('; ', @newlem), "\n";
}

1;
