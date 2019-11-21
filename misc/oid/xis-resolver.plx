#!/usr/bin/perl
use warnings; use strict; use integer;

warn "xis-resolver.plx ARGV=@ARGV\n";

my ($proj,$lang,$xis,$page,$size,$mode,$all) = @ARGV;

xml_error("no project in request") unless $proj;
xml_error("no language in request") unless $lang;
xml_error("no xis in request") unless $xis;
xml_error("no page in request") unless defined $page;
xml_error("no size in request") unless defined $size;
xml_error("no mode in request") unless defined $mode;

$page = 1 unless defined $page;
$size = 25 unless $size;

my $count = -1;
my $projwdir = "/Users/stinney/orc/www/$proj";
my $projpdir = "/Users/stinney/orc/pub/$proj";
my $tisfile = "$projpdir/cbd/$lang/$lang.tis";

xml_error("bad page specification") unless $page==0 || $page =~ /^\d+/;
xml_error("bad size specification") unless $size==0 || $size =~ /^(25|50|100)$/;
xml_error("bad mode specification") unless $mode eq 'xml' || $mode eq 'html';

unless (-r $tisfile) {
    xml_error("bad project in request") unless -d "/Users/stinney/orc/lib/$proj";
    xml_error("bad language in request") unless -d "$projpdir/cbd/$lang";
    xml_error("no public language data") unless -d "$projpdir/cbd/$lang.tis";
}

my $baselang = $lang; $baselang =~ s/-.*$//;
my $bucket = $xis; $bucket =~ s/^$baselang\.//; $bucket =~ s/^(.....).*$/$1/;
my $bucketpath = "$projwdir/cbd/$lang/xis/$bucket"; warn "bucketpath=$bucketpath\n";
my $xisdata = "$bucketpath/$xis";
my $instances = "$xisdata/instances.lst";

unless (-r $instances) {
    xml_error("no web language data") unless -d "$projwdir/cbd/$lang";
    $count = `/Users/stinney/orc/bin/xisdb $tisfile -c $xis`;
    xml_error("bad xis in request") unless $count;
    my $xisdir = "$projwdir/cbd/$lang/xis";
    mkdir $xisdir unless -d $xisdir;
    xml_error("failed to find or create xis cache") unless -d $xisdir;
    mkdir $bucketpath unless -d $bucketpath;
    xml_error("failed to find or create xis bucket") unless -d $bucketpath;
    mkdir $xisdata unless -d $xisdata;
    xml_error("failed to find or create xis data") unless -d $xisdata;
    xis_init($xisdata);    
} else {
    my $n = `cat $xisdata/count`; chomp $n;
    $count = $n;
}

if ($all eq 'all' && $count < 100) {
    xis_result();
} elsif ($page) {
    my $npages = $count / $size;
    ++$npages if $count % $size;
    $page = $npages unless $page <= $npages;
    xis_result();
} else {
    xml_count($count);
}

###############################################################################

sub xis_result {
    my $list = xis_page();

    # use p3 context engine code to create xml version of page
    my $xml = xis_xml($list);

    $xml =~ s/xml$/html/ if $mode eq 'html';

    # return xml to client
    xml_header();
    exec 'cat', $xml;
}

sub xis_xml {
    my $list = shift;
    my $htmlfile = $list;
    my $xmlfile = $list;
    $htmlfile =~ s/lst$/html/;
    $xmlfile =~ s/lst$/xml/;
    unless (-r $xmlfile) {
	system("/Users/stinney/orc/bin/wm -p$proj -l -i$list | /Users/stinney/orc/bin/ce_xtf -3 -l -p $proj >$xmlfile");
	system('xsltproc',
	       '-stringparam', 'divwrap', 'yes',
	       '-stringparam', 'fragment', 'yes',
	       '-stringparam', 'project', $proj,
	       '-o', $htmlfile, '/Users/stinney/orc/lib/scripts/p3-ce-HTML.xsl', $xmlfile)
    }
    $xmlfile;
}

sub xis_init {
    system("/Users/stinney/orc/bin/xisdb $tisfile $xis >$instances");
    open(C,">$xisdata/count"); print C $count; close(C);
}

# use p3-slicer code to extract page from xis list
sub xis_page {
    my $pagefile = '';
    if ($all eq 'all') {
	$pagefile = $instances;
    } else {
	$pagefile = "$xisdata/p${page}s$size.lst";
	unless (-r $pagefile) {
	    if ($page == 1) {
		system "head -$size $instances >$pagefile";
	    } else {
		my $from = (($page-1) * $size) + 1;
		my $to = ($page * $size);
		if ($to > $count) {
		    $to = $count;
		}
		system "sed -n '${from},${to}p' $instances >$pagefile";
	    }
	}
    }
    $pagefile;
}

sub xml_count {
    xml_header();
    print '<count>',$count,'</count>',"\n";
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
