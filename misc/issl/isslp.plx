#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDERR, ':utf8'; binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';
use Getopt::Long;

my $entry = '';
my $harvest = 0;
my $input = '';
my $merge = 0;
my $output = '';
my $target = '';
my $validate = 0;

GetOptions(
    'harvest'=>\$harvest,
    'input:s'=>\$input,
    'merge'=>\$merge,
    'output:s'=>\$output,
    'target:s'=>\$target,
    );

if ($harvest) {
    die "isslp: must name file to harvest from with '-input' option\n"
	unless $input;
    die "isslp: unable to open harvest input '$input'\n"
	unless -r $input;
    if ($output) {
	die "isslp: unablet to open harvest output '$output'\n"
	    unless open(OUTPUT,">$output");
	select OUTPUT;
    }
    harvest();
} elsif ($merge) {
    die "isslp: must name file to merge from with '-input' option\n"
	unless $input;
    die "isslp: unable to open merge input '$input'\n"
	unless -r $input;
    die "isslp: must name file to merge into with '-target' option\n"
	unless $target;
    die "isslp: unable to open merge target output '$target'\n"
	unless -w $target;
    if ($output) {
	die "isslp: unablet to open merge output '$output'\n"
	    unless open(OUTPUT,">$output");
	select OUTPUT;
    }
    merge();
} else {
    die "isslp: must give '-harvest' or '-merge'\n";
}

1;

#######################################################################

sub
harvest {
    open(H, $input);
    while (<H>) {
	if (/^\@entry/) {
	    $entry = $_;
	} elsif (/^\@isslp/) {
	    print $entry if $entry;
	    print;
	    $entry = '' if $entry;
	}
    }
    close(H);
}

sub
merge {
    my %entry_map = ();
    my %input = ();
    open(I, "$input");
    while (<I>) {
	chomp;
	if (/^\@entry/) {
	    $entry = $_;
	    $entry =~ tr/ \t//d;
	    $entry_map{$entry} = $_;
	} else {
	    push @{$input{$entry}}, $_;
	}
    }
    close(I);
    my @pending = ();
    open(T,"$target");
    while (<T>) {
	if (/^\@entry/) {
	    my $e = $_;
	    chomp($e);
	    $e =~ tr/ \t//d;
	    if ($input{$e}) {
		@pending = @{$input{$e}};
		delete $input{$e};
	    }
	} elsif (/^\@isslp/ || /^\@end/) {
	    print(join("\n", @pending), "\n") if $#pending >= 0;
	    @pending = ();
	}
	print;
    }
    close(T);

    foreach my $e (sort keys %input) {
	print STDERR $entry_map{$e}, "\n", join("\n", @{$input{$e}}), "\n";
    }
}
