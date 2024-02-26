#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;

my $not = 0;

GetOptions(
    n=>\$not,
    );

use lib "$ENV{'ORACC_BUILDS'}/lib";
use ORACC::SL::BaseC;

ORACC::SL::BaseC::init();
ORACC::SL::BaseC::pedantic();

my %not = ();
my %o = ();
my %okmap = ();

my %s = ();
my %sv = ();

load_not_but_OK() unless $not;

while (<>) {
    chomp;
    my($s,$o) = (/^(\S+)\s+=>\s+(.*?)$/);
    next unless $s;
    if ($o !~ /q00/ && $o !~ /\./) {
	my $sn = ORACC::SL::BaseC::sign_of($o);
	if (ORACC::SL::BaseC::is_value("$s;v")) {
	    ++${$sv{$sn}}{$s};
	} elsif ($s =~ /^(.*?)\((.*?)\)$/) {
	    my($v,$sn) = ($1,$2);
	    ++${$sv{$sn}}{$v};
	} else {
	    ++${$sv{$sn}}{'#'};
	}
    } else {
	if (/(\|.*?\|)/) {
	    my $c = $1;
	    if ($okmap{$c}) {
		if (/^(.*?)\(/) {
		    my $v = $1;
		    ++${$sv{$okmap{$c}}}{$v};
		} else {
		    ++${$sv{$okmap{$c}}}{'#'};
		}
		next;
	    }
	}
	++$not{$s};
    }
}

if ($not) {
    open(N,'>not-in-ogsl.lst') || die;
    print N join("\n", sort(keys %not)), "\n";
    close(N);
} else {
    open(SV,'>rep.q') || die;
    foreach my $sv (sort keys %sv) {
#	print SV "$sv\t";
#	my @sv = grep(/[^#]/, sort keys %{$sv{$sv}});
#	print SV "@sv", "\n";
	foreach my $v (sort keys %{$sv{$sv}}) {
	    if ($v eq '#') {
		print SV "$sv\n";
	    } else {
		print SV "$v($sv)\n";
	    }
	}
    }
    close(SV);
}

#######################################################

sub load_not_but_OK {
    my @ok = `grep OK not-in-ogsl.tab`; chomp @ok;
    foreach my $ok (@ok) {
	$ok =~ /\t(\S+)\tOK~(\S+)/;
	$okmap{$1} = $2;
    }
}

1;
