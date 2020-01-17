#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
use Getopt::Long;

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

my $block = 0;
my $default_renumber = 0;
my $number = 0;
my $primes = '';

GetOptions(
    'all'=>\$all,
    'v'=>\$verbose,
    );

$default_renumber = 1 if $all;

my $renumber = $default_renumber;

while (<>) {
    if (/^\&/ || /^\s*$/) {
	$block = 0;
	$renumber = $default_renumber if /^\&/;
    } elsif (/^\@/) {
	if ($block) {
	    warn("$.: \@-line found in block\n");
	} elsif (/^\@renumber/) {
	    $renumber = 1;
	} elsif (/^\@norenumber/) {
	    $renumber = 0;	    
	}
    } elsif (!/^[\#\$]/) {
	if ($block) {
	    if ($renumber) {
		if (!/^\!\!/ && s/^\!//) {
		    newnum();
		} else {
		    s/^\S+/$number$primes./;
		    ++$number;
		}
	    }
	} else {
	    if ($renumber) {
		if (s/^\!//) {
		    if (/^\!/) {
			s/^\S+/$number$primes./;
			++$number;
		    } else {
			newnum();
		    }
		} else {
		    s/^\S+/$number$primes./;
		    ++$number;
#		    warn("$.: no ! in first line of block\n");
		}
	    }
	    $block = 1;
	}
    }
    print;
}

sub
newnum {
    m/^(\S+?)\./;
    my $tmp = $1;
    if ($tmp =~ s/(\'+)$//) {
	$primes = $1;
    } else {
	$primes = '';
    }
    die "$.: bad number: $tmp\n" unless $tmp =~ /^\d+$/;
    $number = $tmp;
    ++$number;
}

1;
