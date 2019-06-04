#!/usr/bin/perl
use warnings; use strict; use open 'utf8';

my $prj = shift @ARGV || '';
my $oid = shift @ARGV || '';

if (-r "$ENV{'ORACC_BUILDS'}/xml/$prj/config.xml") {
    if ($oid =~ /^[ox]\d+$/) {
	my $idx = "$ENV{'ORACC_BUILDS'}/pub/$prj/oid-index.tab";
	if (-r $idx) {
	    my $o = `grep '^$oid' $idx`;
	    if ($o) {
		chomp $o;
		$o =~ s/^.*?\t//;
		system 'xmllint', '-xinclude', $o;
	    } else {
		oid_404("OID $oid not found in project $prj");
	    }
	} else {
	    oid_404("OIDs not used project $prj");
	}
    } else {
	oid_404("malformed OID.");
    }
} else {
    oid_404("No such project.");
}

sub oid_404 {
    print <<EOH;
<html>
<head><title>Oracc ID Failure</title></head>
<body>
<p>@_</p>
<p><a href="http://oracc.org">Please return to Oracc and try again</a></p>
</body>
</html>
EOH
}

1;
