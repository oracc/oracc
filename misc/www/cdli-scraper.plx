#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

my $lib = "$ENV{'ORACC_BUILDS'}/lib";
my $scripts = "$lib/scripts";
my $data = "$lib/data";

use Getopt::Long;

my $arg_outfile = '';
my $raw = 0;

GetOptions(
    'outfile:s'=>\$arg_outfile,
    raw=>\$raw,
    );

my $p = shift @ARGV;

my $outfile = $arg_outfile || "$ENV{'ORACC_BUILDS'}/www/epsd2/catalogue/cdli/$p.html";

unless ($p =~ /^P\d\d\d\d\d\d$/) {
    open(O,">$outfile") || die;
    print "<html><head><title>BAD SCRAPE</title></head><body><p>BAD SCRAPE ID $p</p></body></html>";
    close(O);
    exit 0;
}

unless ($raw) {
    open(O, "|tidy -config $data/tidy.conf | xsltproc $scripts/cdli-scraper.xsl - >$outfile 2>/dev/null") || die; select O;
}

my @p = `/usr/local/bin/wget -q -nv -O - https://cdli.ucla.edu/$p`;

my $output = 0;

my $has_version_history = 0;
foreach (@p) {
    if (/Version History/) {
	$has_version_history = 1;
	last;
    }
}

foreach (@p) {
    if (/<table.*?full_object_table/) {
	$output = 1;
	s/^.*?<table/<table/;
	s/ width="\d\d\d\d"//g;
	s/al ign/align/g;
	s/ width="\d+"/ width="33\%"/g; s#<table width=".*?"#<table #;
	s#"/dl#"https://cdli.ucla.edu/dl#g;
	s#</table><hr align.*$#</table>#
	    unless $has_version_history;
	print;
	last unless $has_version_history;
    } else {
	if (/Version History/) {
	    s#<hr al.*$##;
	    print;
	    last;
	} else {
	    print if $output;
	}	
    }
}

close(O) unless $raw;

1;
