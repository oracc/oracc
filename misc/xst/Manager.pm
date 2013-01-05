package ORACC::XST::Manager;

use lib '@@ORACC@@/lib';
use lib '@@ORACC@@/lib/epsd';

use warnings; use strict; use open ':utf8'; use utf8;
use PSD::SUM::Parser::Phrasal;
use PSD::SUM::Parser::XCES;
use ORACC::XML;
use ORACC::NS;
use ORACC::Expand;

# This module assumes that it is being run with CWD = text-id
my $xpf_only = 0;

my $xmlparser;
my $xsltparser;
my %keys = ();
my $xtfdoc = '';
my $xml_uri = 'http://www.w3.org/XML/1998/namespace';
my $xhtml_uri = 'http://www.w3.org/1999/xhtml';
my $xtf_uri = 'http://oracc.org/ns/xtf/1.0';
my $xsb_uri = 'http://oracc.org/ns/xsb/1.0';
#my $text_id = '';
#my $text_n = '';
my %labels = ();
my $last_label = '';
my $unit_id = 0;
my $units_labeled = 0;
my $keep_xsb = 1;

my @xst_phrasal = ();

sub
run {
    my($id,@refs) = @_;
    # run the phrasal parser on each reference
    @xst_phrasal = ();
    foreach my $r (@refs) {
	my($id,$label,@r) = @$r;
	if (ref_comment($r[0])) {
	    push @xst_phrasal, [ 'comment', $r ];
	} else {
	    my $sentence = PSD::SUM::Parser::Phrasal::run(@r);
	    push (@xst_phrasal, [ $id,
				  $label,
				  PSD::SUM::Parser::XCES::xces_print_phrases
				  ($id,$sentence) ]);
	}
    }
    @xst_phrasal;
}

sub
ref_comment {
    my $pw = shift;
#    my $pw = $$r[0];
    $$pw{'break'};
}

sub
dump_xst {
    my($fname,$id,$text_n,@xst) = @_;
#    print STDERR "!!!\n";
#    $unit_id = 0;
    $text_n =~ s/\s*$//;
    open(OUT,'>:utf8', "$fname") || die "can't write to '$fname'\n";
    print OUT "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    print OUT "<units xml:id=\"$id\" n=\"$text_n\" xmlns=\"http://oracc.org/ns/syntax-tree/1.0\"\n>";
#    use Data::Dumper;
    foreach my $x (@xst) {
#	print Dumper $x;
	my($uid,$label,$tree) = ($$x[0],$$x[1],$$x[2]);
	my $unit_id = $uid;
	$unit_id =~ s/^.*?\.U//;
	if ($label eq 'comment') {
	    print OUT "<comment>text broken</comment>";
	} else {
	    print OUT "<unit n=\"$text_n, $label\" u=\"$unit_id\" ref=\"$uid\"\n>";
	    print OUT $tree;
	    print OUT "</unit>";
	    ++$unit_id;
	}
    }
    print OUT "</units>";
    close(OUT);
}

1;
