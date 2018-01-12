#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;
use ORACC::JSON;

my $projcbd = shift @ARGV;
my ($project,$lang) = split(/:/, $projcbd);

my $cbd_ns = "$ENV{'ORACC'}/bld/$project/$lang/articles-with-periods.xml";

die "cbd-geojson.plx: can't read input $cbd_ns\n"
    unless -r $cbd_ns;

my $cbd_geo_xsl = "$ENV{'ORACC_BUILDS'}/lib/scripts/cbd-geojson.xsl";
my @in = `xsltproc $cbd_geo_xsl $cbd_ns | $ENV{'ORACC'}/bin/xns`;
my $cbd_nons = join('', @in);
my $xcbd = load_xml_string($cbd_nons);

if ($xcbd) {
    cbd_geo_howtos();
    print "{\n\"type\": \"FeatureCollection\",\n\"features\": [\n";
    ORACC::JSON::iterate($xcbd->getDocumentElement());
    print "\n]}";
}

sub unquote {
    my $n = shift;
    "\"coordinates\": ".$n->textContent();
}

###############################################################################

sub
cbd_geo_howtos {
    my %howto = ();
    
    $howto{'feature'} = { type=>'{', nam=>'type', val=>'feature' };
    $howto{'properties'} = { nam=>'properties', val=>'{' };
    $howto{'project'} = { nam=>'project', val=>'text()' };
    $howto{'lang'} = { nam=>'lang', val=>'text()' };
    $howto{'headword'} = { nam=>'headword', val=>'text()' };
    $howto{'id'} = { nam=>'id', val=>'text()' };
    $howto{'instances'} = { nam=>'instances', val=>'text()' };
    $howto{'geometry'} = { nam=>'geometry', val=>'{' };
    $howto{'type'} = { nam=>'type', val=>'text()' };
    $howto{'coordinates'} = { nam=>'coordinates', val=>'hook', hook=>\&unquote };
    
    $howto{'#sparse'} = 1;
    $howto{'#geojson'} = 1;
    
    ORACC::JSON::setHowTos(%howto);
}

1;
