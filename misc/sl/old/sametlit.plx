#!/usr/bin/perl
use warnings; use strict; use open ':utf8';
use lib '/home/oracc/lib';
use ORACC::SL::Base;
binmode(STDIN,':utf8'); binmode(STDOUT,':utf8'); binmode(STDERR,':utf8');
ORACC::SL::Base::init(1);
while (<>) {
    my($t,@a) = (split(/[;,\s]/,$_));
    my $res = ORACC::SL::Base::same_tlit("STDIN:$.: ",$t,@a);
    if ($res) {
	warn("$t matches $res\n");
    } else {
	warn("$t not found in @a\n");
    }
}
ORACC::SL::Base::term();
1;
