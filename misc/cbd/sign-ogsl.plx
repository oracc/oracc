#!/usr/bin/perl
use warnings; use strict; use utf8; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

my $tag = shift || undef;

while (<>) {
    /(\S+)\s+not in sign/ || next;
    my $s = $1;
    $s =~ tr/'/|/;
    $s =~ s/^\)//;
    warn "no sign in $_\n" unless $s;
    print <<EOS
\@sign $s
\@inote $tag
\@end sign

EOS
}

1;
