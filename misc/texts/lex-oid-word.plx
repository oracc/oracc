#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDERR, ':utf8'; binmode STDOUT, ':utf8';

use Encode;
use lib "$ENV{'ORACC_BUILDS'}/lib/";
use ORACC::OID;
use ORACC::XML;

my %badwords = ();
my $xid = 'x1000000';
oid_init('sux');

my $master = load_xml("word-master.xml");
foreach (tags($master, 'http://oracc.org/ns/lex/1.0', 'word')) {
    my $lang = $_->getAttribute('lang');
    if ($lang && $lang =~ /^sux/) {
	my $word = $_->getAttribute('cfgw') || 'x';
	my $oid = oid_lookup('sux', $word);
	if ($oid) {
	    $_->setAttribute('oid', $oid);
	} else {
	    $badwords{$word} = ++$xid unless $badwords{$word};
	    $_->setAttribute('oid', $badwords{$word});
	}
    }
}

my $s = $master->toString();
Encode::_utf8_on($s);
open(X,'>word-master.xml'); print X $s; close(X);

foreach my $bad (sort keys %badwords) {
    warn "$0: no sux OID for word $bad\n";
}

1;
