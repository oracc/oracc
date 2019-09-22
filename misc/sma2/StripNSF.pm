package ORACC::SMA2::StripNSF;

require Exporter;
@ISA=qw/Exporter/;
@EXPORT = qw/strip_nsf/;

use warnings;
use strict;

use ORACC::SMA2::NSF;

sub
strip_nsf {
    my $f = shift;
    my @f = split(/-/, $f);
    for (my $i = 0; $i < $#f; ++$i) {
	my $b = join('-', @f[0..$i]);
	if (is_nsf('',$b,@f[($i+1)..$#f])) {
	    return $b;
	}
    }
    undef;
}

1;
