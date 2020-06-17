package ORACC::OID;
require Exporter;
@ISA=qw/Exporter/;

@EXPORT = qw/oid_init oid_load_domain oid_lookup/;

$ORACC::OID::verbose = 0;

use warnings; use strict; use open 'utf8'; use utf8;
use lib "$ENV{'ORACC_BUILDS'}/lib/";
use ORACC::SL::BaseC;

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
    if (!$d || $d eq 'sl') {
	ORACC::SL::BaseC::init();
    }

    %oid;
}

sub oid_lookup {
    my($type,$key) = @_;
    oid_init() unless $inited;
    if ($type eq 'sl') {
	if ($key =~ /^\|/) {
	    if (!$oid{$type,$key}) {
		my $res = ORACC::SL::BaseC::c10e_compound($key);
		if ($res ne $key) {
		    warn "OID.pm: mapping $key to $res via c10e\n" if $ORACC::OID::verbose;
		    $key = $res;
		}
	    }
	} else {
	    my $res = ORACC::SL::BaseC::sign_of($key);
	    if ($res && $res ne $key) {
		warn "OID.pm: mapping $key to $res via OGSL\n" if $ORACC::OID::verbose;
		$key = $res;
	    }
	}
    } else {
	$key =~ s/\s*(\[.*?\])\s*/$1/;
    }
    my $ret = $oid{$type,$key};
#    unless ($ret) {
#	my $key2 = $key; $key2 =~ tr/|//d;
#	my $s = ORACC::SL::BaseC::tlit_sig('oid',$key2);
#	if ($s) {
#	    my $ss = '';
#	    warn "OID.pm: $key => sig $s\n" if $ORACC::OID::verbose;	    
#	}
#    }
    $ret;
}

sub oid_term {
    ORACC::SL::BaseC::term();
}

1;
