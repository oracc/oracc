package ORACC::BIB::Indexer;
use lib '@@ORACC@@/lib';
use strict; use warnings; use open ':utf8';
use Encode;
use GDBM_File;
use XML::LibXML;
use XML::LibXSLT;
use ORACC::XML;
use ORACC::BIB::Config;
use ORACC::BIB::KeyMangler;

my $curr_id = undef;
my $prefix = '';

#my $record_gi = 'biblStruct';
#my $id_attr = 'id';

my $charbuf = '';
my %keys = ();
my %tei_db = ();
my %htm_db = ();

sub
index {
    my ($dbdir,$dbname,@files) = @_;

    ORACC::BIB::Config::init();

    @_ = ();

    my $ok = 1;

    $prefix .= '/' if length $prefix;
    
    $dbdir .= '/' if length $dbdir;

    tie(%tei_db, 'GDBM_File', "$dbdir$dbname-tei.db",
	&GDBM_NEWDB, 0640) 
	or die;
    
    tie(%htm_db, 'GDBM_File', "$dbdir$dbname-htm.db",
	&GDBM_NEWDB, 0640) 
	or die;
    
    my $parser = XML::LibXML->new();
    my $xslt = XML::LibXSLT->new();
    my $htm_doc = $parser->parse_file($ORACC::BIB::env->get('dbhtml.xsl'));
    my $htm_sheet = $xslt->parse_stylesheet($htm_doc);

    foreach my $file (@files) {
	my $tei_dom = $parser->parse_file($file);

	# cache and index TEI version
	foreach my $n ($tei_dom->getDocumentElement()->childNodes()) {
	    next unless $n->isa('XML::LibXML::Element');
	    $curr_id = xid($n);
	    warn("processing $curr_id\n");
	    my $xn = $n->toString();
	    Encode::_utf8_off($xn);
	    $tei_db{ $curr_id } = $xn;
	    foreach my $t ($n->findnodes('.//text()')) {
		index_keys($t->data());
	    }
	}

	# generate HTML version
	my $htm_dom = $htm_sheet->transform($tei_dom);

#	open(H,">htm.dom"); print H $htm_dom->toString(); close(H);

	# cache HTML version
	foreach my $n ($htm_dom->getDocumentElement()->findnodes('t:biblStruct')) {
	    my $id = xid($n);
	    my $p = $n->firstChild();
	    my $xp = $p->toString();
	    $xp =~ s# xmlns(?::h)?=\"http://www.w3.org/1999/xhtml\"##g;
	    Encode::_utf8_off($xp);
	    $htm_db{ $id } = $xp;
	}
    }

    untie(%tei_db);
    untie(%htm_db);

    my %keys_db;
    tie(%keys_db, 'GDBM_File', "$dbdir$dbname.db",
	&GDBM_NEWDB, 0640) 
	or die;
    foreach (keys(%keys)) {
	my $k = $_;
	Encode::_utf8_off($k);
	$keys_db{$k} = join(',', @{$keys{$_}});
    }
    untie(%keys_db);
    return $ok;
}

sub
index_keys {
    my $keys = shift;
    ORACC::BIB::KeyMangler::canonicalize($keys);
    foreach my $key (split(/\s+/, $keys)) {
	next unless length $key;
	push(@{$keys{$key}}, "$prefix$curr_id") if length($key);
    }
}

1;
