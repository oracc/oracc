#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

use lib "$ENV{'ORACC'}/lib";
use ORACC::Texts::Util;

while (<>) {
    my($id,$er) = split(/\t/,$_);
    # arg 1 means prepend project to errline
    $id =~ s/\+.*$//;
    my $fl = wid2err($id,1);
    if ($fl) {
	my $x = $id; $x =~ s/^.*?://;
	print "$fl: [\@$x] $er";
    }
}

1;
