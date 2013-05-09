package ORACC::P2::Util2;
use utf8; use open 'utf8'; binmode(STDOUT, ':utf8');
use lib '@@ORACC@@/lib';
use ORACC::XML;
use File::Temp qw/tempdir tempfile/;
use Encode;

local $XML::LibXML::skipXMLDeclaration = 1;
my $xforms_ns = 'http://www.w3.org/2002/xforms';
my $xforms_pi = '<?xml-stylesheet href="/xsltforms/xsltforms.xsl" type="text/xsl"?>';

sub
create_list {
    my $dir = shift;
    my $tmpdir = tempdir(DIR=>$dir);
    my $list = $tmpdir;
    $list =~ s#^$dir##;
    $list =~ s#^/##;
    ($list,$tmpdir);
}

sub
create_session {
    my $dir = (-w '/var/tmp/oracc/pager' 
	       ? '/var/tmp/oracc/pager'
	       : '.');
    my $tmpdir = tempdir('P2_XXXXXX',DIR=>$dir);
    my $session = $tmpdir;
    $session =~ s#^$dir## if $session =~ m#^/#;
    $session =~ s#^/##;
    ($session,$tmpdir);
}

sub
decode_args_or_cgi {
    my $xdata = undef;
    if ($ARGV[0] && -r $ARGV[0]) {
	$xdata = load_xml($ARGV[0]);
    }
    $xdata;
}

sub
emit_instance {
    my($tmpdir,$xdata,$type,$serialize,$launch,$project,$session,$nohttp) = @_;

    my $gnode = get_node($xdata, 'glossary');
    if ($gnode) {
	unless ($gnode->hasChildNodes()) {
	    $gnode->appendText('#none');
	}
    }

    open(RET,">$tmpdir/returned-$type.xml") 
	|| die "P2::Util: can't write `$tmpdir/returned-$type'\n";
    binmode RET, ':raw';
    print RET $xdata->getDocumentElement()->toString(1);
    close(RET);

    warn "P2:Util2: wrote $tmpdir/returned-$type.xml\n";

    if ($serialize) {
	binmode STDOUT, ':raw';
	print "Content-type: text/xml; charset=utf-8\n\n"
	    unless $nohttp;
	print $xdata->getDocumentElement()->toString(0);
	close STDOUT;
    } elsif ($launch) {
	my $p2 = load_p2_with_instances($tmpdir, $project);
	print "Content-type: text/xml; charset=utf-8\n\n"
	    unless $nohttp;
	print $xforms_pi, "\n";
	my $p2str = $p2->getDocumentElement()->toString(0);
	Encode::_utf8_on($p2str);
	$p2str =~ s/\@\@SESSION\@\@/$session/g;
	print $p2str;
	close STDOUT;
    }
    exit 0;
}

sub
get_node {
    my($x,$t) = @_;
    my @v = tags($x, undef, $t);
    if ($#v >= 0) {
	$v[0];
    } else {
	undef;
    }
}

sub
load_default_instance {
    load_xml("@@ORACC@@/lib/data/default-$_[0].xml");
}

sub
load_p2_with_instances {
    my ($tmpdir,$project) = @_;
    my $p2 = load_xml("@@ORACC@@/xml/$project/p2.xml")
	|| die "P2::Util: failed to load $project/p2.xml\n";
    my @instances = tags($p2,$xforms_ns,'instance');
    foreach my $i (@instances) {
	my $id = $i->getAttribute('id');
	if ($id eq 'pager' || $id eq 'browse' || $id eq 'search') {
	    my $x = load_xml("$tmpdir/returned-$id.xml");
#		|| die "P2::Util: can't load `$tmpdir/returned-$id.xml'\n";
	    if ($x) {
		$i->removeChildNodes();
		$i->appendChild($x->getDocumentElement());
	    }
	}
    }
    $p2;
}

sub
set_data {
    my($xdata,%newdata) = @_;
    my($tag,$val) = ();
    while (($tag,$val) = each %newdata) {
	if ($tag) {
	    my $node = get_node($xdata,$tag);
	    if ($node) {
		$node->removeChildNodes();
		$node->appendTextNode($val||'');
	    }
	} else {
	    warn "set_data failed on `$tag'\n";
	}
    }
#    warn $xdata->toString(1), "\n";
}

sub
save_instance {
    my ($tmpdir,$xdata) = @_;
    my $basename = $xdata->getDocumentElement()->nodeName();
    $basename =~ s/-.*$//;
    open(D,">$tmpdir/$basename.xml");
    binmode(D,':raw');
    print D $xdata->toString(1);
    close(D);
}

sub
setup_tmpdir {
    my($xdata,$values) = @_;
    my($session,$list,$tmpdir) = ();
    if ($xdata || $$values{'session'}) {
	($session,$list) = ($$values{'session'},$$values{'list'});
	if ($session) {
	    $tmpdir = "/var/tmp/oracc/pager/$session";
	} else {
	    ($session,$tmpdir) = create_session();
	}
	if ($list) {
	    $tmpdir .= "/$list";
	} else {
	    ($list,$tmpdir) = create_list($tmpdir);
	}
    } else {
	($session,$tmpdir) = create_session();
	unless ($values{'list'} 
		|| ($values{'glossary'} && $values{'glossary'} ne '#none')) {
	    ($list,$tmpdir) = create_list($tmpdir);
	} else {
	    $list = '';
	}
    }
    $tmpdir = "/var/tmp/oracc/pager/$session/$list"
	unless $tmpdir;
    ($session,$list,$tmpdir);
}

sub
set_values {
    my $xdata = shift;
    my %v = ();
    foreach my $c ($xdata->getDocumentElement()->childNodes()) {
	next unless $c->isa('XML::LibXML::Element');
	$v{$c->localName()} = ($c->textContent() || '');
    }
    (%v);
}

1;
