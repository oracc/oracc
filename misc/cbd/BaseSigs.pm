package ORACC::CBD::BaseSigs;

require Exporter;
@ISA=qw/Exporter/;

@EXPORT = qw/basesigs_load/;

use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;
$ORACC::CBD::BaseSigs::verbose = 0;
use lib "$ENV{'ORACC_BUILDS'}/lib";

# First arg is either a literal base_sigs file name (e.g., 01bld/sux/base-sigs.tab)
# or a project:lang pair, e.g., epsd2:sux.
#
# Second arg is boolean, sort base sigs or leave them ordered as is; sorting them is
# useful for working with very early corpora, e.g., ed3a.
#
# Returns a hash with the data loaded in the format:
#
# 'o0000099.o0002398' => [
#                          '{d}uttu==Uttuk [1] DN'
#                        ],
# 'o0000099.o0000556' => [
#                          'AN-TU==AN.TU [0] DN',
#                          "{d}tur\x{2085}==Nintur [1] DN"
#                        ],
# 'o0000099.o0000497' => [
#                          '{d}muati==Muati [1] DN',
#                          '{d}muati==Nabu [1] DN',
#                          "{d}\x{161}ullat\x{2082}==\x{160}ullat [1] DN"
#                        ],
#
# AND ALSO
#
# 'umah [blow] N' => {
#                       'o0001786' => {
#                                       'umah' => 1
#                                     }
#                    }
#
# In the latter case, the deepest hash may have multiple members for, e.g., @allow be₇=ne.
#


my %global_e = ();

sub basesigs_load {
    my($p,$sort,$esort) = @_;
    my %seen = ();
    my %t = ();
    
    unless (-r $p) {
	my($proj,$lang) = ($p =~ /^(.*?):(.*?)$/);
	if ($proj) {
	    $p = "$ENV{'ORACC_BUILDS'}/$proj/01bld/$lang/base-sigs.tab";
	}
    }
    open(P,$p) || die "$0: can't load Base Sigs $p\n";
    while (<P>) {
	chomp;
	my @f = split(/\t/,$_);
	my $r = "$f[1]==$f[0]";
	next if $seen{$f[0],$r}++;
	if ($sort) {
	    my $s = join('.',sort split(/\./,$f[2])) if $sort;
	    warn "$0: input: using $s for $f[2]\n"
		if ($ORACC::CBD::BaseSigs::verbose && $s ne $f[2]);
	    $f[2] = $s;
	}
	push @{$t{$f[2]}}, $r;
	++${${$t{$f[0]}}{$f[2]}}{$f[1]};
    }
    close(P);
    if ($esort) {
	%t = esort($esort,\%t);
	open(T,'>t.dump') || die;
	print T Dumper \%t;
	close(T);
    }
    %t;
}

sub bsigcmp {
    if (defined($global_e{$a}) && defined($global_e{$b})) {
	return $global_e{$b} <=> $global_e{$a};
    } elsif (defined($global_e{cgp($a)}) && defined($global_e{cgp($b)})) {
	return $global_e{cgp($b)} <=> $global_e{cgp($a)};
    } else {
	0;
    }
}

sub cgp {
    $_[0] =~ /==(.*?)$/;
    $1;
}

sub epack {
    my @n = ();
    foreach my $e (@_) {
	$e =~ s/\s+(\[.*?\])\s+/$1/;
	push @n, $e;
    }
    @n;
}

sub esort {
    my($eref,$tref) = @_;

    %global_e = %$eref;
    
    my %t = %$tref;
    foreach my $t (keys %t) {
	my @t = epack(@{$t{$t}});
	if ($#t > 0) {
	    # warn "===\npre: @t\n";
	    @t = sort { &bsigcmp; } @t;
	    # warn "pst: @t\n";
	}
	@{$t{$t}} = @t;
    }
    %t;
}

1;
