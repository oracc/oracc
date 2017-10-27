#!/usr/bin/perl
use warnings; use strict; use open ':utf8'; use utf8;
use Encode;
use Getopt::Long;
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';

my $stdin = 0;
GetOptions(
    's'=>\$stdin
    );

my $oatf_transform = "$ENV{'ORACC'}/lib/scripts/xtf-OATF.xsl";
my $oatfmaker = load_xsl($oatf_transform);

if ($stdin) {
    undef $/; $_ = (<>);
    oatf_xtfdoc(load_xml_string($_));
} else {
    while (<>) {
	chomp;
	oatf_xtfdoc(load_xml($_));
    }
}

sub
oatf_xtfdoc {
    my $xoatf = $oatfmaker->transform($_[0]);
    my $oatf = $oatfmaker->output_as_bytes($xoatf);
    print $xoatf;
#    Encode::_utf8_on($oatf);
#
#    foreach my $ln (split(/\n/,$oatf)) {
#	my @w = split(/\t/,$ln);
#	my @r1 = map { my $x = $_; $x =~ s/^.*?\N{U+2E21}//; $x } @w;
#	my @r2 = map { s/^\N{U+2E20}(.*?)\N{U+2E21}.*$/$1/; $_ } @w;
#	my $r10 = shift @r1;
#	my $r20 = shift @r2;
#	print join("\t",$r10, 'a', @r1), "\n";
#	print join("\t",$r20, 'b');
#	print join("\t",'',@r2) unless $r2[0] =~ /^[\&\$\@]/;
#	print "\n";
#   }
}

1;
