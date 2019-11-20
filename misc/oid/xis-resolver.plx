#!/usr/bin/perl
use warnings; use strict;

my ($proj,$lang,$xis,$page,$size) = @ARGV;

my $projdir = "$ENV{'ORACC_BUILDS'}/www/$proj";

$page = 0 unless $page;
$size = 0 unless $size;

xml_error("bad page specification") unless $page==0 || $page =~ /^\d+/;
xml_error("bad size specification") unless $size==0 || $size =~ /^(25|50|100)$/;

if (-d $projdir) {
    my $langdir = "$projdir/cbd/$lang";
    if (-d $langdir) {
	my $xisdir = "$langdir/xis";
	mkdir $xisdir unless -d $xisdir;
	if (-d $xisdir) {
	    my $xisbucket = $xis; $xisbucket = s/^(.....).*$/$1/;
	    my $bucket = "$xisdir/$xisbucket";
	    mkdir $bucket unless -d $bucket;
	    if (-d $bucket) {
		my $xisdata = "$bucket/$xis";
		mkdir $xisdata unless -d $xisdir;
		if (-d $xisdata) {
		    my $count = xis_init($xis_data);
		    if ($page) {
			my $npages = $count % $page;
			++$npages if $count % $npages;
			$page = $npages unless $page < $npages;
			xis_base();
		    } else {
			xml_count($count);
		    }
		} else {
		    xml_error("failed to find or create xis data");
		}
	    } else {
		xml_error("failed to find or create xis bucket");
	    }
	} else {
	    xml_error("failed to find or create xis directory");
	}
    } else {
	xml_error("bad language in request");
    } 
} else {
    xml_error("bad project in request");
}

###############################################################################

sub xis_base {
    # use p3-slicer code to extract page from xis list
    # use p3 context engine code to create xml version of page
    # return xml to client
}

sub xis_init {
    # use p3-pager code to extract xis list from tis/xis
}

sub xml_error {
    my $err = shift;
    xml_header();
    print '<error>',$err,'</error>',"\n";
    exit 0;
}

sub xml_header {
    print "Content-type: text/xml; charset=utf-8\n\n";
}

1;
