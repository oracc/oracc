#!/usr/bin/perl
use warnings; use strict; use open ':utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';


if ($#ARGV > 0) {
} else {
    while (<>) {
	clean_par($_);
    }
}

#########################################################################

sub
clean_par {
    1 while $_[0] =~ s/\}([bi])\@\@\1\{//;
    1 while $_[0] =~ s/}{//;
    1 while $_[0] =~ s/\@([bi])\{\s*\}\1\@//;
    print $_[0];
}

sub
clean_text {
    my $f = shift;
    open(F,$f);
    my @f = (<F>);
    close(F);
    open(F,">$f"); select F;
    foreach my $p (@f) {
	clean_par($p);
    }
    close(F);
}

1;
