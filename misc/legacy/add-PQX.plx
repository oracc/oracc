#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
use Getopt::Long;

# Replace P or X numbers in legacy atf files, optionally using a table

binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';

my $all = 0;
my $dumped = 0;
my $input = undef;
my $output = 'PQX.out';
my $P = '';
my $tab = '';
my %tab = ();
my $thisX = '';
my $X = -1;
my $verbose = 0;

GetOptions(
    'all'=>\$all,
    'input:s'=>\$input,
    'output:s'=>\$output,
    'tab:s'=>\$tab,
    'X:i'=>\$X,
    'v'=>\$verbose,
    );

if (!-e $input || !-r _) {
    die "add-PQX.plx: $input non-existent or unreadable\n";
}

# P-number in col1, label in col2
if ($tab) {
    open(T,$tab) || die "add-PQX.plx: tabfile $tab non-existent or unreadable\n";
    while (<T>) {
	chomp;
	my($P,$l) = split(/\t/,$_);
	$tab{$l} = $P;
    }
    close(T);
}

open(I, $input)  || die "add-PQX.plx: failed to open input file $input\n"; 
open(O,">$output") || die "add-PQX.plx: failed to open output file $output\n"; 
select O;
while (<I>) {
    unless (m/^\&/) {
	print;
	next;
    }
    if (/^\&X\d+/ && !$all) {
	print;
	next;
    }
    chomp;
    my $query = $_;
    if ($query =~ /^\&(\S*)\s*=\s*(.*?)(?:\s*=|\s*$)/) {
	$query = $2;
	$thisX = $1;
#	warn "query=$query\n";
    } else {
	warn "$input:$.: expected `\& = [TEXT REFERENCE]' or similar\n";
    }
    if ($tab) {
	if ($tab{$query}) {
	    print "\&$tab{$query} = $query\n";
	} else {
	    warn "add-PQX.plx: no P-number for `$query'\n"
		unless $thisX =~ /^P\d\d\d\d\d\d/;
	    use Data::Dumper; warn Dumper \%tab unless $dumped++;
	}
    } else {
	$query =~ tr/-,.:;()//d;
	$query =~ s/\t.*//;
	$query =~ s/\s*[=\+].*$//;
	$query =~ s/\s+/_/g;
	if ($query =~ /[a-z0-9]/i) {
	    warn "$query\n" if $verbose;
	    my @res = `se \#cdli \!cat $query`;
	    if ($#res >= 0) {
		chomp @res;
		s/^\&(\S*)\s*=\s*//;
		my $oldX = $1;
		print "\&@res = $_ [$oldX]\n";
		next;
	    }
	}
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
