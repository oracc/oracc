#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";

use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;
use ORACC::CBD::Map;

use Data::Dumper;

my @input = ();
my $lang = '';
my $proj = '';
my $sigs = '';

$ORACC::CBD::nosetupargs = 1;
my %args = pp_args();

my $qflag = 0;
if ($args{'lang'} && $args{'lang'} =~ /^qpn/) {
    $qflag = 1;
}


my $map = shift @ARGV; $map = '' unless $map;
die "$0: can't open map $map\n" unless $map && -r $map;

if ($args{'auto'}) {
    ($proj,$lang) = ($map =~ m#^01map/(.*?)~(.*?)\.map#);
    $lang = $args{'lang'} if $args{'lang'} && !$qflag; # don't use args{'lang'} if it is 'qpn'
    $proj =~ tr#-#/#;
    $sigs = "$ENV{'ORACC_BUILDS'}/$proj/01bld/from-xtf-glo.sig";
    unless (-r $sigs) {
	my $sigs2 = "$ENV{'ORACC_BUILDS'}/$proj/01bld/from-prx-glo.sig";
	unless (-r $sigs2) {
	    die "$0: can't find $sigs or $sigs2\n";
	} else {
	    $sigs = $sigs2;
	}
    }
    
    warn "applying $map to $proj/$lang from $sigs\n";
    if (-r $sigs) {
	open(M,$sigs) || die "$0: can't open $sigs for read\n";
	@input = (<M>);
	close(M);
    }
    my $out = $map;
    $out =~ s/01map/01sig/; $out =~ s/map$/sig/;
    open(OUT,">$out") || die "$0: can't open $out for write\n";
    select OUT;
} else {
    @input = (<>);
}

my %map = map_load($map,'sigs');
#warn Dumper \%map;
foreach (@input) {
    if (/^\@[^%]+\s/ || /^\s*$/ || /^#/) {
	print;
    } else {
	if ($lang) {
	    if ($qflag) {
		next unless /\][A-Z]N'/;
	    } else {
		next if /\][A-Z]N'/;
	    }
	    print map_apply_sig(\%args,$_) if /\%$lang/;
	} else {
	    print map_apply_sig(\%args,$_);
	}
    }
} 

if ($args{'auto'}) {
    close(OUT);
}

1;
