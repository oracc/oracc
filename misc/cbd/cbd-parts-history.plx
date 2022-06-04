#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;

binmode STDIN, ':utf8';
binmode STDOUT, ':utf8';
binmode STDERR, ':utf8';

use lib "$ENV{'ORACC_BUILDS'}/lib";
use ORACC::Log::Util;
use ORACC::CBD::History;

$ORACC::CBD::History::return_unspaced = 1;

# autofix errors of the form:
#
#   A.dadag[00]FN does not match a known CF[GW] in `qpn.glo'
#
# which can be caused after cbdedit.

my $verbose = 0;

history_etc_init();
while (<>) {
    if (/\s(.*?)\s+does not match a known CF\[GW\]/) {
	my $fixme = $1;
	my($f,$l) = (/^(.*?):(.*?):/);
	parts_fix_part($f,$l,$fixme);
      }
}

log_term();
history_etc_term();

################################################################

sub parts_fix_part {
    my($f,$l,$fixme) = @_;
    my $line = log_get_line($f,$l);
    if ($line =~ /\@parts\s+(.*?)\s*$/) {
	my $parts = $1;
	1 while $parts =~ s/(\][A-Z]+)\s/$1\cA/;
	my @p = split(/\cA/,$parts);
	my $nth = 0;
	my $ncf = '';
	foreach my $p (@p) {
	    my $m = history_guess($p);
	    if ($m) {
		if ($m ne $p) {
		    # warn "$0: at parts\[$nth\]: found map for $p -> $m\n";
		    $p[$nth] = $m;
		    $ncf = $m; $ncf =~ s/\[.*$//;
		    last;
		}
	    } else {
		warn "$0: no map found for $p\n";
	    }
	    ++$nth;
	}
	$line = "\@parts @p\n";
	log_set_line($l,$line);
	if ($ncf) {
	    while (1) {
		$line = log_get_line($f,--$l);
		last if $line =~ /\@entry/;
	    }
	    # warn "$0: found entry $line";
	    chomp $line;
	    my($pre,$cfs,$post) = ($line =~ /(^.*\@entry\S*)\s+(.*?)\s*(\[.*)$/);
	    my @cfs = split(/\s+/,$cfs);
	    $cfs[$nth] = $ncf;
	    my $orig_cfs = $cfs;
	    $cfs = join(' ',@cfs);
	    # warn "$0: setting CFs from $orig_cfs to $cfs\n";
	    $line = "$pre $cfs $post\n";
	    log_set_line($l,$line);
	}
    } else {
	warn("$0: line $l in file $f should start with \@parts but it doesn't\n");
    }
}

1;
