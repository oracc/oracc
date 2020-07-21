#!/usr/bin/perl
use warnings; use strict;

# say oiduniq.plx -o to uniq by oid and not by full line

my $oidonly = $ARGV[0];
if ($oidonly && $oidonly eq '-o') {
    shift @ARGV;
}
    
my $file = shift @ARGV;
my @keep = ();
my %seen = ();

open(X,$file);
if ($oidonly) {
    while (<X>) {
	my $l = $_;
	$l =~ /^(\S+)\s/;
	push (@keep, $l) unless $seen{$1}++
    }
} else {
    while (<X>) {
	push (@keep, $_) unless $seen{$_}++
    }
}
close(X);

open(X,"| sort >$file");
print X @keep;
close(X);

1;
