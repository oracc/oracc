#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
use Getopt::Long;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';

my %header = ();
my $quiet = 0;
my %sig = ();

GetOptions(
    'quiet'=>\$quiet,
    );

foreach my $s (@ARGV) {
    unless (open(S,$s)) {
	warn "l2-sig-union.plx: no sig file $s\n"
	    unless $quiet;
	next;
    }
    while (<S>) {
	chomp;
	next if /^\s*$/;
	if (/^\@(project|name|lang)\s+(.*?)$/) {
	    $header{$1} = $2;
	    next;
	}
	my @t = split(/\t/, $_);
	my $r = (($#t == 2) ? $t[2] : (($#t == 1) ? $t[1] : ''));
	unless ($sig{$_}) {
	    my @r = split(/\s/, $r);
	    if ($#r >= 0) {
		@{$sig{$t[0]}}{@r} = ();
	    }
	}
    }
    close(S);
}
foreach my $k (qw/project name lang/) {
    print "\@$k $header{$k}\n";
}
print "\n";

#use Data::Dumper; open(X,">dump.log");
#print X Dumper \%sig;
#close(X);

foreach my $s (sort keys %sig) {
    print "$s\t";
    my @r = sort keys %{$sig{$s}};
    if ($#r >= 0) {
	printf "%d\t@r", $#r+1;
    } else {
	print "\t0";
    }
    print "\n";
}

1;
