#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC_BUILDS'}/lib";
use ORACC::XML;

my %s = ();
my $session = '';

my @sfiles = `grep -l '^#session ' 00lib/*.lst`; chomp @sfiles;

foreach my $f (@sfiles) {
    open(F,$f) || die "$0: can't open $f for read\n";
    while (<F>) {
	if (/^#session \s*(.*?)\s*$/) {
	    $session = xmlify($1);
	} elsif (/^#/ || /^\s*$/) {
	    next;
	} else {
	    my $pqxid = '';
	    s/\@.*$//;
	    if (/:(\S+)/) {
		$pqxid = $1;
	    } else {
		/^(\S+)/;
		$pqxid = $1;
	    }
	    if ($pqxid =~ /^[PQX]\d\d\d\d\d\d$/) {
		$s{$pqxid} = $session;
	    } else {
		warn "$0: bad ID $pqxid\n";
	    }
	}
    }
}

system 'mkdir', '-p', '00lib/cat.d';
open(S,'>00lib/cat.d/sessions.xml') || die "$0: can't open 00lib/cat.d/sessions.xml to write sessions data\n";
print S '<?xml version="1.0" encoding="UTF-8"?><session-data unwrapped="1">';
foreach my $id (sort keys %s) {
    print S "<record xml:id=\"$id\"><session>$s{$id}</session></record>";
}
print S '</session-data>';
close(S);

1;
