package ORACC::CBD::Bases;
require Exporter;
@ISA=qw/Exporter/;

@EXPORT = qw/pp_validate v_project v_lang v_form v_is_entry v_set_cfgw/;

use warnings; use strict; use open 'utf8'; use utf8;

sub bases_hash {
    my($arg) = shift;
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
		    push @{$data{'edit'}}, pp_line()-1;
		}
		if ($pri =~ /\s/ && !$is_compound) {
		    pp_warn("space in base `$pri'");
		    $pri = $alt = '';
		} else {
		    ++$bases{$pri};
		    $bases{$pri,'*'} = $stem
			if $stem;
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
		++$bases{$b};
		$bases{$b,'*'} = $stem
		    if $stem;
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
