#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use ORACC::XML;
use ORACC::NS;

use Getopt::Long;

my $glo = '';
my $lang = '';
my $project = '';
my $webdir = '';

GetOptions(
    'glossary:s'=>\$glo,
    'lang:s'=>\$lang,
    'project:s'=>\$project,
    'webdir:s'=>\$webdir,
    );

# This script reads a cbd looking for file inclusions; it takes care
# of ensuring the inclusions are converted to XHTML divs and are placed
# in the 02www/cbd/<LANG>/div directory with the word ID as the basename
# of the file. A two digit number is added to this basename so multiple
# files can be included, e.g., div/x000001-00.xhtml.

# The script operates on the XML version of the glossary articles.xml
# which is processed in place to replace <file name="FILENAME"/> tags
# with the proper xinclude.  This is done during l2p3, where the web
# versions of the cbds is being created.

$project = `oraccopt` unless $project;

die "$0: must give -l <LANGUAGE> on command line\n" unless $lang || $glo;

# load articles.xml
$glo = "01bld/$lang/articles.xml" unless $glo;

my $x = undef;
if (-r $glo) {
    $x = load_xml($glo);
    unless ($lang) {
	$lang = $x->getDocumentElement()->firstChild()->getAttribute('xml:lang');
	warn "lang set to $lang\n";
    }
} else {
    die "$0: no such file $glo\n";
}

$webdir = '01bld/www' unless $webdir;
my $outbase = "$webdir/cbd/$lang/fls";

system 'mkdir', '-p', $outbase;

# collect file tags
my @filenodes = tags($x, $CBD, 'file');

foreach my $fnode (@filenodes) {
    # convert file if necessary
    my $wid = xid($fnode->parentNode());
    my $outname = "$outbase/$wid-00.xhtml";
    my $fname = $fnode->getAttribute('name');
    my $xiname = "$ENV{'ORACC_BUILDS'}/$project/02www/cbd/$lang/fls/$wid-00.xhtml";
    my $xi = $x->createElement('include');
    $xi->setNamespace('http://www.w3.org/2001/XInclude','xi',1);
    $xi->setAttribute('href',$xiname);
    $fnode->parentNode()->replaceChild($xi,$fnode);
    my $type = type_of($fname);
    if (!$type) {
	warn "$0: type-detection failed on file $fname\n";
    } elsif ($type eq 'UNKNOWN') {
	warn "$0: file $fname has unknown type\n";
    } elsif ($type eq 'CBD') {
	warn "$0: file $fname is illegal type $CBD\n";
    } elsif ($type eq 'OTF' || $type eq 'ESP') {
	warn "processing $fname as $type to $outname\n";
	if ($type eq 'OTF') {
	    system 'otf2div.sh', $fname, $outname;
	} elsif ($type eq 'ESP') {
	    # NOTE: There is a technical issue here which is that we
	    # need to induct the post-ESP-processing versions of ESP
	    # files but l2p3 runs before o2-portal.
	    system 'esp2div.sh', $fname, $outname;
	} else {
	    warn "$0: unhandled type $type\n";
	}
    }
}

# duplicate the built fls data in the webdir so it gets recopied at
# install time--this means that further processing of articles.xml
# works with the 02www version but then when that is hosed a
# replacement set of files is installed again.

system 'mkdir', '-p', "01bld/www/cbd/$lang";
system 'cp', '-a', "02www/cbd/$lang/fls", "01bld/www/cbd/$lang";

open(X,">$glo");
binmode STDOUT, ':raw';
my $xs = $x->toString();
Encode::_utf8_on($xs);
print X $xs;
close(X);

################################################################################

sub type_of {
    my $f = shift;
    my $type = '';
    if ($f =~ /\.otf$/) {
	return 'OTF';
    } else {
	open(F,$f) || die "$0: unable to open file $f to determine type\n";
	while (<F>) {
	    s/^<\?.*?\?>//;
	    next if /^\s*$/;
	    if (/<esp:page/) {
		$type = 'ESP';
		last;
	    } elsif (/<entries/ || /<articles/) {
		$type = 'CBD';
		last;
	    } else {
		$type = 'UNKNOWN';
		last;
	    }
	}
	close(F);
    }
    return $type;
}

1;
