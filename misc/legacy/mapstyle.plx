#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

# normal -> norm-lang (default s)
# italic -> ital-lang (default a)
# smallcaps -> uppercase if $smallcaps = 1
# uppercase -> $-marked if $smallcaps = 1

# other fixes?

use lib "$ENV{'ORACC'}/lib";
use ORACC::Legacy::ODF;
use ORACC::Legacy::StyleInfo;
use ORACC::XML;
use ORACC::NS;
use Encode;
use Getopt::Long;

my $Lgrapheme = '([a-zšḫŋṣṭśáéíúàèìù]+)(?=$|[^\p{L}])';
my $Ugrapheme = '([A-ZŠḪŊṢṬŚÁÉÍÚÀÈÌÙ]+)(?=$|[^\p{L}])';

my $content = undef;
my $csc_mode = 0;
my $inplace = 0;
my $keepfiles = 0;
my $outfile = '';
my $trace = 0;

GetOptions(
    'csc'=>\$csc_mode,
    'inplace+'=>\$inplace,
    'keep'=>\$keepfiles,
    'outfile:s'=>\$outfile,
    'trace'=>\$trace,
    );

die "mapstyle.plx: please remove 'content.xml' or work in another directory\n"
    if -e 'content.xml';
my $infile = shift @ARGV || '';
die "mapstyle.plx: can't read XML input `$infile'\n" 
    unless -r $infile;
ORACC::Legacy::StyleInfo::loadinfo($infile);
$content = ORACC::Legacy::ODF::load_content($infile) 
    || die "mapstyle.plx: failed to load XML input $infile\n";
iterate($content, '', '', '');
if ($outfile) {
    open(X,">$outfile") 
	|| die "mapstyle.plx: can't write output file `$outfile'\n";
    binmode X, ':raw';
    print X $content->toString();
    close(X);
} else {
    unless ($inplace) {
	my $infile_new = "$infile.new";
	system 'cp', '-p', $infile, $infile_new;
	$infile = $infile_new;
    }
    ORACC::Legacy::ODF::update_content($infile,$content);
    unlink 'content.xml' unless $keepfiles;
}

1;

#########################################################################

sub
iterate {
    my ($node,$style,$variant,$super) = @_;
    my $styleattr = undef;
    if ($node->isa('XML::LibXML::Element')) {
	$styleattr = $node->getAttributeNS($ODFTEXT,'style-name')
	    || $node->getAttributeNS($ODFTABLE,'style-name');
    }

    if ($styleattr) {
	my $nstyle = ORACC::Legacy::StyleInfo::info4style($styleattr,'style');
	my $nvariant = ORACC::Legacy::StyleInfo::info4style($styleattr,'variant');
	my $nsuper = ORACC::Legacy::StyleInfo::info4style($styleattr,'super');
	$style = $nstyle if $nstyle;
	$variant = $nvariant if $nvariant;
	$super = $nsuper if $nsuper;
    }

    foreach my $child ($node->childNodes()) {
	if ($child->isa('XML::LibXML::Element')) {
	    iterate($child,$style,$variant,$super);
	} elsif ($child->isa('XML::LibXML::Text')) {
	    my $text = $child->getData();
	    Encode::_utf8_on($text);
	    my $newtext = mapstyle($text,$style,$variant,$super);
	    Encode::_utf8_on($newtext);
	    $child->setData($newtext) if $newtext;
	}
    }
}

sub
mapstyle {
    my($t,$sty,$var,$sup) = @_;
    my $orig = $t;
    if ($csc_mode) {
	$t =~ s/($Ugrapheme)/\$$1/g;
	if ($var =~ /small-caps/) {
	    $t =~ s/($Lgrapheme)/\U$1/g
		unless $sup;
	    $t =~ s/\{(.*?)\}/{\L$1}/g;
	}
    }
    if ($sup) {
	$t = "{$t}";
    }
    warn "Mapstyle: s=$sty/v=$var/^=$sup; `$orig'=>`$t'\n"
	if $trace;
    $t;
}
