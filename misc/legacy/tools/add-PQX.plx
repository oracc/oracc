#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
use Getopt::Long;

binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';

my $all = 0;
my $input = undef;
my $output = 'PQX.out';
my $X = -1;

GetOptions(
    'all'=>\$all,
    'input:s'=>\$input,
    'output:s'=>\$output,
    'X:i'=>\$X,
    );

if (!-e $input || !-r _) {
    die "add-PQX.plx: $input non-existent or unreadable\n";
}

open(I, $input)  || die "add-PQX.plx: failed to open input file $input\n"; 
open(O,">$output") || die "add-PQX.plx: failed to open output file $output\n"; 
select O;
while (<I>) {
    unless (m/^\&/) {
	print;
	next;
    }
    if (/^(\&[PQX]\d+)?\t/) {
	if ($all) {
	    s/^.*?\s*=/=/;
	} else {
	    print;
	    next;
	}
    }
    chomp;
    my $query = $_;
    if ($query =~ /^\&\S*\s*=\s*(.*?)(?:\s*=|$)/) {
	$query = $1;
    } else {
	warn "$input:$.: expected `\& = [TEXT REFERENCE]' or similar\n";
    }
    $query =~ tr/-,.:;()//d;
    $query =~ s/\t.*//;
    $query =~ s/\s*[=\+].*$//;
    $query =~ s/\s+/_/g;
    warn "$query\n";
    my @res = `se \#cdli \!cat $query`;
    if ($#res >= 0) {
	chomp @res;
	s/^\&\S*\s*=\s*//;
	print "\&@res = $_\n";
    } else {
	if ($X >= 0) {
	    printf "\&X%06d\t$_\n",$X++;
	} else {
	    print "$_\n";
	}
    }
}
close(I);
close(O);

print STDERR "add-PQX.plx: output written to $output\n";

1;
