#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";

use ORACC::CBD::Hash;
use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;

my %args = pp_args();
$ORACC::CBD::novalidate = 1;

my @entries = ();
if ($args{'entries'}) {
    if ($args{'entries'} eq '-') {
	@entries = (<STDIN>);
    } else {
	open(E, $args{'entries'}) || die "cbdentries.plx: unable to open entries list '$args{'entries'}'\n";
	@entries = (<E>);
	close(E);
    }
    chomp @entries;
}
my $h = undef;
if (($h = pp_hash(\%args))) {
    if ($args{'lines'}) {
	@entries = pp_hash_cfgws($h) unless $#entries >= 0;
	my $err_mode = 0;
	foreach my $e (@entries) {
	    my $ee = $e;
	    if ($e =~ /^(.*?\[.*?):/) {
		$ee = $1;
		$err_mode = 1;
		chomp($e);
	    }
	    $ee =~ s#//.*?]#]#;
	    my $l = pp_hash_line($h,$ee);
	    if ($l) {
		print "$args{'cbd'}:$l: $e\n";
	    } else {
		warn "no ID for $e\n";
	    }
	}
    } else {
	print join("\n", pp_hash_cfgws($h)), "\n";
    }
} else {
    pp_diagnostics();
}

1;
