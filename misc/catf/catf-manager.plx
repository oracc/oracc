#!/usr/bin/perl
use warnings; use strict; use open ':utf8';
use Encode;
use Getopt::Long;
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;
use ORACC::CATF;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';

my $curated = `oraccopt . catf-maintained`;
die "catf-manager.plx: no 'catf-maintained' option in config.xml. Stop."
    unless $curated;
die "catf-manager.plx: illegal value `$curated' for option 'catf-maintained'. Must be 'yes' or 'no'. Stop."
    unless $curated =~ /^(?:yes|no)$/;
my $stdin = 0;
GetOptions(
    's'=>\$stdin
    );

my $catf_transform = "$ENV{'ORACC'}/lib/scripts/xtf-CATF.xsl";
my $catfmaker = load_xsl($catf_transform);

if ($stdin) {
    undef $/; $_ = (<>);
    catf_xtfdoc(load_xml_string($_));
} else {
    while (<>) {
	chomp;
	catf_xtfdoc(load_xml($_));
    }
}

sub
catf_xtfdoc {
    my $credit = `oraccopt . catf-credit`;
    $credit = '' unless $credit;
    $credit =~ s/'/&apos;/g;
    my $xcatf = $catfmaker->transform($_[0], 
				      credit=>"'$credit'",
				      curated=>"'$curated'");
    my $catf = $catfmaker->output_as_bytes($xcatf);
    Encode::_utf8_on($catf);
    print ORACC::CATF::catf_fixer(split(/\n/, $catf)), "\n";
}

1;
