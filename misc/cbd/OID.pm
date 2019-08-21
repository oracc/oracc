package ORACC::CBD::OID;
require Exporter;
@ISA=qw/Exporter/;

@EXPORT = qw/oid_add/;

use warnings; use strict; use open 'utf8'; use utf8;

use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;
use ORACC::OID;
use Data::Dumper;

sub oid_add {
    my($args,@cbd) = @_;
    my $l = $$args{'#lang'} || lang();
    $l =~ s/-.*$//;
    warn("lang=$l\n");
    my %oid = oid_init($l);
#    print Dumper \%oid;
    for (my $i = 0; $i <= $#cbd; ++$i) {
	if ($cbd[$i] =~ /^$acd_rx?\@entry/) {
	    my $j = end_entry($i,@cbd);
	    if (!has_oid($i,$j,@cbd)) {
		my ($head) = ($cbd[$i] =~ /^$acd_rx?\@entry\S*\s+(.*?)\s*$/);
		$head =~ s/\s+\[/[/; $head =~ s/\]\s+/]/;
		if ($oid{$l,$head}) {
		    # pp_notice("adding OID $oid{$l,$head} for $head");
		    $cbd[$i] .= "\n\@oid $oid{$l,$head}";
		} else {
		    pp_notice("no OID for $head");
		}
	    }
	}
    }
    @cbd;
}

sub end_entry {
    my($i,@c) = @_;
    while ($c[$i] !~ /^\@end\s+entry/) {
	++$i;
    }
    return $i;
}

sub has_oid {
    my($i,$j,@c) = @_;
    while ($i < $j) {
	return 1 if $c[$i] =~ /^\@oid/;
	++$i;
    }
    0;
}

1;

