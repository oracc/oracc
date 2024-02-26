package ORACC::SL::Oracc2;
use warnings; use strict; use utf8; use open 'utf8';
use lib "@@ORACC@@/lib";
use ORACC::Legacy::Sexify;
use IPC::Open2;

binmode STDERR, ':utf8';
use constant UCODE => 1;
use constant UCHAR => 0;

use Encode;
use Fcntl;

sub o2tlitsig_init {
    # open the signlist engine for write and read
    $ts_pid = open2(\*TS_OUT, \*TS_IN, "@@ORACC@@/bin/tlitsig -h");
    binmode TS_OUT, ':utf8';
    binmode TS_IN, ':utf8';
    $tlitsig_loaded = 1;
}

sub o2tlitsig_term {
    return unless $tlitsig_loaded;
    print TS_IN "\cD\n";
    $loaded = 0;
}

sub o2tlitsig {
    # warn "slseA: trying $_[0]\n";
    print TS_IN "$_[0]\n";
    my $res = <TS_OUT>;
    if (defined $res) {
	chomp($res);
	# warn "slseA: got '$res'\n";
    } else {
	warn "slseA internal error on input $_[0]\n";
	$res = '';
    }
    $res;
}

1;

