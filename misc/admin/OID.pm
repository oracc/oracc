package ORACC::OID;
require Exporter;
@ISA=qw/Exporter/;

@EXPORT = qw/oid_init oid_lookup/;

use warnings; use strict; use open 'utf8'; use utf8;

use Data::Dumper;

my %oid = ();

sub oid_init {
    if (open(O, "$ENV{'ORACC_BUILDS'}/oid/oid.tab")) {
	while (<O>) {
	    my($oid,$key) = split(/\t/, $_);
	    $oid{$key} = $oid;
	}
	close(O);
    } else {
	warn "$0: can't read OID file\n";
    }
}    

sub oid_lookup {
#    warn "oid_lookup passed $_[0]\n";
    $oid{$_[0]};
}

1;
