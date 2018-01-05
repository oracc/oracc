#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;
use ORACC::JSON;

my $project = `oraccopt`;
unless ($project) {
    my $pwd = `pwd`;
    die "esp-json.plx: unable to set project in directory $pwd\n";
}

my $esp_source = '01tmp/source-tree-40.xml';
die "esp-json.plx: no such file $esp_source\n"
    unless -r $esp_source;

my $esp_transform = "$ENV{'ORACC_BUILDS'}/lib/scripts/esp2-for-json.xsl";

my %howto = ();
esp_howtos();

my $esp = `xsltproc --stringparam project $project $esp_transform $esp_source`;
my $xesp = load_xml_string($esp);

if ($xesp) {
    print "{\n";
    ORACC::JSON::iterate($xesp->getDocumentElement());
    print "\n}";
}

###############################################################################

sub
esp_howtos {
    $howto{'text4json'} = { type=>"",nam=>'type',val=>'portal',chld=>['chunks','['],
			  hook=>\&ORACC::JSON::default_metadata
    };
    $howto{'chunk'} = { nam=>'#ignore', val=>'{' };
    $howto{'type'} = { nam=>'type', val=>'text()' };
    $howto{'url'} = { nam=>'url',   val=>'text()' };
    $howto{'text'} = { nam=>'text', val=>'text()' };
    
    ORACC::JSON::setHowTos(%howto);
}

1;
