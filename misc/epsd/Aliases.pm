package ORACC::Aliases;
use warnings; use strict;

sub
as_array {
    open(A, "$ENV{'ORACC'}/pub/ogsl/aliases.dumb") || die;
    binmode A, ':utf8';
    my @a = (<A>);
    close(A);
    @a;
}

1;
