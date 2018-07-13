package ORACC::CBD::Equivs;
require Exporter;
@ISA=qw/Exporter/;

@EXPORT = qw/equivs_resolve/;

use warnings; use strict; use open 'utf8'; use utf8;

use ORACC::CBD::PPWarn;
use ORACC::CBD::Senses;
use ORACC::CBD::Util;
use Data::Dumper;

my %base_senses = ();
my %base_entries = ();

sub equivs_resolve {
    my($args,$base_cbd,$cbd) = @_;
    my @base_cbd = @$base_cbd;
    my @cbd = @$cbd;
    %base_senses = senses_collect(@base_cbd);
    %base_entries = index_entries(%base_senses);
    for (my $i = 0; $i <= $#cbd; ++$i) {
	next unless $cbd[$i] =~ /^\@equiv/ && $cbd[$i] =~ /\%$$args{'#lang'}/;
	my $res = resolve_equiv($cbd[$i]);
	if ($res) {
	    $cbd[$i] .= " => @$res";
	}
    }
    #    print Dumper \%base_senses;
    @cbd;
}

# produce and index by CFs
sub index_entries {
    my %s = @_;
    my %ix = ();
    foreach my $s (keys %s) {
	next if $s =~ /insert$/;
	$s =~ /^(.*?)\s+\[/;
	push @{$ix{$1}}, $s;
    }
#    print Dumper \%ix;
    %ix;
}

sub resolve_equiv {
    my $c = shift;
    my ($e) = ($c =~ /%\S+\s+(.*?)\s*$/);
    $e =~ s/\?+$//;
    if ($e) {
	if ($base_entries{$e}) {
	    warn "found $e\n";
	    return $base_entries{$e};
	} else {
	    warn "nope $e\n";
	    return undef;
	}
    }
}

1;
