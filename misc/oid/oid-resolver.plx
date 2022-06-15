#!/usr/bin/perl
use warnings; use strict; use open 'utf8';

my $prj = shift @ARGV || '';
my $oid = shift @ARGV || '';
my $lng = shift @ARGV || '';
my $xml = shift @ARGV || '';

#warn "prj=$prj; oid=$oid; lng=$lng; xml=$xml\n";

if (-r "$ENV{'ORACC_BUILDS'}/xml/$prj/config.xml") {
    if ($oid =~ /^[ox]\d+$/) {
	my $idx = "$ENV{'ORACC_BUILDS'}/pub/$prj/oid-index.tab";
	if (-r $idx) {
	    my $o = `grep '^$oid' $idx`;
	    if ($o) {
		chomp $o;
		$o =~ s#^.*?\t#$ENV{'ORACC_BUILDS'}/#;
		system "sed 's#\@\@OB\@\@#$ENV{'ORACC_BUILDS'}#g' $o | xmllint --xinclude -";
	    } else {
		oid_404("OID $oid not found in project $prj");
	    }
	} else {
	    if ($xml) {
		exec "$ENV{'ORACC_BUILDS'}/bin/xfrag", '-c',
		"$ENV{'ORACC_BUILDS'}/bld/$prj/$lng/articles-with-periods.xml", $oid;
	    } else {
		my $html = "$ENV{'ORACC_BUILDS'}/www/$prj/cbd/$lng/$oid.html";
		if (-r $html) {
		    exec 'cat', $html;
		} else {
		    oid_404("OID $oid not known in $prj/$lng (looked for $html)");
		}
	    } 
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
