package ORACC::Legacy::ODF;
use warnings; use strict; use open 'utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;

sub
load_content {
    my $odf = shift;
    my @xml = ();
    open(O,"unzip -p $odf content.xml|");
    @xml = (<O>);
    close(O);
    load_xml_string(join('',@xml));    
}

sub
update_content {
    my ($odf,$xml) = @_;
    open(X,">content.xml") 
	|| die "mapchars.plx: can't write content to file 'content.xml'\n";
    binmode X, ':raw';
    print X $xml->toString();
    close(X);
    system 'zip', '-f', $odf, "content.xml";
#    unlink 'content.xml';
}

1;
