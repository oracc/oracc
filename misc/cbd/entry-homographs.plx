#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC_BUILDS'}/lib";

my $default_project = `oraccopt`;
my $default_lang = 'sux';

use ORACC::CBD::Bases;
ORACC::CBD::Bases::bases_homographs_init('epsd2','sux')
    || die "$0: unable to initialize base homographs for epsd2/sux\n";

my @keys = (<>); chomp @keys;

my $project = '';
my $lang = '';
my $cgp = '';
my $firstkey = $keys[0];

if ($firstkey =~ /^\@(.*?)\%(.*?):(.*?)=?$/) {
    # also gets parsed in key handling loop
} else {
    # warn "$0: project/language not found in first key $firstkey; using defaults $default_project , $default_lang\n";
    $project = $default_project;
    $lang = $default_lang;
    ORACC::CBD::Bases::bases_homographs_init($project,$lang)
	|| die "$0: unable to initialize base homographs for $project/$lang\n";
}

foreach my $k (@keys) {
    if ($k =~ /^\@(.*?)\%(.*?):(.*?)=?$/) {
	$project = $1;
	$lang = $2;
	$cgp = $3;
    } else {
	$cgp = $k;
    }
    my @found = ORACC::CBD::Bases::bases_homographs_lookup($project,$lang,$cgp);
    if ($#found >= 0) {
	foreach my $sb (@found) {
	    my($sig,$b) = @$sb;
	    my @homographs = ORACC::CBD::Bases::bases_homographs_lookup('epsd2','sux',$sig);
	    if ($#homographs >= 0) {
		print "$cgp\t$$b\t$sig\n";
		foreach my $cb (@homographs) {
		    my($h_cgp,$h_base) = @$cb;
		    print "\t$h_cgp\t$h_base\n";
		}
	    }
	}
    }
}

1;
