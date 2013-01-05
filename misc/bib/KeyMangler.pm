package ORACC::BIB::KeyMangler;
use strict; use warnings;

sub
canonicalize {
    $_[0] =~ tr/.,;:`'"()[]{}\/?!=*&+//d;
    $_[0] =~ s/--/-/g;
    $_[0] =~ s/([^0-9])-/$1/g;
    $_[0] =~ tr/A-Z/a-z/;
}

1;
