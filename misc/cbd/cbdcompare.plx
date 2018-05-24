#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";

use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;
use ORACC::CBD::Hash;
use ORACC::CBD::Words;

my %args = pp_args();

if ($args{'base'}) {
    my @cbd = setup_cbd(\%args,$args{'base'});
    if (pp_hash(\%args,@cbd)) {
	my $cbdname = ${$ORACC::CBD::data{'cbds'}}[0];
	open(B,'>base.dump');
	use Data::Dumper;
	print B Dumper \%{$ORACC::CBD::data{$cbdname}};
	close(B);
    } else {
	pp_diagnostics();
	exit 1;
    }
} else {
    die "cbdcompare.plx: must give base glossary with -base\n";
}

if (pp_hash(\%args)) {
    my $cbdname = ${$ORACC::CBD::data{'cbds'}}[1];
    open(D,'>incoming.dump');
    use Data::Dumper;
    print D Dumper \%{$ORACC::CBD::data{$cbdname}};
    close(D);
} else {
    pp_diagnostics();
}

if (pp_status()) {
    pp_diagnostics();
    exit 1;
} else {
    warn "cbdwords.plx: words in $args{'cbd'} have no detectable duplicates or overlaps.\n";
    exit 0;
}

1;
