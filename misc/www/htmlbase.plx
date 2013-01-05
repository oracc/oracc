#!/usr/bin/perl
use warnings; use strict;
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;
use ORACC::NS;
use Getopt::Long;

my @inputs = ();
my $project = '';
my $verbose = 0;
my $zeroweb = 0;

GetOptions(
    '00web'=>\$zeroweb,
    'project:s'=>\$project,
    'verbose'=>\$verbose);

if ($zeroweb) {
    @inputs = `find 00web -name '*.html'`;
    chomp @inputs;
} else {
    @inputs = @ARGV;
}

foreach my $f (@inputs) {
    next if $f =~ /\#/;
    next if $f =~ /_vt/;
    my $x = load_xml($f);
    if ($x) {
	my $head = undef;
	foreach my $c ($x->getDocumentElement()->childNodes()) {
	    $head = $c and last 
		if $c->isa('XML::LibXML::Element')
		&& $c->nodeName() eq 'head';
	}
	if ($head) {
	    my $uri = $head->namespaceURI();
	    if (!$uri || $uri eq $XHTML) {
		my $base = undef;
		foreach my $c ($head->childNodes()) {
		    $base = $c and last 
			if $c->isa('XML::LibXML::Element')
			&& $c->nodeName() eq 'base';
		}
		unless ($base) {
		    my $fdir = $f;
		    $fdir =~ s#(?:\./|00web/)##;
		    if ($fdir =~ m#/#) {
			$fdir =~ s#/([^/]+)$#/#;
		    } else {
			$fdir = undef;
		    }
		    my $basedir = 'http://'.$ENV{'ORACC_HOST'}.'/';
		    $basedir .= "$project/" if $project;
		    $basedir .= "$fdir" if $fdir;
		    $base = XML::LibXML::Element->new('base');
		    $base->setNamespace($XHTML) if $uri;
		    $base->setAttribute('href',$basedir);
		    $head->appendChild($base);
		    system 'chmod', '+w', $f;
		    open(X,">$f");
		    print X $x->toString(1);
		    close(X);
		    unless ($zeroweb) {
			system 'chmod', '-w', $f;
		    }
		}
	    } else {
		warn "htmlbase.plx: namespace $uri not handled: skipping\n"
		    if $verbose;
	    }
	} else {
	    warn "htmlbase.plx: $f is not valid HTML: skipping\n"
		if $verbose;
	}
    } else {
	warn "htmlbase.plx: $f non-existent or not well-formed: skipping\n"
	    if $verbose;
    }
}

1;
