#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;
use ORACC::JSON;

glossary_howtos();

my $projcbd = shift @ARGV;
my ($project,$lang) = split(/:/, $projcbd);

my $cbd_ns = "$ENV{'ORACC'}/bld/$project/$lang/$lang.g2x";
my $xis_ns = "$ENV{'ORACC'}/bld/$project/$lang/$lang.xis";

print "{\n";
print "\t\"type\": \"glossary\",\n";
print "\t\"project\": \"$project\",\n";
print "\t\"lang\": \"$lang\",\n";

my @in = `cat $cbd_ns | $ENV{'ORACC'}/bin/xns`;
my $cbd_nons = join('', @in);
my $xcbd = load_xml_string($cbd_nons);
ORACC::JSON::iterate($xcbd->getDocumentElement());
$xcbd = undef;
$cbd_nons = undef;
print "\n,\n";

ORACC::JSON::reset();

@in = `cat $xis_ns | $ENV{'ORACC'}/bin/xns`;
my $xis_nons = join('',@in);
my $xxis = load_xml_string($xis_nons);
ORACC::JSON::iterate($xxis->getDocumentElement());
$xxis = undef;

print "\n}\n";



1;
