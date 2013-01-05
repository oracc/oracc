package ORACC::XMD::SortLabels;
use lib '@@ORACC@@/lib';
use warnings; use strict;
use ORACC::XML;
my $labels = undef;

my %normalize = (
		 time=>'T',
		 T=>'T',
		 's:T'=>'T',
		 period=>'T',
		 place=>'P',
		 provenience=>'P',
		 P=>'P',
		 's:P'=>'P',
		 genre=>'G',
		 G=>'G',
		 's:G'=>'G',
		 designation=>'N',
		 name=>'N',
		 N=>'N',
		 's:N'=>'N',
		 supergenre=>'S',
		 S=>'S',
		 's:S'=>'S'
		 );

sub
get {
    my($alpha,$num) = @_;
    $alpha = $normalize{$alpha};
    my $v = $labels->getElementsById("$alpha$num");
    if ($v) {
	$v->getAttribute('k');
    } else {
#	"BAD:$alpha$num";
	'unknown';
    }
}

sub
init {
    $labels = load_xml('@@ORACC@@/catalog/results/sortlabels.xml');
}

sub
term {
    undef $labels;
}

1;
