package ORACC::SSA4::Common;
use ORACC::SSA4::Is;
require Exporter;
@ISA=qw/Exporter/;
@EXPORT = qw/head head_cfgw last_pos v_cfgw Vparse/;

sub
head {
    my $n = shift;
    if (!defined $$n{'type'}) {
	return $n;
    } elsif ($$n{'type'} eq 'leaf') {
	$n;
    } else {
	my $cref = $$n{'children'};
	if ($cref && ref($cref) eq 'ARRAY' && $#$cref >= 0) {
	    if (ORACC::SSA4::Is::is_v($n)) {
		foreach my $c (@$cref) {
		    return $c if $$c{'pos'} =~ /^V/;
		}
	    } elsif (ORACC::SSA4::Is::is_modp($$cref[0]) && $#$cref > 0) {
		head($$cref[1]);
	    } else {
		head($$cref[0]);
	    }
	} else {
	    return $n;
	}
    }
}

sub
head_cfgw {
    my $h = head($_[0]);
    $$h{'form'};
}

sub
last_pos {
    my $node = shift;
    my @c = @{$$node{'children'}};
    if ($c[$#c]{'type'} eq 'leaf') {
	return $c[$#c]{'pos'};
    } else {
	last_pos($c[$#c]);
    }
}

sub
v_cfgw {
    my $node = shift;
    if ($$node{'type'} eq 'phrase' && $$node{'label'} =~ /^VC-/) {
	my @c = @{$$node{'children'}};
	foreach my $c (@c) {
	    if ($$c{'pos'} =~ /^V/) {
		return $$c{'form'};
	    }
	}
    }
    0;
}

sub
Vparse {
    my $n = shift;
    if (ORACC::SSA4::Is::is_leaf($n) && $$n{'pos'} =~ /^V/) {
	return $$n{'parse'};
    } else {
	foreach my $c (@{$$n{'children'}}) {
	    if (ORACC::SSA4::Is::is_leaf($c) && $$c{'pos'} =~ /^V/) {
		return $$c{'parse'};
	    }
	}
	undef;
    }
}

1;
