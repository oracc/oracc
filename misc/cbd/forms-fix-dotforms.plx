#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use Data::Dumper;

use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;
use ORACC::CBD::Fix;

$ORACC::CBD::nosetupargs = 1;

my %args = pp_args(); $args{'mode'} = 'glossary'; $args{'stdout'} = 1;

$ORACC::CBD::nonormify = 1;

my %f = pp_fix_load_fixes(\%args,0);

# print Dumper \%f;

# use Data::Dumper; warn Dumper \%f; exit 1;

if ($args{'forms'}) {
    if ('-' eq $args{'forms'}) {
	while (<>) {
	    fix_dotform($_);
	}
    } elsif (-r $args{'forms'}) {
	open(F,$args{'forms'}) || die;
	while (<F>) {
	    fix_dotform($_);
	}
	close(F);
    } else {
	die "$0: -forms FILE $args{'forms'} nonexistent or unreadable\n";
    }
} else {
    die "$0: must give -forms FILE on command line\n";
}

###########################################################

sub fix_dotform {
    my($cfgw) = ($_[0] =~ m/^(.*?)\t/);
    if ($cfgw) {
	my $xcfgw = $cfgw;
	$xcfgw =~ s/\s+(\[.*?\])\s+/$1/;
	my $mcfgw = $f{$cfgw} || $f{$xcfgw};
	if ($mcfgw) {
	    $mcfgw =~ s/\s*(\[.*?\])\s*/ $1 /;
	    $_[0] =~ s/^.*?\t//;
	    print "$mcfgw\t$_[0]";
	} else {
	    print $_[0];
	}
    } else {
	warn "$args{'forms'}:$.: syntax error in dotforms line\n";
    }
}

1;
