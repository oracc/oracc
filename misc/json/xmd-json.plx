#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;
use ORACC::JSON;

my %attmap = ();
my %howto = ();
xmd_howtos();

my $output = '';

my $xmd = shift @ARGV;
$output = shift @ARGV;

die "xmd-json.plx: must give catalogue on commandline\n"
    unless $xmd;

die "xmd-json.plx: no such file $xmd\n"
    unless -r $xmd;

my @in = `cat $xmd | $ENV{'ORACC'}/bin/xns`;
my $xmd_nons = join('', @in);
my $xxmd = load_xml_string($xmd_nons);

if ($xxmd) {
    print "{\n";
    ORACC::JSON::iterate($xxmd->getDocumentElement());

    my $xmd_sum = '01bld/cat-sum.html';
    if (-r $xmd_sum) {
	warn "xmd-json.plx: adding summaries from $xmd_sum\n";
	print "\n,\n";
	ORACC::JSON::reset();
	@in = `cat $xmd_sum | $ENV{'ORACC'}/bin/xns`;
	my $sum_nons = join('', @in);
	my $xsum = load_xml_string($sum_nons);
	ORACC::JSON::iterate($xsum->getDocumentElement());
    }
#    print ",";
#    xmd_fields_used($xxmd->getDocumentElement());
#    print ",";
#    xmd_sortvals($xxmd->getDocumentElement()->getAttribute('project'));
    print "\n}";
}

###############################################################################

sub htmlstring {
    my $n = shift;
    my $tmp = $n->toString();
    $tmp =~ s/xhtml_//g;
    $tmp;
}

sub
xmd_fields_used {
    my $doc = shift;
    my %fields = ();
    foreach my $xmd ($doc->getElementsByTagName('xmd_xmd')) {
	foreach my $c ($xmd->firstChild()->childNodes()) {
	    ++$fields{$c->nodeName()} if $c->hasChildNodes();
	}
    }
    my $flds = 0;
    print "\"fields\": [";
    foreach my $f (sort keys %fields) {
	print "," if $flds++;
	$f =~ s/^xmd_//;
	print "\"$f\"";
    }
    print "]";
}

sub
xmd_sortvals {
    my $proj = shift;
    system "$ENV{'ORACC'}/bin/xmd-sv-json.plx $ENV{'ORACC_BUILDS'}/$proj/01bld/sortvals.xml";
}

sub
xmd_howtos {
    $howto{'xmd-set'} = { type=>"",nam=>'type',val=>'catalogue',att=>'-file langs',chld=>['members','{'],
			  hook=>\&ORACC::JSON::default_metadata
    };
    $howto{'xmd_xmd'} = { type=>"",nam=>'@xml:id',val=>'{',att=>'-id' };
    $howto{'xmd_cat'} = { type=>"#ignore" };
    $howto{'xmd_subfield'} = { nam=>'#ignore', val=>'text()' };
    $howto{'xmd_tr'} = { nam=>'#ignore', val=>'text()' };
    $howto{'#auto'} = 1;
    $howto{'#skipempty'} = 1;
    
    $attmap{'c'} = '#ignore';
#    $attmap{'project'} = '#ignore';

    $howto{'xhtml_html'} = { type=>'#ignore' };
    $howto{'xhtml_head'} = { type=>'#ignore', recurse=>'no' };
    $howto{'xhtml_body'} = { nam=>'summaries', val=>'{' };
    $howto{'xhtml_p'} = { nam=>'@id', val=>'hook()', hook=>\&htmlstring, recurse=>'no' };
    
    ORACC::JSON::setHowTos(%howto);
    ORACC::JSON::setAttMap(%attmap);
}

1;
