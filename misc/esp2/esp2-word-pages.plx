#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;

GetOptions(
    );

if (-r '00etc/word-pages.lst') {
    my @wp = `cat 00etc/word-pages.lst`;
    foreach (@wp) {
	chomp;
	my($w,$wid) = split(/\t/,$_);
	word_page($w,$wid);
    }
}

sub word_page {
    my ($w,$wid) = @_;
    
    # extract the div from the word page into our directory of word divs
    #   (inline contained xincludes when doing this)

    my $art = "02www/cbd/akk-x-stdbab/$wid.html";
    my $out = "02www/espglo/akk-x-stdbab/art/$wid.xhtml";
    if (-r $art) {
	warn "processing $art to make body into div\n";
	system 'art2div.sh', $art, $out;
    }
    
    # write an ESP page that xincludes the word div
    my $espglo = "02www/espglo/akk-x-stdbab/$wid.html";
    my @wrdpage = `cat 02www/espglo/akk-x-stdbab/art/$wid.xhtml`;
    open(WP,">$espglo") || die;
    my $head = `cat 00etc/html-head.xml`; $head =~ s/\@\@WORD\@\@/$w/;
    print WP $head;
    # <xi:include xmlns:xi="http://www.w3.org/2001/XInclude" href="art/$wid.xhtml"/>
    print WP <<EOF;
	<body>
	<div id="Header"><span id="HeadTitle"><a href="../../index.html" title="NinMed: Medicine fit for a king">The Nineveh Medical Project</a></span></div>
	@wrdpage	    
    </body></html>
EOF
    close(WP);

    system 'chmod', '-R', 'o+r', '02www/espglo';
}

1;
