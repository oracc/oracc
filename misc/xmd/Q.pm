package ORACC::XMD::Q;
use warnings; use strict; use open ':utf8';
use lib '@@ORACC@@/lib';
use ORACC::Expand2;
use ORACC::XML;
use ORACC::NS;

binmode STDERR, ':utf8';
binmode STDOUT, ':utf8';

my $q;
my $qfile = '@@ORACC@@/catalog/Q/sources/Q.xmd';
my $qid = '';
my %indexes = ();
my @records = ();
my %records = ();
my %q_by_name = ();

sub
createQrec {
    my(%fields) = @_;
    return unless $fields{'id_composite'} && !exists $records{$fields{'id_composite'}};
    my $c = $records[0]->cloneNode(1);
    foreach my $cc ($c->childNodes()) {
	$cc->removeChildNodes() if $cc->isa('XML::LibXML::Element');
    }
    foreach my $f (keys %fields) {
	warn("Q.pm: $f: undefined\n") and next unless defined $fields{$f};
	my $n = (tags($c,$XMD,$f))[0];
	if ($n) {
	    $n->appendTextNode($fields{$f});
	} else {
	    warn("Q.pm: $f: unknown field in createQrec\n");
	}
    }
    $q->getDocumentElement()->appendChild($c);
}

sub
dump {
    open(N,">$qfile.new");
    print N $q->toString();
    close(N);
    warn("Q.pm: new Q data written to $qfile.new\n");
}

sub
dump_tabbed {
    my @rows = ();
    my %fieldnames = ();
    my %fieldcodes = ();
    my $code = 0;

    foreach my $r ($q->getDocumentElement()->childNodes()) {
	foreach my $f ($r->childNodes()) {
	    if ($f->isa('XML::LibXML::Element')) {
		++$fieldnames{$f->localName()};
	    }
	}
    }
    my @fieldnames = sort keys %fieldnames;
    print join("\t", @fieldnames), "\n";
    foreach my $f (@fieldnames) {
	$fieldcodes{$f} = $code++;
    }
    foreach my $r ($q->getDocumentElement()->childNodes()) {
	next unless $r->isa('XML::LibXML::Element');
	my @fields = ();
	foreach my $f ($r->childNodes()) {
	    if ($f->isa('XML::LibXML::Element')) {
		$fields[$fieldcodes{$f->localName()}] = $f->textContent();
	    }
	}
	for (my $i = 0; $i <= $#fieldnames; ++$i) {
	    print $fields[$i] if $fields[$i];
	    if ($i < $#fieldnames) {
		print "\t";
	    } else {
		print "\n";
	    }
	}
    }
}

sub
getByField {
    my($field,$value) = @_;
    if (!defined $indexes{$field}) {
	foreach my $r (@records) {
	    my $fval;
	    my $fnode = (tags($r,$XMD,$field))[0];
	    $fval = $fnode->textContent() if $fnode;
	    $fval = '' unless $fval;
	    push @{${$indexes{$field}}{$fval}},$r;
	}
    }
    if (${$indexes{$field}}{$value}) {
	@{${$indexes{$field}}{$value}};
    } else {
	();
    }
}

sub
getValue {
    my ($rnode,$field) = @_;
    my $fnode = (tags($rnode,$XMD,$field))[0];
    if ($fnode) {
	return $fnode->textContent();
    } else {
	'';
    }
}

sub
load {
#    warn "loading Q\n";
    $q = load_xml($qfile);
    @records = tags($q,$XMD,'record');
    foreach my $r (@records) {
	$qid = (tags($r, ###$records[$#records],
		     $XMD,
		     'id_composite'))[0]->textContent();
	my $name = (tags($r,$XMD,'designation'))[0]->textContent();
#	warn("$name = $qid\n");
	$records{$qid} = $r;
	$q_by_name{$name} = $qid;
    }
}

sub
nextqid {
    $qid = (tags($records[$#records],
		 $XMD,
		 'id_composite'))[0]->textContent()
	unless $qid;
    $qid++;
}

sub
id_from_name {
    $q_by_name{$_[0]};
}

1;
