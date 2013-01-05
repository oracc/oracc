package ORACC::NS;
require Exporter;
@ISA=qw/Exporter/;

$ORACC::NS::CBD   = 'http://oracc.org/ns/cbd/1.0';
$ORACC::NS::CBH   = 'http://oracc.org/ns/cbh/1.0';
$ORACC::NS::DC    = 'http://purl.org/dc/elements/1.1';
$ORACC::NS::DC03  = 'http://dublincore.org/documents/2003/06/02/dces/';
$ORACC::NS::DCTERMS= 'http://purl.org/dc/terms';
$ORACC::NS::ETCSL = 'http://www-etcsl.orient.ox.ac.uk/etcsl';
$ORACC::NS::GDL   = 'http://oracc.org/ns/gdl/1.0';
$ORACC::NS::G2SLICE  = 'http://oracc.org/ns/g2-slice/1.0';
$ORACC::NS::LEM   = 'http://oracc.org/ns/lemma/1.0';
$ORACC::NS::NGM   = 'http://oracc.org/ns/ngram/1.0';
$ORACC::NS::SKL   = 'http://oracc.org/ns/skl/1.0';
$ORACC::NS::STT   = 'http://oracc.org/ns/stats/1.0';
$ORACC::NS::TEI   = 'http://www.tei-c.org/ns/1.0';
$ORACC::NS::XCL   = 'http://oracc.org/ns/xcl/1.0';
$ORACC::NS::XDF   = 'http://oracc.org/ns/xdf/1.0';
$ORACC::NS::XFF   = 'http://oracc.org/ns/xff/1.0';
$ORACC::NS::XHTML = 'http://www.w3.org/1999/xhtml';
$ORACC::NS::XI    = 'http://www.w3.org/2001/XInclude';
$ORACC::NS::XIM   = 'http://oracc.org/ns/xim/1.0';
$ORACC::NS::XIX   = 'http://oracc.org/ns/xix/1.0';
$ORACC::NS::XLF   = 'http://oracc.org/ns/lemma/1.0';
$ORACC::NS::XLINK = 'http://www.w3.org/1999/xlink';
$ORACC::NS::XCF   = 'http://oracc.org/ns/xcf/1.0';
$ORACC::NS::XMD   = 'http://oracc.org/ns/xmd/1.0';
$ORACC::NS::XML   = 'http://www.w3.org/XML/1998/namespace';
$ORACC::NS::XOC   = 'http://oracc.org/ns/ontology/1.0';
$ORACC::NS::XPD   = 'http://oracc.org/ns/xpd/1.0';
$ORACC::NS::XRU   = 'http://oracc.org/ns/xru/1.0';
$ORACC::NS::XST   = 'http://oracc.org/ns/syntax-tree/1.0';
$ORACC::NS::XTC   = 'http://oracc.org/ns/xtc/1.0';
$ORACC::NS::XTF   = 'http://oracc.org/ns/xtf/1.0';
$ORACC::NS::XTF2  = 'http://oracc.org/ns/xtf/1.0';
$ORACC::NS::XVL   = 'http://oracc.org/verblist';

$ORACC::NS::ODFOFFICE = 'urn:oasis:names:tc:opendocument:xmlns:office:1.0';
$ORACC::NS::ODFSTYLE = 'urn:oasis:names:tc:opendocument:xmlns:style:1.0';
$ORACC::NS::ODFTABLE = 'urn:oasis:names:tc:opendocument:xmlns:table:1.0';
$ORACC::NS::ODFTEXT =  'urn:oasis:names:tc:opendocument:xmlns:text:1.0';

@EXPORT = qw/$CBD $CBH $DC $DCO3 $DCTERMS $ETCSL $GDL 
	     $G2SLICE
	     $LEM $NGM $SKL $STT $TEI
             $XCL $XTF $XTF2 $XFF $XLF $XST $XIM $XIX $XVL 
	     $XOC $XMD $XML $XHTML $XPD
             $XLINK $XDF $XRU $XCF $XTC $XI
             $ODFSTYLE $ODFTABLE $ODFTEXT/;

1;
