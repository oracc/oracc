#!/usr/bin/perl
use warnings; use strict;
use Getopt::Long;
sub xsystem;

my $base = '';
my $dupcmd = 'ln';
my $duparg = '-sf';

my $phase = '';
my $project = undef;
my $verbose = 0;

GetOptions(
    'project:s'=>\$project,
    'verbose'=>\$verbose,
    ) || pod2usage(1);

$project = `oraccopt` unless $project;

$base = "$ENV{'ORACC'}";
my $bld = "$base/bld/$project";
my $bldtexts = "$base/bld/$project/[PQX]*";
my $pub = "$base/pub/$project";
my $xml = "$base/xml/$project";
my $xdb = "$base/$project/02xml";
my $omltexts = "$base/$project/02xml/oml/texts";
my $lib = "$xdb/lib";
my $oml = "$xdb/oml";
my $tei = "$xdb/tei";
my $usr = "$xdb/usr";
my $xml00 = "$ENV{'ORACC'}/$project/00xml";

$phase = 'initialization';
my @texts = ();
if (open(A,'01bld/lists/approved.lst')) {
    @texts = (<A>);
    close(A);
    @texts = map { tr/\n\r \t//d; $_ } @texts;
}

my @cbds = (<00lib/*.glo>);

# my @sign = 'results/signlist.xml';

xsystem 'rm', '-fr', $oml, $lib, $usr;
xsystem 'mkdir', '-p', $oml, $lib, $usr;

$phase = 'texts';
#foreach my $t (@texts) {
#    text($t);
#}
link_texts();

$phase = 'gloss';
my $i = '01';
foreach my $c (sort @cbds) {
    $c =~ s#00lib/(.*?)\..*$#$1#;
    gloss($c,$i);
#    ++$i; # all glossary files are 01 prefix
}

$phase = 'signs';

$phase = 'admin';

xsystem $dupcmd, $duparg, "$xml/config.xml", "$oml/00config.xml";
xsystem "chmod -R o+r $oml";

xsystem "ln -sf $xml00/lib/* $lib" if hasfiles("$xml00/lib");
xsystem "ln -sf $xml00/usr/* $usr" if hasfiles("$xml00/usr");
xsystem "ln -sf $xml/00tei $tei" if hasfiles("$xml/00tei");

# create XML framework with XIncludes
create_xml();

####################################################################

sub
create_xml {
    my @flist = getfiles();
    chomp @flist;
    xsystem 'rm', '-f', "$pub/project.xml";
    open(X,">$pub/project.xml");
    print X '<?xml version="1.0" encoding="utf-8"?>', "\n";
    print X "<project xmlns:xi=\"http://www.w3.org/2001/XInclude\" xml:base=\"$xml/\">";
    foreach my $f (@flist) {
	$f =~ s#^02xml/##;
	print X "<xi:include href=\"$f\"/>";
    }
    print X '</project>';
    close(X);
}

sub
getfiles {
    # if we used find -L here we could include the 
    # tei stuff as well, but it seems better for now
    # to keep project.xml to just the oml stuff
    `find $xdb -follow -type f | sort`;
}

sub
hasfiles {
    my $dir = shift;
    my $hasfiles = 0;
    my $f = undef;
    if (opendir(D,$dir)) {
	while (($f = readdir D)) {
	    if (-f "$dir/$f" || (-d "$dir/$f" && $f !~ /^\.*$/)) {
		$hasfiles = 1;
		last;
	    }
	}
	closedir(D);
    }
    $hasfiles;
}

sub
gloss {
    my($l,$pref) = @_;
    if (-e "$bld/$l/$l.g2c") {
	xsystem $dupcmd, $duparg, "$bld/$l/$l.g2c", "$oml/$pref$l.g2c";
    }
    if (-e "$bld/$l/$l.xis") {
	xsystem $dupcmd, $duparg, "$bld/$l/$l.xis", "$oml/$pref$l.xis";
    }
}

sub
link_texts {
    if (-d $omltexts) {
	xsystem 'rm', '-fr', $omltexts;
    }
    xsystem 'mkdir', '-p', $omltexts;
    foreach (eval("<$bldtexts>")) {
	xsystem 'ln', '-sf', $_, $omltexts;
    }
}

sub
text {
    my $PQ = shift;
    warn "xmlmanager.plx: bad text ID `$PQ'\n" and return
	unless $PQ =~ /^[A-Z]\d{6,}$/;
    xsystem 'mkdir', '-p',"$oml/$PQ";
    xsystem 'chmod', 'o+rx',"$oml/$PQ";
    my $four = $PQ; $four =~ s/^(....).*$/$1/;
    my $PQdir = "$bld/$four/$PQ";
    foreach my $f (<$PQdir/*>) {
	next if $f =~ /txh$/;
	xsystem 'ln', '-sf', $f, "$oml/$PQ";
    }
}

sub
xsystem {
    warn "$phase: system @_\n" if $verbose;
    system @_;
}

1;
