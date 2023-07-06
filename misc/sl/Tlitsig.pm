package ORACC::SL::Tlitsig;
use warnings; use strict; use utf8; use open 'utf8';
use lib "@@ORACC@@/lib";
use IPC::Open2;

#### Harness for new c2 tlitsig which replaces need for BaseC in CBD/Validate

binmode STDERR, ':utf8';
use Encode;
use Fcntl;

sub
tlitsigx {
    print SL_IN "$_[0]\n";
    my $res = <SL_OUT>;
    if (defined $res) {
	chomp($res);
    } else {
	warn "slseA internal error on input $_[0]\n";
    }
    $res;
}

sub tlit_sig {
    my($context,$test) = @_;
    my $s = tlitsigx($test);
    if ($s =~ /q/ && $test =~ /\|/) {
	$test =~ tr/|//d;
	$s = tlitsigx($test);
    }
    return $s;
}

sub messages {
    my @tmp = @messages;
    @messages = ();
    return @tmp;
}

sub
init {
    return if $loaded;
    my $tsv_file = "@@ORACC@@/pub/ogsl/sl/sl.tsv";
    if (-r $tsv_file) {
	$loaded = 1;
    } else {
	die "ORACC::SL::BaseC: signlist data $tsv_file non-existent or unreadable\n";
    }

    # open the tlitsig engine for write and read
    $sl_pid = open2(\*SL_OUT, \*SL_IN, "@@ORACC@@/bin/tlitsig");
    binmode SL_OUT, ':utf8';
    binmode SL_IN, ':utf8';

    $loaded = 1;

}

sub
term {
    return unless $loaded;
    print SL_IN "\cD\n";
    $loaded = 0;
}

1;
