#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
use Getopt::Long;

my $install = 0;
my $map = '';
my %map = ();
my %seen = ();
my $status = 0;

GetOptions(
    'map:s'=>\$map,
    );

die "un-X.plx: usage:\n\tun-X.plx -map [MAPFILE]\n" unless $map;
die "un-X.plx: [MAPFILE] `$map' not readable\n" unless -r $map;
die "un-X.plx: no 00atf directory\n" unless -d '00atf';

open(M, $map) || die;
while (<M>) {
    my($f1,$f2) = (/^([PQX]\d+).([PQX]\d+)$/);
    if ($f1) {
	if ($f1 =~ /^X/) {
	    if ($f2 =~ /^X/) {
		warn "$map:$.: both IDs start with X\n";
		++$status;
	    } else {
		if ($seen{$f1}++) {
		    warn "$map:$.: already saw ID $f1\n";
		    ++$status;
		} else {
		    if ($seen{$f2}++) {
			warn "$map:$.: already saw ID $f2\n";
			++$status;
		    } else {
			$map{$f1} = $f2;
		    }		    
		}
	    }
	} elsif ($f2 =~ /^X/) {
	    if ($f1 =~ /^X/) {
		warn "$map:$.: both IDs start with X\n";
	    } else {
		if ($seen{$f1}++) {
		    warn "$map:$.: already saw ID $f1\n";
		    ++$status;
		} else {
		    if ($seen{$f2}++) {
			warn "$map:$.: already saw ID $f2\n";
			++$status;
		    } else {
			$map{$f2} = $f1;
		    }		    
		}
	    }
	} else {
	    warn "$map:$.: neither ID starts with X\n";
	    ++$status;
	}
    } else {
	warn "$map:$.: bad syntax, must be ID<SEP>ID, no spaces, <SEP> must be non-alphanumeric\n";
	++$status;
    }
}
close(M);

if ($status) {
    warn "$status errors in map file $map; no mapping of ATF files will be performed\n";
    exit 1;
}

map_atf();

install_atf() if $install;

1;

###################################################################################################

sub
map_atf {
    system 'mkdir', '-p', '01tmp/unX';
    my @atf = (<00atf/*.atf>);
    undef $/;
    foreach my $atf (@atf) {
	open(A, $atf);
	my $A = <A>;
	close(A);
	$A =~ s/(X\d\d\d\d\d\d)/unX($atf,$1)/eg;
	$atf =~ s#00atf#01tmp/unX/#;
	open(A, ">$atf");
	print A $A;
	close(A);
    }
}

sub
unX {
    my($atf,$X) = @_;
    if ($map{$X}) {
	$map{$X};
    } else {
	warn "unmapped X $X in file $atf\n";
	$X;
    }
}

1;
