package ORACC::OSS2::Manager;

use lib '@@ORACC@@/lib';

use warnings; use strict; use open ':utf8'; use utf8;
use ORACC::OSS2::Node;
use ORACC::OSS2::XCL;
#use ORACC::SSA3::Driver;
use ORACC::XML;
use ORACC::NS;
use ORACC::Expand2;

use ORACC::SSA3::Parser;

use Data::Dumper;

my $xpf_only = 0;

my $xmlparser;
my $xsltparser;
my %keys = ();
my $xtfdoc = '';
my $xml_uri = 'http://www.w3.org/XML/1998/namespace';
my $xhtml_uri = 'http://www.w3.org/1999/xhtml';
my $xtf_uri = $XTF;
my $xsb_uri = 'http://oracc.org/ns/xsb/1.0';
my %labels = ();
my $last_label = '';
my $unit_id = 0;
my $units_labeled = 0;
my $keep_xsb = 1;

my $error_id = '';
my $error_label = '';

my @xst_phrasal = ();

sub
run {
    my($xcl,$id,@refs) = @_;
    # run the phrasal parser on each reference
    # refs are generated for each sentence and Node::iterate
    # takes care of running the parser depth-first
    @xst_phrasal = ();
    my $sux = ORACC::SSA3::Parser->new();
    foreach my $r (@refs) {
	my($xcl_node, $ref_node) = @$r;
	if ($xcl) {
	    my($id,$label,$refs_node) = @$ref_node;
	    my $sentence = ORACC::OSS2::Node::iterate($id,$label,$xcl_node,$refs_node,
						      $sux);
	    my $result = ORACC::OSS2::XCL::xcl_print_phrases($xcl,$id,$sentence);
	    $xcl_node->removeChildNodes();
	    unless ($xcl_node->hasAttribute('tag')) {
		$xcl_node->setAttribute('tag', $result->getAttribute('tag'));
	    }
	    foreach ($result->childNodes()) {
		$xcl_node->appendChild($_);
	    }
	}
    }
}

1;
