package ORACC::XMD::Util;
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;
use ORACC::NS;

$ORACC::XMD::Util::quiet = 0;
my @fields = ();
my %fields = ();
my $file;

sub
internalize {
    $file = shift;
    my $xmd = load_xml($file); die "ORACC::XMD::Util: open failed on $file\n" unless $xmd;
    my @cat = ();
    @cat = tags($xmd,$xmd->getDocumentElement()->namespaceURI(),'record');
    initialize_fields($cat[0]);
    my $rows = xmd_records($file, @cat);
    ([@fields],{%fields},$rows);
}

sub
internalize_list {
    my @list = @_;
    my $xmd = load_xml($list[0]); die "ORACC::XMD::Util: open failed on $file\n" unless $xmd;
    my @cat = ();
    @cat = tags($xmd,$xmd->getDocumentElement()->namespaceURI(),'record');
    initialize_fields($cat[0]);
    my @rows = ();
    my $rows = xmd_records($list[0], @cat);
    push @rows, @$rows; # $rows is a scalar ref to a list containing a single scalar ref here
    for (my $i = 1; $i <= $#list; ++$i) {
	undef $xmd;
	$xmd = load_xml($list[$i]);
	@cat = tags($xmd,$xmd->getDocumentElement()->namespaceURI(),'record');
	$rows = xmd_records($list[$i], @cat);
	push @rows, @$rows;
    }
    ([@fields],{%fields},[@rows]);
}

sub
initialize_fields {
    my $i = 0;
    @fields = ();
    %fields = ();
    if ($_[0]) {
	foreach my $f ($_[0]->childNodes()) {
	    next unless $f->isa('XML::LibXML::Element');
	    my $fname = $f->localName();
	    $fields[$i] = $fname;
	    $fields{$fname} = $i++;
	}
    }
}

sub
xmd_records {
    my $file = shift;
    my @rows = ();
    my $pacifier = 0;
    print STDERR "ORACC::XMD::Util: reading records from $file " unless $ORACC::XMD::Util::quiet;
    my $nfields = $#fields;
    foreach my $r (@_) {
	my @row = ();
#	my $rec_project = $r->getAttribute('project');
	foreach my $f ($r->childNodes()) {
	    next unless $f->isa('XML::LibXML::Element');
	    my $fname = $f->localName();
	    if (!defined $fields{$fname}) {
		push @fields, $fname;
		$fields{$fname} = $#fields;
	    }
	    # guarantee the fields are in the same order in all rows
	    my $tmp = $f->textContent();
	    $tmp =~ tr/\n\r/  /;
	    $tmp =~ s/^\s*(.*?)\s*$/$1/;
	    $row[$fields{$fname}] = $tmp;
	}
	push @rows, [ @row ];
	unless ($pacifier++ % 10000) {
	    print STDERR '.' 
		unless $ORACC::XMD::Util::quiet;
	}
    }
    print STDERR " $#rows\n"
	unless $ORACC::XMD::Util::quiet;
    [@rows];
}

1;
