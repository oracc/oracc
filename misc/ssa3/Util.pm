package ORACC::SSA3::Util;
use warnings; use strict; use open ':utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDERR, ':utf8';

sub set_data_dir {
    $ORACC::SSA3::data = shift;
}

sub open_data {
    my $d = shift;
    my $f = "$ORACC::SSA3::data/$d";
    if (-r $f) {
	open(DATA, $f);
    } else {
	die "$0: unable to open $f. Stop.\n";
    }
    return \*DATA;
}
