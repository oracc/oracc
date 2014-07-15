#!/usr/bin/perl
use warnings; use strict;

use Getopt::Long;

my $basedir = '';
my $project = '';
my $standalone = 0;

GetOptions(
    'basedir:s'=>\$basedir,
    'project:s'=>\$project,
    'standalone'=>\$standalone,
);

my $indir = '';
my $outdir = '';

if ($standalone) {
    $indir = '.';
    $outdir = 'out';
} else {
    $project = `oraccopt` unless $project;
    die "esp2-stylesheets.plx: must run in a project directory\n" unless $project;
    $indir = "$ENV{'ORACC'}/lib/esp2/css"; # '00web/00config'
    $outdir ="$basedir/css";
}

system 'mkdir', '-p', $outdir;

my %app = ();
my %class = ();
my %classmap = (
    medium=>'mid',
    outline=>'border',
    alternate=>'heading-bg',
    highlight=>'hover-fg',
    print=>'#000',
    text=>'#333',
    page=>'#fff',
    );
my @colours = qw/dark medium light outline alternate text page highlight/;
my @cssfiles = ('screen.css','print.css');
my %usedfor = ();

my $p3colours = find_p3colours() || die "esp2-stylesheet.plx: can't find p3colours.txt\n";
open(P, $p3colours) || die "esp2-stylesheet.plx: can't open $p3colours\n";
while (<P>) {
    if (/^colou?r-def-(.*?)\s+(\S+)$/) {
	$class{$1} = $2;
    } elsif (/^colou?r-bind-(.*?)\s/) {
	my $class = $1;
	my $usedfor = '';
	if (/\s+(.+)$/) {
	    my $ulist = $1;
	    foreach my $u (split(/\s+/, $ulist)) {
		$usedfor{$u} = $class;
	    }
	}
    }
}
close(P);

foreach my $c (@colours) {
    if ($class{$c}) {
	$app{$c} = $class{$c};
    } elsif ($usedfor{$c}) {
	$app{$c} = $class{$usedfor{$c}};
    } elsif ($classmap{$c}) {
	my $m = $classmap{$c};
	if ($m =~ /^#/) {
	    $app{$c} = $classmap{$c};
	} elsif ($class{$m}) {
	    $app{$c} = $class{$m};
	} elsif ($usedfor{$m} && $class{$usedfor{$m}}) {
	    $app{$c} = $class{$usedfor{$m}};
	} else {
	    warn "esp2-stylesheet.plx: colours value for '$c': no map for $m in $p3colours\n";
	    $app{$c} = '#000';
	}
    } else {
	warn "esp2-stylesheet.plx: no definition for colour '$c'\n";
	$app{$c} = '#000';
    }
}

foreach my $f (@cssfiles) {
    cssfile($f);
}

############################################################################################

sub
cssfile {
    my $f = shift;
    my $in = "$indir/$f";
    my $out = "$outdir/$f";
    open(OUT, ">$out"); select OUT;
    subfile($in);
    close(OUT);
}

sub
subfile {
    my $f = shift;
    my $fh = undef;
    open($fh, "$f");
    die "esp2-stylesheets.plx: unable to open '$f' for read\n" unless $fh;
    while (<$fh>) {
	if (/^\@file\s+(.*?)\s*$/) {
	    subfile("$indir/$1");
	} else {
	    s/\$([a-z]+)/$app{$1}/g;
	    print;
	}
    }
    close $fh;
}

sub
find_p3colours {
    if ($standalone) {
	if (-r 'p3colours.txt') {
	    return 'p3colours.txt';
	}
    }
    my $up = '';
    while (1) {
	my $try = "${up}00lib/p3colours.txt";
	if (-r $try) {
	    return $try;
	} else {
	}
	$up = "$up../";
	last unless -d $up;
    }
    undef;
}
