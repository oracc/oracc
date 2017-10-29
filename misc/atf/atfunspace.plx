#!/usr/bin/perl                                                                 use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

my @reps = ();
my @pats = ();

open(R,'00src/rep.tab'); # right now this is just for my work on u3adm
while (<R>) {
    chomp;
    my($from,$to) = split(/\t/,$_);
    my @p1 = map { "$b$_" } split(/\s+/,$from);
    my @p2 = map { "$b$_" } split(/[\s+-]/,$to);
    
    push @p1, $b;
    push @p2, $b;
    print "s/@p1/@p2/g\n";
}
close(R);

1;
