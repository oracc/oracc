#!/usr/bin/perl
use warnings; use strict;
open(L,"01tmp/json.log") || die "errors-json.log: no file 01tmp/json.log\n";
my @lines = (<L>);
close(L);

foreach (my $i = 0; $i <= $#lines; ++$i) {
    if ($lines[$i] =~ /^Creating/) {
	if ($lines[$i+1] =~ /^Creating/ || $lines[$i+1] =~ /^$/) {
	    $lines[$i] = 'DEL';
	}
    } elsif ($lines[$i] =~ /^Validating/) {
	if ($lines[$i+1] =~ /^Validating/ || $lines[$i+1] =~ /^$/) {
	    $lines[$i] = 'DEL';
	}
    }
}

my @nlines = grep(!/^DEL/, @lines);
@nlines = grep(!/^$/, @nlines);
if ($#nlines >= 0) {
    open(E,'>01tmp/json-error.log') 
	|| die "errors-json.log: can't write 01tmp/json-error.log\n";
    print E @nlines;
    close(E);
} else {
    unlink('01tmp/json-error.log');
}

1;
