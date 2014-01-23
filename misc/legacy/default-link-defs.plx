#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
my $project = `oraccopt`;
die "default-link-defs.plx must be run from a project directory. Stop.\n" unless $project;
my @atf = <00atf/*.atf>;
foreach my $a (@atf) {
    open(A,$a) || die;
    my @a = (<A>);
    close(A);
    foreach my $l (@a) {
	if ($l =~ /^#link/) {
	    $l =~ s/=\s+([PQX])/= $project:$1/;
	    $l =~ s/parallel\s*([PQX])/parallel $project:$1/;
	} elsif ($l =~ /^\@incl/) {
	    $l =~ s/include\s*([PQX])/include $project:$1/;
	}
    }
    open(A,">$a") || die;
    print A @a;
    close(A);
}
1;
