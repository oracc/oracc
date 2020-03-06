#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

my %tab = ();
open(T,'edit-me.tab') || die;
while (<T>) {
    chomp;
    s/\s+/\t/g;
    my($f,$b,$bm,$m) = split(/\t/,$_);
    $b =~ tr/o/·/;
    $b =~ s/{geš}/{ŋeš}/g;
    $b =~ s/-gar/-ŋar/g;
    $b =~ s/-ga₂/-ŋa₂/g;
    $b =~ s/-{ga₂}gar/-{+ŋa₂}ŋar/g;
    $b =~ s/-gal₂/-ŋal₂/g;
    $tab{$f} = [ $b , $m ];
}
close(T);

while (<>) {
    if (/^\@form\S*\s+(\S+)/) {
	my $f = $1;
	if ($tab{$f}) {
	    my @finfo = @{$tab{$f}};
	    s#/(\S+)#/$finfo[0]#;
	    if ($finfo[1]) {
		s/#(\S+)/#~,$finfo[1]/;
	    }
	}
    }
    print;
}

1;
