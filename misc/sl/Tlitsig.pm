package ORACC::SL::Tlitsig;
use warnings; use strict; use utf8; use open 'utf8';
use IPC::Open2;
binmode STDERR, ':utf8';

#### Harness for new c2 tlitsig which replaces need for BaseC in CBD/Validate

use Fcntl;

my $loaded = 0;
my @messages = ();
my $pedantic = 0;

sub
init {
    return if $loaded;
    my $tsv_file = "@@ORACC@@/pub/ogsl/sl/sl.tsv";
    if (-r $tsv_file) {
	$loaded = 1;
    } else {
	die "ORACC::SL::BaseC: signlist data $tsv_file non-existent or unreadable\n";
    }
    eval {
	my $p = ($pedantic ? "p" : "");
	open2(\*SL_OUT, \*SL_IN, "@@ORACC@@/bin/tlitsig -h$p");
    };
    if ($@) {
	die "tlitsig: $@\n";
    } else {
	binmode SL_OUT, ':utf8';
	binmode SL_IN, ':utf8';
    }
}

sub messages {
    my @tmp = @messages;
    @messages = ();
    return @tmp;
}

sub pedantic {# return the previous value
    my $ret = $pedantic;
    if (defined $_[0]) {
	$pedantic = $_[0];
    } else {
	$pedantic = 1;
    }
    $ret;
}

sub sig {
    my($context,$test) = @_;
    my $s = tlitsigx($test);
    if ($s && $s =~ /q/ && $test =~ /\|/) {
	$test =~ tr/|//d;
	$s = tlitsigx($test);
    }
    return $s;
}

sub
term {
    return unless $loaded;
    print SL_IN "\cD\n"; # tlitsig exits on ^D so no need to wait $pid 0 here 
    $loaded = 0;
}

sub
tlitsigx {
    print SL_IN "$_[0]\n";
    my $res = undef;
    while (1) {
	$res = <SL_OUT>;
	if ($res && $res =~ s/^>>//) { # error message
	    $res =~ s/^.*?:\s+//; chomp($res);
	    ### FIXME: add context somehow
	    push @messages, $res;
	} else {
	    last;
	}
    }    
    if (defined $res) {
	chomp($res);
    } else {
	warn "tlitsig internal error on input $_[0]\n";
    }
    $res;
}

1;
