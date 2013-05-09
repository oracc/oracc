#!/usr/bin/perl
use warnings; use strict;
use utf8; use open ':utf8'; binmode(STDOUT, ':utf8');
use lib '@@ORACC@@/lib';
use Encode;
use File::Temp qw/tempdir tempfile/;
use CGI qw/:standard/;

my $xsl = "@@ORACC@@/lib/scripts";
my $vartmporacc = '/var/tmp/oracc/pager';
my $tmpdir = tempdir(DIR=>$vartmporacc);

warn "oase: tmpdir = $tmpdir\n";

save_data();
run_search();
return_data();

#######################################################

sub
return_data {
    print "Content-type: text/xml; charset=utf-8\n\n";
    exec 'cat', "$tmpdir/return.xml";
}

sub
run_search {
#    exec 'cp', "$tmpdir/search.xml", "$tmpdir/return.xml";
    system 'xsltproc', '-o', "$tmpdir/search.txt", "$xsl/xse-TEXT.xsl", "$tmpdir/search.xml";
    system 'se', '-x', $tmpdir;
    system 'xsltproc', '-o', "$tmpdir/return.xml", "$xsl/xse-RETURN.xsl", "$tmpdir/search.xml";
}

sub
save_data {
    my $data = param('POSTDATA');
    open(D,">$tmpdir/search.xml");
    binmode(D,':raw');
    print D $data;
    close(D);
}

1;
