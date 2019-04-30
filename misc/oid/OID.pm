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
    if ($d eq 'sl' || !$d) {
	ORACC::SL::BaseC::init();
    }

    %oid;
}

sub oid_lookup {
    my($type,$sign) = @_;
    oid_init() unless $inited;
    if ($type eq 'sl') {
	if ($sign =~ /^\|/) {
	    if (!$oid{$type,$sign}) {
		my $res = ORACC::SL::BaseC::c10e_compound($sign);
		if ($res) {
		    warn "OID.pm: mapping $sign to $res via c10e\n" if $ORACC::OID::verbose;
		    $sign = $res;
		}
	    }
	} else {
	    my $res = ORACC::SL::BaseC::sign_of($sign);
	    if ($res && $res ne $sign) {
		warn "OID.pm: mapping $sign to $res via OGSL\n" if $ORACC::OID::verbose;
		$sign = $res;
	    }
	}
    }
    my $ret = $oid{$type,$sign};
#    unless ($ret) {
#	my $sign2 = $sign; $sign2 =~ tr/|//d;
#	my $s = ORACC::SL::BaseC::tlit_sig('oid',$sign2);
#	if ($s) {
#	    my $ss = '';
#	    warn "OID.pm: $sign => sig $s\n" if $ORACC::OID::verbose;	    
#	}
#    }
    $ret;
}

sub oid_term {
    ORACC::SL::BaseC::term();
}

1;
