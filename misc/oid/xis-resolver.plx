#!/usr/bin/perl
use warnings; use strict;

my ($proj,$lang,$xis,$page,$size) = @ARGV;

$proj = '' unless $proj;
$lang = '' unless $lang;
$xis = '' unless $xis;
$page = 0 unless $page;
$size = 0 unless $size;

my $count = -1;
my $projdir = "/Users/stinney/orc/www/$proj";
my $xisdata = '';

xml_error("bad page specification") unless $page==0 || $page =~ /^\d+/;
xml_error("bad size specification") unless $size==0 || $size =~ /^(25|50|100)$/;

if ($proj && -d $projdir) {
    my $langdir = "$projdir/cbd/$lang";
    if ($lang && -d $langdir) {
	my $xisdir = "$langdir/xis";
	mkdir $xisdir unless -d $xisdir;
	if (-d $xisdir) {
	    my $xisbucket = $xis; $xisbucket = s/^(.....).*$/$1/;
	    my $bucket = "$xisdir/$xisbucket";
	    mkdir $bucket unless -d $bucket;
	    if (-d $bucket) {
		$xisdata = "$bucket/$xis";
		mkdir $xisdata unless -d $xisdir;
		if (-d $xisdata) {
		    $count = xis_init($xisdata);
		    if ($page) {
			my $npages = $count % $page;
			++$npages if $count % $npages;
			$page = $npages unless $page < $npages;
			xis_result();
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

sub xis_result {
    my $list = xis_page();
    # use p3 context engine code to create xml version of page

    my $xml = xis_xml($list);

    # return xml to client
    xml_header();
    exec 'cat', $xml;
}

sub xis_xml {
    my $list = shift;
    my $xmlfile = $list;
    $xmlfile =~ s/lst$/xml/;
    unless (-r $xmlfile) {
	system("cat $list | /Users/stinney/orc/bin/ce2 -3 -icbd/$lang -p $proj >$xmlfile");
    }
    $xmlfile;
}

#	system('xsltproc', '-stringparam', 'fragment', 'yes', '-stringparam', 'project', $p{'project'}, @offset_param, 
#		'-o', "$p{'tmpdir'}/results.html", '/Users/stinney/orc/lib/scripts/p3-ce-HTML.xsl', "$p{'tmpdir'}/content.xml")

sub xis_init {
    # use p3-pager code to extract xis list from tis/xis
    my $pubcbdxdir = "/Users/stinney/orc/pub/$proj/cbd/$lang";
    my $tis_file = "$pubcbdxdir/$lang.tis";
    if (-r $tis_file) {
	system("/Users/stinney/orc/bin/xisdb $tis_file $xis >$xisdata/xis.lst");
    } else {
	system("/Users/stinney/orc/bin/xis", '-f', 
		"/Users/stinney/orc/bld/$proj/$lang/$lang.xis", '-i', $xis, 
		'-o', "$xisdata/xis.lst");
    }
    my $wc = `wc -l $xisdata/xis.lst`;
    $wc =~ /^\s+(\d+)/;
    return $1;
}

# use p3-slicer code to extract page from xis list
sub xis_page {
    my $pagefile = "$xisdata/p${page}s$size.lst";
    unless (-r $pagefile) {
	if ($page == 1) {
	    system "head -$size $xisdata/xis.lst >$pagefile";
	} else {
	    my $from = (($page-1) * $size) + 1;
	    my $to = ($page * $size);
	    if ($to > $count) {
		$to = $count;
	    }
	    system "sed -n '${from},${to}p' $xisdata/xis.lst >$pagefile";
	}
    }
    $pagefile;
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
