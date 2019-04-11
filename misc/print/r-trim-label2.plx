#!/usr/bin/perl
use warnings; use strict; use open 'utf8';

binmode STDOUT, 'utf8';

use lib "$ENV{'ORACC_BUILDS'}/lib";
use ORACC::XML;

my $f = shift @ARGV;
my $x = load_xml($f);

my @rr = tags($x,'http://oracc.org/ns/xis/1.0','rr');
foreach my $rr (@rr) {
    my $last_pre_col = '';
    foreach my $r ($rr->childNodes()) {
	my $l2 = $r->getAttribute('label2');
	if ($l2) {
#	    warn "l2=$l2\n";
	    my ($pre_col,$post_col) = ($l2 =~ /^(.*?):(.*?)$/);
	    if ($pre_col eq $last_pre_col) {
		$r->setAttribute('label2',$post_col);
	    } else {
		$last_pre_col = $pre_col;
	    }
	} else {
	    warn "no label2\n";
	}
    }
}

$f =~ s/\./+./;
open(X,">$f");
print X $x->toString();
close(X);

1;
