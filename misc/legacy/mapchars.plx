#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

use lib "$ENV{'ORACC'}/lib";
use ORACC::Legacy::Charmap;
use ORACC::Legacy::ODF;
use ORACC::Legacy::StyleInfo;
use ORACC::XML;
use ORACC::NS;
use Encode;
use Getopt::Long;

my $content = undef;
my $textfont = '';
my $inplace = 0;
my $keepfiles = 0;
my $newchars = 0;
my $outfile = '';
my $trace = 0;
my $textinput = 0;

GetOptions(
    'font:s'=>\$textfont,
    'inplace+'=>\$inplace,
    'keep'=>\$keepfiles,
    'newchars'=>\$newchars,
    'outfile:s'=>\$outfile,
    'textinput'=>\$textinput,
    'trace'=>\$trace,
    );

ORACC::Legacy::Charmap::set_trace($trace);

if ($textinput) {
    die "mapchars.plx: must use '-textfont' with '-textinput'\n"
	unless $textfont;
    ORACC::Legacy::Charmap::set_odf('<stdin>');    
    while (<>) {
	s/^﻿//; # remove BOM
	print ORACC::Legacy::Charmap::map_char($_,$textfont,0);
    }
} else {
    die "mapchars.plx: please remove 'content.xml' or work in another directory\n"
	if -e 'content.xml';
    my $infile = shift @ARGV || '';
    die "mapchars.plx: can't read XML input `$infile'\n" 
	unless -r $infile;
    ORACC::Legacy::StyleInfo::loadinfo($infile);
    $content = ORACC::Legacy::ODF::load_content($infile) 
	|| die "mapchars.plx: failed to load XML input $infile\n";
    ORACC::Legacy::Charmap::set_odf($infile);
    iterate($content, '');
    if ($outfile) {
	open(X,">$outfile") 
	    || die "mapchars.plx: can't write output file `$outfile'\n";
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
}

1;

#########################################################################

sub
iterate {
    my ($node,$font) = @_;
    my $styleattr = undef;
    my $stylefont = undef;
    my $sub = undef;
    if ($node->isa('XML::LibXML::Element')) {
	$styleattr = $node->getAttributeNS($ODFTEXT,'style-name')
	    || $node->getAttributeNS($ODFTABLE,'style-name');
    }
    $stylefont = ORACC::Legacy::StyleInfo::info4style($styleattr,'font')
	if $styleattr;
    $font = $stylefont if $stylefont;
    $sub = ORACC::Legacy::StyleInfo::info4style($styleattr,'sub')
	if $styleattr;
    foreach my $child ($node->childNodes()) {
	if ($child->isa('XML::LibXML::Element')) {
	    iterate($child,$font);
	} elsif ($child->isa('XML::LibXML::Text')) {
	    my $text = $child->getData();
	    my $newtext = $text;
	    Encode::_utf8_on($text);
	    $newtext = ORACC::Legacy::Charmap::map_char($text,$font,$sub);
	    Encode::_utf8_on($newtext);
	    $child->setData($newtext) if $newtext;
	}
    }
}
