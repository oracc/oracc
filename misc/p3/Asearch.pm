package ORACC::P3::Asearch;
use warnings; use strict;
use utf8; use open 'utf8'; binmode(STDOUT, ':utf8');
use lib '@@ORACC@@/lib';
use ORACC::XML;
use Encode;

sub
asrch {
    my $tmpdir = shift;
    my $xsl = '@@ORACC@@/lib/scripts';
    load_xml("$tmpdir/search.xml");
    my $search_transform = load_xsl("$xsl/oas-SEARCH.xsl");
    $res = eval { $search_transform->transform($xdata) };
    unless ($res) {
	print STDERR "oas-SEARCH.xsl failed\n";
	print STDERR "---\nTransform reports:\n$@\n---\n";
	exit 1; # return error
    } else {
	print STDERR " ok\n" if $verbose;
    }

    $search = $search_transform->output_string($res);
    Encode::_utf8_on($search);
    run_search($search);

    my $return_transform = load_xsl("$xsl/oas-RETURN.xsl");
    $res = eval { $return_transform->transform($xdata, 
					       results=>"'$tmpdir/results.xml'",
					       search=>"'$search'") };
    unless ($res) {
	print STDERR "oas-RETURN.xsl failed\n";
	print STDERR "---\nTransform reports:\n$@\n---\n";
	exit 1; # return error
    } else {
	print STDERR " ok\n" if $verbose;
    }
}

sub
run_search {
    open(D,">$tmpdir/search.txt");
    binmode(D,':raw');
    print D $_[0];
    close(D);
    my $use_unicode = $values{'use-unicode'}; ### port from xdata
    my $arg8 = '';
    $arg8 = "8" if $use_unicode eq 'true';
    system '@@ORACC@@/bin/se', "-s${arg8}2x", $tmpdir;
}
