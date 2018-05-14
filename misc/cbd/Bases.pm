package ORACC::CBD::Bases;
require Exporter;
@ISA=qw/Exporter/;

@EXPORT = qw/bases_hash bases_log bases_log_errors bases_process bases_stats/;

use warnings; use strict; use open 'utf8'; use utf8;

use ORACC::CBD::PPWarn;

my %log_errors = ();
my %stats = ();

sub bases_hash {
    my($arg,$is_compound) = @_;
    if ($arg =~ s/;\s*$//) {
	pp_warn("bases entry ends with semi-colon--please remove it");
    }
    my @bits = split(/;\s+/, $arg);

    my $alt = '';
    my $stem = '';
    my $pri = '';
    my %vbases = (); # this one is just for validation of the current @bases field
    my $pricode = 0;

    foreach my $b (@bits) {
	if ($b =~ s/^\*(\S+)\s+//) {
	    $stem = $1;
	} elsif ($b =~ /^\*/) {
	    $b =~ s/^\*\s*//;
	    pp_warn("misplaced '*' in \@bases");
	}
	if ($b =~ /\s+\(/) {
	    my $tmp = $b;
	    pp_warn("malformed alt-base in `$b'")
		if ($tmp =~ tr/()// % 2);
	    ($pri,$alt) = ($b =~ /^(.*?)\s+\((.*?)\)\s*$/);
	    if ($pri) {
		if ($pri =~ s/>.*$//) {
		    pp_warn("can't merge bases in a file with unresolved edits");
		}
		if ($pri =~ /\s/ && !$is_compound) {
		    pp_warn("space in base `$pri'");
		    $pri = $alt = '';
		} else {
#		    ++$bases{$pri};
#		    $bases{$pri,'*'} = $stem
#			if $stem;
		}
		if ($pri) {
		    if (defined $vbases{$pri}) {
			pp_warn("repeated base $pri");
		    } else {
			%{$vbases{$pri}} = ();
			$vbases{"$pri#code"} = ++$pricode;
		    }
		    foreach my $a (split(/,\s+/,$alt)) {
			if ($a =~ /\s/ && !$is_compound) {
			    pp_warn("space in alt-base `$a'");
			    $pri = $alt = '';
			} else {
			    if ($a) {
				if (${$vbases{$pri}}{$a}++) {
				    pp_warn("$pri has repeated alternate base $a");
				}
				# all alternates for this primary
				++${$vbases{"$pri#alt"}}{$a};
				# all alternates in this @bases
				if (defined ${${$vbases{'#alt'}}{$a}}) {
				    my $prevpri =  ${${$vbases{'#alt'}}{$a}};
				    pp_warn("alt $a already defined for primary $prevpri\n");
				} else {
				    ${${$vbases{'#alt'}}{$a}} = $pri;
				}
			    }
			}
		    }
		}
	    } else {
		pp_warn("syntax error in base with (...) [missing paren?]");
	    }
	} else {
	    if ($b =~ /\s/ && !$is_compound) {
		pp_warn("space in base `$b'");
		$pri = $alt = '';
	    } else {
#		++$bases{$b};
#		$bases{$b,'*'} = $stem
#		    if $stem;
		$pri = $b;
		$alt = '';
		if (defined $vbases{$pri}) {
		    pp_warn("repeated base $pri");
		} else {
		    %{$vbases{$pri}} = ();
		    $vbases{"$pri#code"} = ++$pricode;
		}
	    }
	}
    }
    %vbases;
}

sub bases_log{
    my $args = shift;
    open(L,$$args{'log'});
    while (<L>) {
	chomp;
	next unless /\(bases\)/;
	my($file,$line,$err) = (/^(.*?):(.*?):\s+\(bases\)\s+(.*?)\s*$/);
	if ($file eq $$args{'cbd'}) {
	    push @{$log_errors{$line}}, $err;
	}
    }
    close(L);
}

sub bases_log_errors {
    my $n = shift;
    if (defined $log_errors{$n}) {
	@{$log_errors{$n}};
    } else {
	()
    }
}

sub bases_stats {
    my($cfgw,$base) = @_;
    ++${$stats{$cfgw}}{$base};
}

sub bases_process {
    my %bd = @_;
    my @log_errors = bases_log_errors($bd{'line'});
    my %b = bases_hash($bd{'data'}, $bd{'compound'});
    open(D,'>bases.dump');
    use Data::Dumper;
    print D Dumper \%stats;
    print D Dumper \%b;
    close(D);
#    bases_fix(\%b,@log_errors);
    %stats = ();
}

1;
