#!/usr/bin/perl
use warnings; use strict;

my @files = `find . -name '*.in'`;
chomp @files;
my @ignore = ('config', 'Makefile', 'mk_rnc');

foreach my $f (@files) {
    my $ignore = 0;
    foreach my $i (@ignore) {
	if ($f =~ /$i/) {
	    $ignore = 1;
	    last;
	}
    }
    next if $ignore;

    my $f_no_in = $f;
    $f_no_in =~ s/\.in$//;

    print "diffing $f_no_in ...\n";

    my @diff = `diff -w $f $f_no_in`;

    for (my $i = 0; $i < $#diff; ) {
	if ($diff[$i] =~ /^\d+c\d+/) {
	    if ($diff[$i+1] =~ /\@\@ORACC\@\@/) {
		$i += 4;
		next;
	    } else {
		print $diff[$i++];
	    }
	} else {
	    print $diff[$i++];
	}
    }
}

1;
