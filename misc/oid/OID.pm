package ORACC::OID;
require Exporter;
@ISA=qw/Exporter/;

@EXPORT = qw/oid_init oid_load_domain oid_lookup/;

use warnings; use strict; use open 'utf8'; use utf8;

use Data::Dumper;

my %oid = ();
my $inited = 0;

sub oid_load_domain {
    oid_init(@_);
}

sub oid_init {
    my $d = shift @_;
    if (open(O, "$ENV{'ORACC_BUILDS'}/oid/oid.tab")) {
	if ($d) {
	    while (<O>) {
		my($oid,$dom,$key) = split(/\t/, $_);
		$oid{$dom,$key} = $oid if ($d eq $dom);
	    }
	    close(O);
	} else {
	    while (<O>) {
		my($oid,$dom,$key) = split(/\t/, $_);
		$oid{$dom,$key} = $oid;
	    }
	    close(O);
	}
    } else {
	warn "$0: can't read OID file\n";
    }
    ++$inited;
    %oid;
}    

sub oid_lookup {
    oid_init() unless $inited;
    $oid{$_[0],$_[1]};
}

1;
